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

void allocate_memory(polygon *pol, int size);

polygon create_polygon_from_file(char *filename, int c1, int c2, int c3, int x_resolution, int y_resolution);

void draw_polygon(polygon pol, framebuffer f);

void set_color_polygon(polygon *pol, int c1, int c2, int c3);

polygon dilate(polygon pol, polygon frame, int scale, int x_center, int y_center);

polygon translate(polygon pol, polygon frame, int x_distance, int y_distance);

polygon rotate(polygon pol, int x_center, int y_center);

void release_memory(polygon *pol);

#endif
