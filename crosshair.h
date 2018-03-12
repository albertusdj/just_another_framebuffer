#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "framebuffer.h"
#include "point.h"


extern const int dimension_x;
extern const int dimension_y;

extern const int crosshair_red;
extern const int crosshair_green;
extern const int crosshair_blue;
	
extern const int crosshair_middle;
extern const int crosshair_radius;

int getRed(int x, int y, framebuffer f);

int getGreen(int x, int y, framebuffer f);

int getBlue(int x, int y, framebuffer f);

void drawCrosshair(point p, int r, framebuffer f);

void readTempCrosshair(point parr[], point p, framebuffer f);

void drawTempCrosshair(point parr[], framebuffer f);

int isCrosshairMoveValid(point c, int dx, int dy);

#endif