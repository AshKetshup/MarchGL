#ifndef MAIN_H
#define MAIN_H

#include "marchgl.h"

#include <iostream>
#include <thread>
#include <string>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const unsigned int THREAD_AMNT = thread::hardware_concurrency() / 2;
const char* RMODE = "CPU";

const char* HELP =
"MarchGL - Marching Cubes IsoSurfaces render"
""
"-W, --width   [int]\t\t\tSets width pixels. Default is 1280px"
"-H, --height  [int]\t\t\tSets height pixels. Default is 720px"
"-T, --threads [int]\t\t\tOverrides calculated ammount of threads"
""
"-h, --help         \t\t\tDisplays help and exits"
"";

int argHandler(int argc, const char* argv[], Arguments& args);
int main(int argc, const char* argv[]);

#endif