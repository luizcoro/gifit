#!/bin/bash

MODE=0
SPEED_FACTOR=1
SCALE_FACTOR=1
SLEEP_TIME=0.1
WINDOW_GEOMETRY=0
PARSED_WINDOW_GEOMETRY=0
SCREENKEY_ENABLE=0

ZOOM=2
ZOOM_SPEED=3

COUNTDOWN_SOUND="sounds/race-countdown.ogg"
ZOOM_IN_SOUND="sounds/zoom_in.ogg"
ZOOM_OUT_SOUND="sounds/zoom_out.ogg"

GIFIT_GET_INPUT="gifitgetinput/gifitgetinput"

GEOMETRIES=()

TMP_DIR=$([ $GIFIT_TMP_DIR ] && echo $(mktemp -d -p $GIFIT_TMP_DIR) || echo $(mktemp -d))
GIF_DIR=$([ $GIFIT_GIF_DIR ] && echo $GIFIT_GIF_DIR || echo ".")

FIFO=$(mktemp -p $TMP_DIR)

trap 'killall' EXIT

killall()
{
    rm -rf $TMP_DIR
    trap '' INT TERM     # ignore INT and TERM while shutting down
    kill -TERM 0         # fixed order, send TERM not INT
    wait
}

assert_commands_exist()
{
    for command in $@; do
        command -v $command >/dev/null 2>&1 || { echo >&2 "$command needs to be installed to enable this feature."; exit 1; }
    done
}

##WHY DOES THE SCREENKEY -V COMMAND USE STDERR TO OUTPUT VERSION
assert_screenkey_version()
{
    if screenkey -v &> /dev/null; then
        screenkey -v &> screenkey_version
        screenkey_version=$(cat screenkey_version)
        rm screenkey_version

        if [ $(echo "$screenkey_version >= 0.8" | bc) == 1 ]; then
            return
        fi
    fi

    echo >&2 "screenkey needs to be version 0.8 or later."
    exit 1
}

assert_parameter_between()
{
    if [ $(echo "$2 <= $1 && $1 <= $3" | bc) == 0 ]; then
        echo >&2 "Parameter $4 needs to be between $2 and $3."
        exit 1
    fi
}

parse_args()
{
    while getopts ":n:v:t:wsk" opt; do
        case $opt in
        n)
            assert_parameter_between $OPTARG 1 30 "-n"
            SLEEP_TIME=$(echo "1/$OPTARG" | bc -l)
            ;;
        v)
            assert_parameter_between $OPTARG 0.1  4 "-v"
            SPEED_FACTOR=$OPTARG
            ;;
        t)
            assert_parameter_between $OPTARG 0.1  4 "-t"
            SCALE_FACTOR=$OPTARG
            ;;
        w)
            MODE=1
            ;;
        s)
            assert_commands_exist xrectsel
            MODE=2
            ;;
        k)
            assert_commands_exist screenkey
            assert_screenkey_version
            SCREENKEY_ENABLE=1
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit 1
            ;;
        esac
    done

    shift "$((OPTIND-1))"
}

parse_geometry()
{
    g=$1

    wxh=${g%%+*}
    xy=${g#*+}

    echo ${xy/+/ } ${wxh/x/ }
}

get_window_geometry()
{
    if [ $MODE -eq 1 ]; then
        unset x y w h
        eval $(xwininfo |
          sed -n -e "s/^ \+Absolute upper-left X: \+\([0-9]\+\).*/x=\1/p" \
                 -e "s/^ \+Absolute upper-left Y: \+\([0-9]\+\).*/y=\1/p" \
                 -e "s/^ \+Width: \+\([0-9]\+\).*/w=\1/p" \
                 -e "s/^ \+Height: \+\([0-9]\+\).*/h=\1/p" )
        WINDOW_GEOMETRY="$w""x$h+$x+$y"
    elif [ $MODE -eq 2 ]; then
        WINDOW_GEOMETRY=$(xrectsel)
    else
        WINDOW_GEOMETRY=$(xwininfo -root | grep 'geometry' | awk '{print $2;}')
    fi

    PARSED_WINDOW_GEOMETRY=$(parse_geometry $WINDOW_GEOMETRY)
}

get_final_zoom_geometry()
{
    fw=$(($3 / $ZOOM))
    fh=$(($4 / $ZOOM))

    fx=$(($5 - ($fw/2)))
    fy=$(($6 - ($fh/2)))

    if [[ $fx -lt $1 ]]; then
        fx=$1
    fi

    if [[ $fy -lt $2 ]]; then
        fx=$2
    fi

    printf "%.0fx%.0f+%.0f+%.0f" $fw $fh $fx $fy
}



get_zoom_interpolation_needed()
{
    if [ $3 -lt $7 ]; then
        echo $(echo "scale=9;($7/$3)-1" | bc)
    else
        echo $(echo "scale=9;($3/$7)-1" | bc)
    fi
}

get_interpolated_zoom_towards()
{
    fx=$(echo "(($5 - $1) * $SLEEP_TIME * $ZOOM_SPEED) + $1" | bc)
    fy=$(echo "(($6 - $2) * $SLEEP_TIME * $ZOOM_SPEED) + $2" | bc)
    fw=$(echo "(($7 - $3) * $SLEEP_TIME * $ZOOM_SPEED) + $3" | bc)
    fh=$(echo "(($8 - $4) * $SLEEP_TIME * $ZOOM_SPEED) + $4" | bc)

    printf "%.0fx%.0f+%.0f+%.0f" $fw $fh $fx $fy
}

get_zoom_towards()
{
    g1=$(parse_geometry $1)
    g2=$(parse_geometry $2)

    # if [ $(echo "$(get_zoom_interpolation_needed $g1 $g2) < 0.01" | bc) == 1 ]; then
    #     echo $2
    # else
        echo $(get_interpolated_zoom_towards $g1 $g2)
    # fi
}

do_shots()
{
    if [ -t 0 ]; then stty -echo -icanon -icrnl time 0 min 0; fi

    count=0
    keypress=''
    real_sleep=0

    zoomed=0

    current_geometry=$WINDOW_GEOMETRY
    target_geometry=$WINDOW_GEOMETRY

    $GIFIT_GET_INPUT $FIFO &

    while [[ "$keypress" != "q" ]]; do

        if [ $(echo "$real_sleep > 0" | bc) -eq 1 ]; then
            sleep $real_sleep
        fi

        last_date=$(date +%s%N)

        scrot "$TMP_DIR/$(printf %05d $count).pnm"
        GEOMETRIES[$count]=$current_geometry

        echo -ne "Press [q] to stop recording\ttotal shots = $(($count+1))\r"
        let count+=1
        keypress="`cat -v`"

        fifo_entry=$(head -1 $FIFO)

        if [[ ! -z $fifo_entry ]]; then

            if [ "$fifo_entry" == "q" ]; then
                keypress=$fifo_entry
            else
                if [ $zoomed == 0 ]; then
                    target_geometry=$(get_final_zoom_geometry $PARSED_WINDOW_GEOMETRY $fifo_entry)
                    paplay $ZOOM_IN_SOUND &
                    zoomed=1
                else
                    target_geometry=$WINDOW_GEOMETRY
                    paplay $ZOOM_OUT_SOUND &
                    zoomed=0
                fi
            fi

            > $FIFO
        fi

        current_geometry=$(get_zoom_towards $current_geometry $target_geometry)
        echo $current_geometry
        current_date=$(date +%s%N)

        real_sleep=$(echo "scale=9;($SLEEP_TIME-(($current_date-$last_date)/1000000000))" | bc)
    done

    if [ -t 0 ]; then stty sane; fi
    echo -ne "\n\n"
}

get_file_id()
{
    f=$1
    filename=${f##*/}
    echo $((10#${filename%.*}))
}


crop_shots_if_needed()
{
    echo "Croping..."

    if [ $MODE -ne 0 ]; then
        gm mogrify -crop $WINDOW_GEOMETRY\! $TMP_DIR/*.pnm
    fi

    wxh=${WINDOW_GEOMETRY%%+*}

    for file in $TMP_DIR/*.pnm; do
        id=$(get_file_id $file)

        if [ $WINDOW_GEOMETRY != ${GEOMETRIES[$id]} ]; then
            gm mogrify -crop ${GEOMETRIES[$id]}\! -resize $wxh\! $file
        fi
    done
}

scale_shots()
{
    echo "Scaling..."
    gm mogrify -scale "$(echo "$SCALE_FACTOR*100"| bc)%" "$TMP_DIR/*"
}

remove_shots()
{
    assert_commands_exist yad
    files=$(yad  --title="Select a file to remove" --add-preview --multiple --image-filter --splash --filename="$TMP_DIR/" --separator=" " --file-selection 2> /dev/null)

    if [[ ! -z $files ]]; then
        rm $files
    fi
}

shots_to_gif()
{
    echo "Making gif..."

    gm convert +dither -colorspace YUV -colors 63 -fuzz 10% -delay $(echo "100*$SLEEP_TIME/$SPEED_FACTOR" | bc -l) $TMP_DIR/*.pnm $TMP_DIR/out.gif

    if [ "$?" -ne "0" ]; then
    echo "Gif failed. Try to set the variable GIFIT_TMP_DIR in your ~/.bashrc. Choose a temporary folder with alot of space!"
    exit 1
    fi

    echo "Optimizing..."
    gifsicle -O3 --method blend-diversity --colors 63  $TMP_DIR/out.gif -o $GIF_DIR/$(date +%F-%T).gif

    echo "Gif OK!"
}

main ()
{
    assert_commands_exist xwininfo scrot gm gifsicle

    parse_args $@
    get_window_geometry

    paplay $COUNTDOWN_SOUND

    if [ "$SCREENKEY_ENABLE" == 1 ]; then
        screenkey -t 1 -s small -g $WINDOW_GEOMETRY --opacity 0.7; do_shots; killall screenkey
    else
        do_shots
    fi

    crop_shots_if_needed

    echo "Do you wanna remove some shots? (y/N)"
    read -n 1 remove

    echo ""

    if [[ $remove == "y" || $remove == "Y" ]]; then
        remove_shots
    fi

    if [ "$SCALE_FACTOR" != "1" ]; then
        scale_shots
    fi

    shots_to_gif

    echo "Thanks for using this script."
}

main $@
