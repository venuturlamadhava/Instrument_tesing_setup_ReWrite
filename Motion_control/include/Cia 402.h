#pragma once  //used to avoid duplicate header files during comilation
#include<cmath> //used for math functions

constexpr int TICK_PER_REV=4096;
constexpr int TICK_OFFSET=2048;
constexpr double DEGREES_PER_TICK=360/4096;

//constexpr - Compute this value during compilation instead of while the program is running.

CIA402_State decode_state(std::unit16_t statusword);

