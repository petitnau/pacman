#ifndef INTERFACE_H
#define INTERFACE_H

#include <curses.h>

#include "ghost.h"
#include "pacman.h"
#include "entity.h"
#include "control.h"

static const char S_PACMAN[4][4] = {"(*v", ">*)", "^*)", "(*<"};
static const char S_GHOST[7][4] = {"[\"]", "[\"]", "[w]", "* *", "   ", "[\"]", "[\"]"};

static const char S_FRUIT[1][4] = {".^."};

static const char F_NUMBERS[][7] = {"lkxxmj", " k x v", "lkljmj", "lk umj", "k mu v", "lkmkmj", "lktkmj", "lk x  ", "lktumj", "lkmumj"};
static const char F_LETTERS[][7] = {"lktu  ", "", "lkx mj", "", "lkt mj", "", "lkxkmj", //ABCDEFG
                                    "kltujm", " w x v", "", "", "", "", "", //HIJKLMN
                                    "lkxxmj", "lktjv ", "", "lkt.vm", "lkmkmj", "", //OPQRSTU
                                    "klxxmj", "", "", "", ""}; //VWXYZ
                                    
static const char MAP_PACMAN[MAP_HEIGHT][MAP_WIDTH+1] = {
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
"@@@@@@@@@@x   x@x      #########      x@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x   lqqqqq^^^qqqqqk   x@x   x@@@@@@@@@@",
"qqqqqqqqqqj   mqj   x@@@@@   @@@@@x   mqj   mqqqqqqqqqq",
"                    x@   ]   [   @x                    ",
"qqqqqqqqqqk   lqk   x@@@@@@@@@@@@@x   lqk   lqqqqqqqqqq",
"@@@@@@@@@@x   x@x   mqqqqqqqqqqqqqj   x@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x                     x@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x   lqqqqqqqqqqqqqk   x@x   x@@@@@@@@@@",
"lqqqqqqqqqj   mqj   mqqqqqk@lqqqqqj   mqj   mqqqqqqqqqk",
"x                         x@x                         x",
"x   lqqqqqk   lqqqqqqqk   x@x   lqqqqqqqk   lqqqqqk   x",
"x   mqqqk@x   mqqqqqqqj   mqj   mqqqqqqqj   x@lqqqj   x",
"x       x@x            #########            x@x       x",
"tqqqk   x@x   lqk   lqqqqqqqqqqqqqk   lqk   x@x   lqqqu",
"tqqqj   mqj   x@x   mqqqqqk@lqqqqqj   x@x   mqj   mqqqu",
"x             x@x         x@x         x@x             x",
"x   lqqqqqqqqqj@mqqqqqk   x@x   lqqqqqj@mqqqqqqqqqk   x",
"x   mqqqqqqqqqqqqqqqqqj   mqj   mqqqqqqqqqqqqqqqqqj   x",
"x                                                     x",
"mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj"};

static const char MAP_GUNMAN[MAP_HEIGHT][MAP_WIDTH+1] = {
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
"@@@@@@@@@@x   x@x      #########      x@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x   lqqqqq^^^qqqqqk   x@x   x@@@@@@@@@@",
"qqqqqqqqqqj   mqj   x@@@@@   @@@@@x   mqj   mqqqqqqqqqq",
"                    <             >                    ",
"qqqqqqqqqqk   lqk   x@@@@@   @@@@@x   lqk   lqqqqqqqqqq",
"@@@@@@@@@@x   x@x   mqqqqqvvvqqqqqj   x@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x                     x@x   x@@@@@@@@@@",
"@@@@@@@@@@x   x@x   lqqqqqqqqqqqqqk   x@x   x@@@@@@@@@@",
"lqqqqqqqqqj   mqj   mqqqqqk@lqqqqqj   mqj   mqqqqqqqqqk",
"x                         x@x                         x",
"x   lqqqqqk   lqqqqqqqk   x@x   lqqqqqqqk   lqqqqqk   x",
"x   mqqqk@x   mqqqqqqqj   mqj   mqqqqqqqj   x@lqqqj   x",
"x       x@x            #########            x@x       x",
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

void print_ui(WINDOW*, ControlData*);

#endif