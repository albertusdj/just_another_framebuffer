#ifndef POLYGON_H
#define POLYGON_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include "point.h"
#include "framebuffer.h"

typedef struct polygon{
	int **arr;
	int size;
	int c1;
	int c2;
	int c3;
	int x_resolution;
	int y_resolution;
	int x_center;
	int y_center;
}polygon;

polygon create_polygon_from_file(char *filename, int c1, int c2, int c3, int x_resolution, int y_resolution);

void draw_polygon(polygon pol, framebuffer f);

void clear_polygon(polygon *pol);

polygon dilate(polygon pol, int scale, int x_center, int y_center);

#endif