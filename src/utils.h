#ifndef UTILS_H
#define UTILS_H

#define K_UP 65
#define K_DOWN 66
#define K_RIGHT 67
#define K_LEFT 68

#define NUM_GHOSTS 4
#define PACMAN_ID 0
#define PLAYER_ID 1
#define GHOST_ID 2

#define P_RD 0
#define P_WR 1

#define PIPE_ERROR 16
#define FORK_ERROR 32

#define MAPXMIN 0
#define MAPYMIN 0
#define MAPXMAX 55
#define MAPYMAX 31

typedef enum {UP, DOWN, RIGHT, LEFT} Direction;

typedef struct
{
    int x,y;
} Position;

typedef struct
{
    int id;
    Position p;
    Direction dir;
} Entity;

static const char S_PAC[4][3] = {"(*v", "^*)", "(*<", ">*)"};
static const char S_GHST[3] = "[\"]";

static const char *MAP[] = {
"lqqqqqqqqqqqqqqqqqqqqqqqqqwqwqqqqqqqqqqqqqqqqqqqqqqqqqk",
"x                         x@x                         x",
"x   lqqqqqk   lqqqqqqqk   x@x   lqqqqqqqk   lqqqqqk   x",
"x   x@@@@@x   x@@@@@@@x   x@x   x@@@@@@@x   x@@@@@x   x",
"x   mqqqqqj   mqqqqqqqj   mqj   mqqqqqqqj   mqqqqqj   x",
"x                                                     x",
"x   lqqqqqk   lqk   lqqqqqqqqqqqqqk   lqk   lqqqqqk   x",
"x   mqqqqqj   x@x   mqqqqqk@lqqqqqj   x@x   mqqqqqj   x",
"x             x@x         x@x         x@x             x",
"mqqqqqqqqqk   x@mqqqqqk   x@x   lqqqqqj@x   lqqqqqqqqqj",
"@@@@@@@@@@x   x@lqqqqqj   mqj   mqqqqqk@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x                     x@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x   lqqqqq---qqqqqk   x@x   x@@@@@@@@@@",
"qqqqqqqqqqj   mqj   x@@@@@@@@@@@@@x   mqj   mqqqqqqqqqq",
"                    x@@@@@@@@@@@@@x                    ",
"qqqqqqqqqqk   lqk   x@@@@@@@@@@@@@x   lqk   lqqqqqqqqqq",
"@@@@@@@@@@x   x@x   mqqqqqqqqqqqqqj   x@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x                     x@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x   lqqqqqqqqqqqqqk   x@x   x@@@@@@@@@@",
"lqqqqqqqqqj   mqj   mqqqqqk@lqqqqqj   mqj   mqqqqqqqqqk",
"x                         x@x                         x",
"x   lqqqqqk   lqqqqqqqk   x@x   lqqqqqqqk   lqqqqqk   x",
"x   mqqqk@x   mqqqqqqqj   mqj   mqqqqqqqj   x@lqqqj   x",
"x       x@x                                 x@x       x",
"tqqqk   x@x   lqk   lqqqqqqqqqqqqqk   lqk   x@x   lqqqu",
"tqqqj   mqj   x@x   mqqqqqk@lqqqqqj   x@x   mqj   mqqqu",
"x             x@x         x@x         x@x             x",
"x   lqqqqqqqqqj@mqqqqqk   x@x   lqqqqqj@mqqqqqqqqqk   x",
"x   mqqqqqqqqqqqqqqqqqj   mqj   mqqqqqqqqqqqqqqqqqj   x",
"x                                                     x",
"mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj"};

static const char *PELLETS[] = {
"                                                       ",
"  ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~     ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~  ",
"  ~         ~           ~     ~           ~         ~  ",
"  `         ~           ~     ~           ~         `  ",
"  ~         ~           ~     ~           ~         ~  ",
"  ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~  ",
"  ~         ~     ~                 ~     ~         ~  ",
"  ~         ~     ~                 ~     ~         ~  ",
"  ~ ~ ~ ~ ~ ~     ~ ~ ~ ~     ~ ~ ~ ~     ~ ~ ~ ~ ~ ~  ",
"            ~                             ~            ",
"            ~                             ~            ",
"            ~                             ~            ",
"            ~                             ~            ",
"            ~                             ~            ",
"            ~                             ~            ",
"            ~                             ~            ",
"            ~                             ~            ",
"            ~                             ~            ",
"            ~                             ~            ",
"            ~                             ~            ",
"  ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~     ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~  ",
"  ~         ~           ~     ~           ~         ~  ",
"  ~         ~           ~     ~           ~         ~  ",
"  ` ~ ~     ~ ~ ~ ~ ~ ~ ~     ~ ~ ~ ~ ~ ~ ~     ~ ~ `  ",
"      ~     ~     ~                 ~     ~     ~      ",
"      ~     ~     ~                 ~     ~     ~      ",
"  ~ ~ ~ ~ ~ ~     ~ ~ ~ ~     ~ ~ ~ ~     ~ ~ ~ ~ ~ ~  ",
"  ~                     ~     ~                     ~  ",
"  ~                     ~     ~                     ~  ",
"  ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~  ",
"                                                       "};

void print_map_at(int, int);
void print_map();
void print_pellets();
void print_pacman(Entity);
char get_map_at(int, int);
Position get_pac_eff_pos(Position);
int mod(int, int);

int random_between(int, int);
int distance(Position, Position);
int minIndex(int, int *);
Position offset_position(Position, Direction, int);
Direction reverseDirection(Direction);

#endif