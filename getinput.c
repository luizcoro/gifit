#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

typedef struct
{
    Display *display;
    Window window;
} X11Data;

void grab_exit_key(X11Data* x11data)
{
    unsigned int    modifiers       = ControlMask;
    int             keycode         = XKeysymToKeycode(x11data->display,XK_q);
    /* Window          grab_window     = DefaultRootWindow(x11data.display); */
    Window          grab_window     =  x11data->window;
    Bool            owner_events    = False;
    int             pointer_mode    = GrabModeAsync;
    int             keyboard_mode   = GrabModeAsync;

    XGrabKey(x11data->display, keycode, modifiers, grab_window, owner_events, pointer_mode, keyboard_mode);
}

void grab_zoom_button(X11Data* x11data)
{
    unsigned int    button          = Button1;
    unsigned int    modifiers       = ControlMask;
    Window          grab_window     = DefaultRootWindow(x11data->display);
    /* Window          grab_window     =  x11data.window; */
    Bool            owner_events    = False;
    int             event_mask      = ButtonPressMask;
    int             pointer_mode    = GrabModeAsync;
    int             keyboard_mode   = GrabModeAsync;
    Window          confine_to      = None;
    Cursor          cursor          = None;

    XGrabButton(x11data->display, button, modifiers, grab_window, owner_events, event_mask, pointer_mode, keyboard_mode, confine_to, cursor);
}

int handle_focus(X11Data *x11data)
{
    int revert_to_return = 0;
    XGetInputFocus(x11data->display, &x11data->window, &revert_to_return);

    if (x11data->window != None)
    {
        grab_exit_key(x11data);
        grab_zoom_button(x11data);

        unsigned int masks = FocusChangeMask;
        masks |= KeyPressMask;
        masks |= ButtonPressMask;

        XSelectInput(x11data->display, x11data->window, masks);
    }
}

int handle_event(X11Data* x11data)
{
    XEvent ev;
    XNextEvent(x11data->display, &ev);

    switch (ev.type)
    {
        case FocusOut:
        {
            x11data->window = None;
            break;
        }
        case KeyPress:
        {
            printf("key pressed\n");
            break;
        }
        case ButtonPress:
        {
            printf("button clicked\n");
            break;
        }
        default: break;
    }
}

int main(int argc, char *argv[])
{

    X11Data x11data;
    x11data.display = XOpenDisplay(NULL);
    x11data.window = None;

    for(;;)
    {
        if (x11data.window == None)
        {
            handle_focus(&x11data);
            char* name = '\0';
            int status = XFetchName(x11data.display, x11data.window, &name);
            if (status >= Success)
            {
                printf("Found: 0x%x  %s\n", x11data.window, name);
            }
            XFree(name);
        }
        else
            handle_event(&x11data);
    }

    XCloseDisplay(x11data.display);
    return 0;
}

