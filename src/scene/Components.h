#pragma once
#include <SDL.h>

struct Position {
    float x, y;
};

struct Velocity {
    float vx, vy;
};

struct Size {
    float w, h;
};

struct Color {
    Uint8 r, g, b, a;
};

struct Paddle {};
struct Ball {};
struct Block {};
