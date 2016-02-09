#!/bin/bash

PNG_DIR=$(mktemp -d)
DELAYS=()
MODE=0
SLEEP_TIME=0.066666666666
WINDOW_GEOMETRY=0
OPTIONAL_TMP_DIR=$(printenv GIFIT_TMP_DIR)
OPTIONAL_GIF_DIR=$(printenv GIFIT_GIF_DIR)
while getopts ":n:ws" opt; do
  case $opt in
    n)
      SLEEP_TIME=$(echo "1/$OPTARG" | bc -l)
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
    maim -g $1 "$PNG_DIR/$(printf %05d $count).png"
    current_date=$(date +%s%2N)
    DELAYS[$count]=$(($current_date-$last_date))
    last_date=$current_date

    echo -ne "Press [q] to stop recording\ttotal shots = $count\r"

    sleep $SLEEP_TIME
    let count+=1
    keypress="`cat -v`"
  done

  if [ -t 0 ]; then stty sane; fi
  echo -ne "\n\n"
}

shots_to_gif(){
  _CONVERT="convert -limit memory 2GiB -limit map 4GiB "

  if [ -n "$OPTIONAL_TMP_DIR" ]; then
    _CONVERT="${_CONVERT} -define registry:temporary-path=$OPTIONAL_TMP_DIR "
  fi

  count=0

  for f in $PNG_DIR/*
    do
     _CONVERT="${_CONVERT} -delay ${DELAYS[$count]} $f "
     (( count++ ))
    done

  new_gif=$PNG_DIR/out.gif
  _CONVERT="${_CONVERT} -loop 0 -fuzz 2% -layers Optimize -colors 32 $new_gif"
  echo "Making gif..."
  eval $_CONVERT

  if [ "$?" -ne "0" ]; then
    echo "Gif failed. Try to set the variable GIFIT_TMP_DIR in your ~/.bashrc. Choose a temporary folder with alot of space!"
    exit 1
  fi


  if [ -n "$OPTIONAL_GIF_DIR" ]; then
    gifsicle --colors 32 -O3 $new_gif -o "$OPTIONAL_GIF_DIR/$(date +%F-%T).gif"
  else
    gifsicle --colors 32 -O3 $new_gif -o "$(date +%F-%T).gif"
  fi

  echo "Gif OK!"
}

get_window_geometry
paplay ./race-countdown.ogg
do_shots $WINDOW_GEOMETRY
shots_to_gif

rm -rf $PNG_DIR

echo "Thanks for using this script."
