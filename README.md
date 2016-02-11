#gifit

Simple shell script to make gif of your screen.


###Example:

![alt tag](http://s24.postimg.org/h6dln48j9/2015_12_23_03_21_53_gif.gif)


###Dependencies:

* xwininfo to get window information
* xrectsel to select a retangular area
* scrot to take screenshots
* graphicsmagick to image processing
* gifsicle to gif optimization


###Arguments:

-w = choose a specific window

-s = select a specific area

-n number = screenshot rate (default 10/s)

-v number = gif speed factor (default 1)

-t number = screenshot scale factor (default 1)


###Environment variables:

GIFIT_TMP_DIR = temporary folder to convert screenshots into gif

GIFIT_GIF_DIR = folder to save generated gif (default current directory)
