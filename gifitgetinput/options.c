#include "options.h"

GifitOpts create_default_gifit_options(char *tmp_dir, char *gif_dir)
{
    GifitOpts options;

    options.mode = 0;
    options.screenshot_rate = 10;
    options.gif_speed = 1.0f;
    options.gif_scale = 1.0f;
    options.zoom_factor = 2.0f;
    options.zoom_speed = 3.0f;
    options.show_keys_enable = false;

    char tmp_dir_template[1024];

    if(tmp_dir != NULL)
    {
        strcpy(tmp_dir_template, tmp_dir);
        strcat(tmp_dir_template, "/gifit.XXXXXX");
    }
    else
    {
        strcpy(tmp_dir_template, "/tmp/gifit.XXXXXX");
    }

    strcpy(options.tmp_dir, mkdtemp (tmp_dir_template));

    if(gif_dir != NULL)
    {
        strcpy(options.gif_dir, gif_dir);
    }
    else
    {
        char cwd[1024];

        if(getcwd(cwd, sizeof(cwd)) != NULL)
            strcpy(options.gif_dir, cwd);
        else
            strcpy(options.gif_dir, ".");
    }

    return options;
}

void assert_int_option_between(int value, int min, int max, int label)
{
    if(value < min || value > max)
    {
        fprintf (stderr, "Option -%c needs to be between %d and %d.\n", label, min, max);
        exit(1);
    }
}

void assert_float_option_between(float value, float min, float max, int label)
{
    if(value < min || value > max)
    {
        fprintf (stderr, "Option -%c needs to be between %.1f and %.1f.\n", label, min, max);
        exit(1);
    }
}

GifitOpts parse_args (int argc, char **argv)
{
    char *tmp_dir = getenv("GIFIT_TMP_DIR");
    char *gif_dir = getenv("GIFIT_GIF_DIR");


    GifitOpts options = create_default_gifit_options(tmp_dir, gif_dir);


    int c;
    float vf;
    int vi;

    opterr = 0;
    while ((c = getopt (argc, argv, "n:v:t:fwsk")) != -1)
    {
        switch (c)
        {
            case 'n':
                vi = atoi(optarg);
                assert_int_option_between(vi, 1, 30, c);
                options.screenshot_rate = vi;
                break;
            case 'v':
                vf = atof(optarg);
                assert_float_option_between(vf, 0.1f, 4.0f, c);
                options.gif_speed = vf;
                break;
            case 't':
                vf = atof(optarg);
                assert_float_option_between(vf, 0.1f, 4.0f, c);
                options.gif_scale = vf;
                break;
            case 'f':
                options.mode = 0;
                break;
            case 'w':
                options.mode = 1;
                break;
            case 's':
                options.mode = 2;
                break;
            case 'k':
                options.show_keys_enable = true;
                break;
            case '?':
                if (optopt == 'n' | optopt == 'v' |optopt == 't'  )
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                exit(1);
            default:
                abort ();
        }
    }

    int index;
    for (index = optind; index < argc; index++)
    {
        printf ("Non-option argument %s\n", argv[index]);
        exit(1);
    }

    return options;
}
