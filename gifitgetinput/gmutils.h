#ifndef GMUTILS_H
#define GMUTILS_H

#include <string.h>

#include <magick/api.h>

void init_gm();

Image *load_images(char *filenames[], int n_filenames);

Image *resize_images(Image *images, int n_images, int width, int height);

void create_gif(Image *images, int n_images, unsigned int delay, char *filename);

void free_gm();
#endif
