#ifndef OPTIONS_H
#define OPTIONS_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct
{
    int mode, screenshot_rate;
    float gif_speed, gif_scale, zoom_factor, zoom_speed;
    bool show_keys_enable;
    char tmp_dir[1024], gif_dir[1024];

} GifitOpts;

GifitOpts parse_args(int argc, char *argv[]);

#endif
