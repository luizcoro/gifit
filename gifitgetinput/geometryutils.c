#include "geometryutils.h"

Geometry get_geometry_by_mode(int mode)
{

    char *output;
    char *pch;

    switch(mode)
    {
        case 0:
            output = run_external_program("xwininfo -root");

            char *geometry_test = "  -geometry";
            int geometry_test_size = strlen(geometry_test);

            pch = strtok (output,"\n");
            while (pch != NULL)
            {
                if(strncmp(geometry_test, pch, geometry_test_size) == 0)
                {
                    char *pch2;

                    pch2 = strtok(pch, " ");
                    pch2 = strtok(NULL, " ");

                    return  string_to_geometry(pch2);
                }

                pch = strtok (NULL, "\n");
            }
        case 1:
            output = run_external_program("xwininfo");
            char *x_test = "  Absolute upper-left X";
            int x_test_size = strlen(x_test);

            char *y_test = "  Absolute upper-left Y";
            int y_test_size = strlen(y_test);

            char *width_test = "  Width";
            int width_test_size = strlen(width_test);

            char *height_test = "  Height";
            int height_test_size = strlen(height_test);

            Geometry g;

            char *end_output;
            pch = strtok_r(output, "\n", &end_output);
            while (pch != NULL)
            {
                if(strncmp(x_test, pch, x_test_size) == 0)
                {
                    char *pch2;
                    char *end_pch;

                    pch2 = strtok_r(pch, " \t", &end_pch);
                    pch2 = strtok_r(NULL, " \t", &end_pch);
                    pch2 = strtok_r(NULL, " \t", &end_pch);
                    pch2 = strtok_r(NULL, " \t", &end_pch);

                    g.x = atoi(pch2);
                }
                else if(strncmp(y_test, pch, y_test_size) == 0)
                {
                    char *pch2;
                    char *end_pch;

                    pch2 = strtok_r(pch, " \t", &end_pch);
                    pch2 = strtok_r(NULL, " \t", &end_pch);
                    pch2 = strtok_r(NULL, " \t", &end_pch);
                    pch2 = strtok_r(NULL, " \t", &end_pch);

                    g.y = atoi(pch2);
                }
                else if(strncmp(width_test, pch, width_test_size) == 0)
                {
                    char *pch2;
                    char *end_pch;

                    pch2 = strtok_r(pch, " \t", &end_pch);
                    pch2 = strtok_r(NULL, " \t", &end_pch);

                    g.width = atoi(pch2);
                }
                else if(strncmp(height_test, pch, height_test_size) == 0)
                {
                    char *pch2;
                    char *end_pch;

                    pch2 = strtok_r(pch, " \t", &end_pch);
                    pch2 = strtok_r(NULL, " \t", &end_pch);

                    g.height = atoi(pch2);

                    return g;
                }

                pch = strtok_r (NULL, "\n", &end_output);
            }

            break;
        case 2:
            return string_to_geometry(run_external_program("xrectsel"));
    }
}

char *geometry_to_string(Geometry geometry)
{
    char *str = (char *) malloc(sizeof(char) * 20);
    sprintf(str, "%dx%d+%d+%d\0", geometry.width, geometry.height, geometry.x, geometry.y);

    return str;
}

Geometry string_to_geometry(char *str)
{
    Geometry g;
    sscanf(str, "%dx%d+%d+%d", &g.width, &g.height, &g.x, &g.y);

    return g;
}

Geometry zoom_by_level(Geometry *geometry, Click *click, float zoom_level)
{
    Geometry g;

    g.width = geometry->width/zoom_level;
    g.height = geometry->height/zoom_level;

    g.x = click->x - (g.width/2);
    g.y = click->x - (g.width/2);

    if(g.x < geometry->x)
        g.x = geometry->x;
    else if(g.x > geometry->width)

    if(g.y < geometry->y)
        g.y = geometry->y;

    return g;
}

Geometry zoom_towards(Geometry *g_initial, Geometry *g_final, float step)
{
    Geometry g;

    g.width = ((g_final->width - g_initial->width) * step)  + g_initial->width;
    g.height = ((g_final->height - g_initial->height) * step)  + g_initial->height;
    g.x = ((g_final->x - g_initial->x) * step)  + g_initial->x;
    g.y = ((g_final->y - g_initial->y) * step)  + g_initial->y;

    return g;
}

