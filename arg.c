
#include "arg.h"


const char *argp_program_version =
  "DungeonGenerator 0.5";

const char *argp_program_bug_address =
  "<anatas.torsten@gmx.de>";

struct Arguments arg = {
    0, // flag debug
    0x0815, // seed
    5, // iterations
    25, // grid size
    40, // mapWidth
    30, // mapHeight
    1, // corridor width
    0, // numCorridors
    1, // roomOffset
    0, // roomSize
    5, // minRoomSize
    NULL, // outfile
    1, // numLevels
    0, // dungeonDesign
    0.0, // dungeonDecay
    0, // rectOffset
    {0} // levelData
};


error_t
parse_opt (int key, char *args, struct argp_state *state)
{
    switch (key)
    {   
        case 'D':
            arg.flag_debug = 1;
            bsp_set_debug ();
            break;

        case 's':
            arg.seed = atoi (args);
            break;

        case 'i':
            arg.iterations = atoi (args);
            break;

        case 'o': // output file
            arg.outfile = args;
            break;

        case 'l': // numLevels
            arg.numLevels = atoi (args);
            break;

        case 500: // map size
            sscanf (args, "%ix%i", &arg.mapWidth, &arg.mapHeight);
            break;

        case 501: // grid_size
            arg.gridSize = atoi (args);
            break;

        case 502: // corridor width
            arg.corridorWidth = atoi (args);
            break;

        case 503: // number of corridors on top level
            arg.numCorridors = atoi (args);
            break;

        case 504: // number of rolls for room size; < 0 = Disadv.
            arg.roomSize = atoi (args);
            break;

        case 505:
            arg.roomOffset = atoi (args);
            if (arg.roomOffset < 0)
            {
                fprintf (stderr, "roomOffset invalid < 0, using default value\n");
                arg.roomOffset = 1;
            }
            break;

        case 506:
            arg.dungeonDesign = atoi (args);
            break;

        case 507:
            sscanf (args, "%f", &arg.dungeonDecay);
            break;

        case 508:
            arg.rectOffset = atoi (args);
            break;

        case 509:
            arg.minRoomSize = atoi (args);
            break;

        case 'L':
            cstr_remove (args, arg.levelData, " ");
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}


