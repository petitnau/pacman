#ifndef INTERFACE_H
#define INTERFACE_H

#include "ghost.h"
#include "pacman.h"

#define MAP_WIDTH 55
#define MAP_HEIGHT 31

#define FRUIT_POS_Y 17
#define FRUIT_POS_X 27

static const char S_PAC[4][4] = {"(*v", ">*)", "^*)", "(*<"};
static const char S_GHST[4][4] = {"[\"]", "[\"]", "[w]", "* *"};

static const char FRUIT[1][4] = {".^."};

static const char F_NUMBERS[][7] = {"lkxxmj", " k x v", "lkljmj", "lk umj", "k mu v", "lkmkmj", "lktkmj", "lk x  ", "lktumj", "lkmumj"};
static const char F_LETTERS[][7] = {"lktu  ", "", "lkx mj", "", "lkt mj", "", "lkxkmj", //ABCDEFG
                                    "kltujm", " w x v", "", "", "", "", "", //HIJKLMN
                                    "lkxxmj", "lktjv ", "", "lkt.vm", "lkmkmj", "", //OPQRSTU
                                    "klxxmj", "", "", "", ""}; //VWXYZ
                                    
static const char MAP[MAP_HEIGHT][MAP_WIDTH+1] = {
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
"@@@@@@@@@@x   x@x   lqqqqq   qqqqqk   x@x   x@@@@@@@@@@",
"qqqqqqqqqqj   mqj   x@@@@@   @@@@@x   mqj   mqqqqqqqqqq",
"                    x@@@@@   @@@@@x                    ",
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

static const char PELLETS[MAP_HEIGHT][MAP_WIDTH+1] = {
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

void print_pacman(CharPacman);
void print_ghost(CharGhost);
void unprint_area(int, int, int, char[MAP_HEIGHT][MAP_WIDTH]);
void sunprint_area(int, int, int, char[MAP_HEIGHT][MAP_WIDTH], CharPacman, CharGhost);
void print_ui(int, CharPacman, CharGhost);
void print_fruit();
void print_temp_text(int, int, char*);

#endif