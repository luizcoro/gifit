#include "xutils.h"
#include "systemutils.h"

X11Data create_default_x11data()
{
    /* XInitThreads(); */

    X11Data x11data;
    x11data.display = XOpenDisplay(NULL);
    x11data.screen = ScreenOfDisplay(x11data.display, DefaultScreen(x11data.display));
    x11data.visual = DefaultVisual(x11data.display, XScreenNumberOfScreen(x11data.screen));
    x11data.colormap = DefaultColormap(x11data.display, XScreenNumberOfScreen(x11data.screen));

    x11data.root_window = XDefaultRootWindow(x11data.display);
    x11data.window = None;

    return x11data;
}

void print_xwindow(X11Data *x11data)
{
    char* name;
    int status = XFetchName(x11data->display, x11data->window, &name);
    if (status >= Success)
    {
        printf("0x%lx  %s\n", x11data->window, name);
    }
    XFree(name);
}

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

int handle_event(X11Data* x11data, CommandQueue *queue)
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
            Command command;
            strcpy(command.message, "q");

            enqueue(queue, command);
            break;
        }
        case ButtonPress:
        {
            char str[20];
            sprintf(str, "%d %d", ev.xbutton.x, ev.xbutton.y);

            Command command;
            strcpy(command.message, str);

            enqueue(queue, command);
            break;
        }

        default: break;
    }
}
