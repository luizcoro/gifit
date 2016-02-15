#ifndef XUTILS_H_
  #include "X11/Xlib.h"
#endif

typedef struct
{
    Display *display;
    Window window;
} X11Data;

X11Data create_default_x11data();

void print_xwindow(X11Data *x11data);

void grab_exit_key(X11Data *x11data);

void grab_zoom_button(X11Data *x11data);

int handle_focus(X11Data *x11data);

int handle_event(X11Data *x11data, char *filename);
