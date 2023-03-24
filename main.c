
#include <stdio.h>
#include <stdlib.h>

#include "arg.h"
#include "rng.h"
#include "cairo_renderer.h"
#include "bsp.h"
#include "dungeon.h"


int
main (int argc, char **argv)
{   
    /**+ INIT ***/
    argp_parse (&argp, argc, argv, 0, 0, NULL);
    rng_seed (arg.seed);
    Renderer *renderer = render_create (arg.mapWidth * arg.gridSize, arg.mapHeight * arg.gridSize);
    render_fill (renderer, 0x0c090a);

    
    Dungeon dungeon = Dungeon_create (arg.numLevels, arg.mapWidth, arg.mapHeight);

    FILE *fp = stdout;
    if (arg.outfile)
        fp = fopen (arg.outfile, "w");

    for (int i = 0; i < dungeon.numLevels; i++)
    {
        char buffer[1048];
        render_fill (renderer, 0x0c090a);
        render_map (renderer, &dungeon.map[i * dungeon.width * dungeon.height], 
                dungeon.width, dungeon.height, arg.gridSize);
        render_grid (renderer, arg.gridSize, 0x757575);

        if (arg.flag_debug)
            render_grid (renderer, arg.gridSize * 10, 0xFF0000);

        sprintf (buffer, "level_%02i.png", i);
        render_save (renderer, buffer);
    
        if (arg.flag_debug || arg.outfile)
        {
            map_fprint (fp, &dungeon.map[i * dungeon.width * dungeon.height], dungeon.width, dungeon.height);
            fprintf (fp, "\n");
        }
    }
    
    if (arg.outfile)
        fclose (fp);


    Dungeon_destroy (dungeon);
    render_destroy (renderer);

    return EXIT_SUCCESS;
}
