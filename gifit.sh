#!/bin/bash

DELAYS=()
MODE=0
SPEED_FACTOR=1
SCALE_FACTOR=1
SLEEP_TIME=0.1
WINDOW_GEOMETRY=0
TMP_DIR=$([ $GIFIT_TMP_DIR ] && echo $(mktemp -d -p $GIFIT_TMP_DIR) || echo $(mktemp -d))
GIF_DIR=$([ $GIFIT_GIF_DIR ] && echo $GIFIT_GIF_DIR || echo ".")

while getopts ":n:v:t:ws" opt; do
  case $opt in
    n)
      SLEEP_TIME=$(echo "1/$OPTARG" | bc -l)
      ;;
    v)
      SPEED_FACTOR=$OPTARG
      ;;
    t)
      SCALE_FACTOR=$OPTARG
      ;;
    w)
      MODE=1
      ;;
    s)
      MODE=2
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

get_window_geometry(){
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

do_shots(){
  if [ -t 0 ]; then stty -echo -icanon -icrnl time 0 min 0; fi

  count=0
  keypress=''
  last_date=$(date +%s%2N)

  while [[ "$keypress" != "q" ]]; do
    scrot "$TMP_DIR/$(printf %05d $count).pnm"

    echo -ne "Press [q] to stop recording\ttotal shots = $(($count+1))\r"

    current_date=$(date +%s%2N)
    DELAYS[$count]=$(($current_date-$last_date))
    real_sleep=$(echo "scale=2;($SLEEP_TIME-(${DELAYS[$count]}/100))" | bc)
    echo "count = $count, sleep = $SLEEP_TIME, sleeped = ${DELAYS[$count]}, real = $real_sleep" >> log

    if [ $(echo "$real_sleep > 0" | bc) -eq 1 ]; then
        echo "entered" >> log
        sleep $real_sleep
    else
        echo "not entered" >> log
    fi

    last_date=$(date +%s%2N)

    let count+=1
    keypress="`cat -v`"
  done

  if [ -t 0 ]; then stty sane; fi
  echo -ne "\n\n"
}

scale_shots(){
    echo "Scaling..."
    gm mogrify -scale "$(echo "$SCALE_FACTOR*100"| bc)%" "$TMP_DIR/*"
}

get_index_from_file()
{
    file=$1
    filename=${file##*/}
    index=$((10#${filename%.*}))

    echo $index
}

remove_shots()
{
    files=$(yad  --title="Select a file to remove" --add-preview --multiple --image-filter --splash --filename="$TMP_DIR/" --file-selection)

    IFS='|' read -ra IMGS <<< "$files"

    for img in "${IMGS[@]}"; do
        rm $img
    done
}

shots_to_gif(){

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

get_window_geometry
paplay ./race-countdown.ogg
do_shots

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
