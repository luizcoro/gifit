#ifndef XUTILS_H
#define XUTILS_H

#include "X11/Xlib.h"
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>

#include "commandqueue.h"

typedef struct
{
    Display *display;
    Visual *visual;
    Screen *screen;
    Colormap colormap;
    Window window;
    Window root_window;

} X11Data;

X11Data create_default_x11data();

void print_xwindow(X11Data *x11data);

void grab_exit_key(X11Data *x11data);

void grab_zoom_button(X11Data *x11data);

int handle_focus(X11Data *x11data);

int handle_event(X11Data *x11data, CommandQueue *q);

#endif
