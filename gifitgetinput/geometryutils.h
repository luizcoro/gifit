#ifndef GEOMETRYUTILS
#define GEOMETRYUTILS

#include "stdio.h"
#include "stdlib.h"
#include "systemutils.h"

typedef struct
{
    int x;
    int y;
    int width;
    int height;

} Geometry;

typedef struct
{
    int x;
    int y;

} Click;

Geometry get_geometry_by_mode(int mode);

char *geometry_to_string(Geometry geometry);

Geometry string_to_geometry(char *str);

Geometry zoom_by_level(Geometry *geometry, Click *click, float zoom_level);

Geometry zoom_towards(Geometry *g_initial, Geometry *g_final, float step);

#endif
