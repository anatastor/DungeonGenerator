
#include "arg.h"


const char *argp_program_version =
  "DungeonGenerator 0.5";

const char *argp_program_bug_address =
  "<anatas.torsten@gmx.de>";

struct Arguments arg = {
    0, // flag debug
    0, // flag color
    0x0815, // seed
    5, // iterations
    25, // grid size
    40, // map width
    30, // map height
    "out.png" // output file
};

error_t
parse_opt (int key, char *args, struct argp_state *state)
{
    switch (key)
    {   
        case 'D':
            arg.flag_debug = 1;
            break;

        case 'C':
            arg.flag_color = 1;
            break;

        case 's':
            arg.seed = atoi (args);
            break;

        case 'i':
            arg.iterations = atoi (args);
            break;

        case 'o': // output file
            arg.output_file = args;
            break;

        case 500: // map size
            sscanf (args, "%ix%i", &arg.map_width, &arg.map_height);
            break;

        case 501: // grid_size
            arg.grid_size = atoi (args);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}


