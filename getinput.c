#include <stdio.h>
#include <stdlib.h>

#include "xutils.h"

int main(int argc, char *argv[])
{

    X11Data x11data = create_default_x11data();

    for(;;)
    {
        if (x11data.window == None)
        {
            handle_focus(&x11data);
            print_xwindow(&x11data);
        }
        else
            handle_event(&x11data, argv[1]);
    }

    return 0;
}
