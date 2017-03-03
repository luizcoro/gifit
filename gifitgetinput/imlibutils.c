#include "imlibutils.h"

Display *disp = NULL;
Visual *vis = NULL;
Screen *scr = NULL;
Colormap cm;
int depth;
Window root = 0;

// I dont know why do i need to make another connection to xlib in order to take screenshots, at least this works

/* void init_imlib(X11Data *x11data) */
void init_imlib()
{

   disp = XOpenDisplay(NULL);
   scr = ScreenOfDisplay(disp, DefaultScreen(disp));

   vis = DefaultVisual(disp, XScreenNumberOfScreen(scr));
   depth = DefaultDepth(disp, XScreenNumberOfScreen(scr));
   cm = DefaultColormap(disp, XScreenNumberOfScreen(scr));
   root = RootWindow(disp, XScreenNumberOfScreen(scr));

   imlib_context_set_display(disp);
   imlib_context_set_visual(vis);
   imlib_context_set_colormap(cm);
   imlib_context_set_color_modifier(NULL);
   imlib_context_set_operation(IMLIB_OP_COPY);


    /* imlib_context_set_display(x11data->display); */
    /* imlib_context_set_visual(x11data->visual); */
    /* imlib_context_set_colormap(x11data->colormap); */
    /* imlib_context_set_color_modifier(NULL); */
    /* imlib_context_set_operation(IMLIB_OP_COPY); */
}

/* void save_screenshot(X11Data *x11data, char *filename, Geometry *geometry) */
void save_screenshot(char *filename, Geometry *geometry)
{
    Imlib_Image screen_image = gib_imlib_create_image_from_drawable(root, 0, geometry->x, geometry->y, geometry->width, geometry->height, 1);
    imlib_context_set_image(screen_image);

    XFixesCursorImage *xcursor = XFixesGetCursorImage(disp);

    const size_t size_px = xcursor->width * xcursor->height;
    DATA32 pixels [ size_px ];

    for ( unsigned int i=0;i<size_px;i++ ) {
        pixels[ i ] = (uint32_t)xcursor->pixels[ i ];
    }

    Imlib_Image cursor_image = imlib_create_image_using_data( xcursor->width, xcursor->height, pixels );

    Imlib_Image buffer = imlib_context_get_image();
    imlib_context_set_image( cursor_image );
    imlib_image_set_has_alpha( 1 );
    imlib_context_set_image( buffer );

    imlib_blend_image_onto_image( cursor_image, 0, 0, 0, xcursor->width, xcursor->height, xcursor->x - geometry->x - xcursor->xhot , xcursor->y - geometry->y - xcursor->yhot,  xcursor->width, xcursor->height );

    imlib_image_attach_data_value("quality", NULL, 75, NULL);

    Imlib_Image blended_image = imlib_context_get_image();
    gib_imlib_save_image(blended_image, filename);
    gib_imlib_free_image_and_decache(blended_image);
}
