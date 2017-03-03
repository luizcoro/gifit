#include "gmutils.h"

void init_gm()
{
    InitializeMagick(NULL);
}

Image *load_images(char *filenames[], int n_filenames)
{
    ExceptionInfo exception;
    Image *image, *images;
    ImageInfo *image_info;


    GetExceptionInfo(&exception);

    image_info=CloneImageInfo((ImageInfo *) NULL);
    images=NewImageList();

    int i;

    for(i = 0; i < n_filenames; ++i)
    {
        strcpy(image_info->filename, filenames[i]);
        printf("Reading %s ...", image_info->filename);
        image = ReadImage(image_info, &exception);
        printf(" %lu frames\n", GetImageListLength(image));

        if (exception.severity != UndefinedException)
            CatchException(&exception);

        if (image)
            AppendImageToList(&images,image);
    }

    DestroyImageInfo(image_info);

    return images;
}

Image *resize_images(Image *images, int n_images, int width, int height)
{
    ExceptionInfo exception;

    Image *image, *resized_image, *resized_images;
    ImageInfo *image_info = CloneImageInfo((ImageInfo *) NULL);

    GetExceptionInfo(&exception);

    int i;

    resized_images = NewImageList();

    while((image = RemoveFirstImageFromList(&images)) != (Image *) NULL)
    {
        /* image = GetImageFromList(images, i); */
        resized_image = ResizeImage(image, width, height, LanczosFilter, 1.0, &exception);

        strcpy(resized_image->filename, image->filename);
        WriteImage (image_info, resized_image);


        if(resized_image)
            AppendImageToList(&resized_images, resized_image);


        DestroyImage(image);
    }


    DestroyImageInfo(image_info);
    DestroyImageList(images);

    return resized_images;
}

void create_gif(Image *images, int n_images, unsigned int delay, char *filename)
{

    QuantizeInfo *quantize_info = CloneQuantizeInfo(NULL);
    quantize_info->dither = MagickFalse;
    quantize_info->colorspace = YUVColorspace;
    quantize_info->number_colors = 63;

    QuantizeImages(quantize_info, images);

    Image *image;

    int i;

    for(i = 0; i < n_images; ++i)
    {
        image = GetImageFromList(images, i);
        image->delay = delay;
        image->fuzz = 10;
    }

    strcpy(images->filename, filename);

    ImageInfo *image_info = CloneImageInfo((ImageInfo *) NULL);

    image_info->adjoin = MagickTrue;
    image_info->dither = MagickFalse;;

    WriteImage(image_info, images);
}

void free_gm()
{
    DestroyMagick();
}
