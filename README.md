# gifit

Simple shell script to make gif of your screen.

Example:
![alt tag](http://s24.postimg.org/h6dln48j9/2015_12_23_03_21_53_gif.gif)

Arguments:

-w = choose a specific window

-s = select a specific area

-n number = screenshot rate (default 15/s)

-v number = gif speed factor (default 1)

-t number = screenshot scale factor (default 1)


Environment variables:

GIFIT_TMP_DIR = temporary folder to convert screenshots into gif

GIFIT_GIF_DIR = folder to save generated gif (default current directory)


PS: Currently there is a huge bug, maim program takes 200 milliseconds to take a fullscreen shot. So, the -n argument is not consistent with the specification. In fact you can only make gifs with less than 5 frames per second.

Need to find another way to take shots and implement a resampling function.
