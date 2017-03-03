#ifndef IMLIBUTILS_H_
#define IMLIBUTILS_H_

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <giblib/giblib.h>
#include <stdint.h>

#include "xutils.h"
#include "geometryutils.h"

/* void init_imlib(X11Data *x11data); */
void init_imlib();

/* void save_screenshot(X11Data *x11data, char *filename, Geometry *geometry); */
void save_screenshot(char *filename, Geometry *geometry);

#endif
