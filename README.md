#gifit

Simple shell script to make gif of your screen.


###Example:

![alt tag](http://s24.postimg.org/h6dln48j9/2015_12_23_03_21_53_gif.gif)


###Dependencies:

* xwininfo to get window information
* scrot to take screenshots
* graphicsmagick to image processing
* gifsicle to gif optimization

###Optional Dependencies:

* xrectsel to select a retangular area
* yad to remove screenshots with a file selection dialog
* screenkey 0.8 or later to show pressed keys

###Arguments:

-n number = screenshot rate - 1-30 (default 10/s)

-v number = gif speed factor - 0.1-4 (default 1)

-t number = screenshot scale factor - 0.1-4 (default 1)

-w = choose a specific window

-s = select a specific area (xrectsel required)

-k = show key pressed (screenkey required)


###Environment variables:

GIFIT_TMP_DIR = temporary folder to convert screenshots into gif

GIFIT_GIF_DIR = folder to save generated gif (default current directory)

###Troubleshooting

Q: When using -k argument the screenkey dialog doesnt have transparency. How could I fix this?

A: Consider installing a composite manager like compton or xcompmgr if your window manager doesnt have it.

