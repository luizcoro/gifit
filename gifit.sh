#!/bin/bash

MODE=0
SPEED_FACTOR=1
SCALE_FACTOR=1
SLEEP_TIME=0.1
WINDOW_GEOMETRY=0
SCREENKEY_ENABLE=0

TMP_DIR=$([ $GIFIT_TMP_DIR ] && echo $(mktemp -d -p $GIFIT_TMP_DIR) || echo $(mktemp -d))
GIF_DIR=$([ $GIFIT_GIF_DIR ] && echo $GIFIT_GIF_DIR || echo ".")

assert_commands_exist()
{
    for command in $@; do
        command -v $command >/dev/null 2>&1 || { echo >&2 "$command needs to be installed to enable this feature.";rm -rf $TMP_DIR; exit 1; }
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
    rm -rf $TMP_DIR
    exit 1
}

assert_parameter_between()
{
    if [ $(echo "$2 <= $1 && $1 <= $3" | bc) == 0 ]; then
        echo >&2 "Parameter $4 needs to be between $2 and $3."
        rm -rf $TMP_DIR
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
}

do_shots()
{
    if [ -t 0 ]; then stty -echo -icanon -icrnl time 0 min 0; fi

    count=0
    keypress=''
    real_sleep=0

    while [[ "$keypress" != "q" ]]; do

        if [ $(echo "$real_sleep > 0" | bc) -eq 1 ]; then
            sleep $real_sleep
        fi

        last_date=$(date +%s%N)

        scrot "$TMP_DIR/$(printf %05d $count).pnm"
        echo -ne "Press [q] to stop recording\ttotal shots = $(($count+1))\r"
        let count+=1
        keypress="`cat -v`"

        current_date=$(date +%s%N)

        real_sleep=$(echo "scale=9;($SLEEP_TIME-(($current_date-$last_date)/1000000000))" | bc)
    done

    if [ -t 0 ]; then stty sane; fi
    echo -ne "\n\n"
}

scale_shots(){
    echo "Scaling..."
    gm mogrify -scale "$(echo "$SCALE_FACTOR*100"| bc)%" "$TMP_DIR/*"
}

remove_shots()
{
    assert_commands_exist yad
    rm $(yad  --title="Select a file to remove" --add-preview --multiple --image-filter --splash --filename="$TMP_DIR/" --separator=" " --file-selection)
}

shots_to_gif()
{
    echo "Making gif..."

    if [ $MODE -ne 0 ]; then
    gm mogrify -crop $WINDOW_GEOMETRY\! $TMP_DIR/*.pnm
    fi

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

    paplay ./race-countdown.ogg

    if [ "$SCREENKEY_ENABLE" == 1 ]; then
        screenkey -t 1 -s small -g $WINDOW_GEOMETRY --opacity 0.7; do_shots; killall screenkey
    else
        do_shots
    fi

    echo "Do you wanna remove some shots? (Y/n)"
    read remove

    if [ "$remove" != "n" ]; then
        remove_shots
    fi

    if [ "$SCALE_FACTOR" != "1" ]; then
        scale_shots
    fi

    shots_to_gif

    rm -rf $TMP_DIR

    echo "Thanks for using this script."
}

main $@
