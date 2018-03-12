#include "crosshair.h"
#include "framebuffer.h"
#include "point.h"
#include "line.h"

const int dimension_x = 1366;
const int dimension_y = 768;

const int crosshair_red = 255;
const int crosshair_green = 255;
const int crosshair_blue = 255;
    
const int crosshair_middle = 5;
const int crosshair_radius = 20;

int getRed(int x, int y, framebuffer f) {
    int loc = (x+f.vinfo.xoffset) * (f.vinfo.bits_per_pixel/8) + 
            (y+f.vinfo.yoffset) * f.finfo.line_length;
    int c3 = *(f.fbp + loc + 2);
    if (c3 < 0) {
        c3 += 256;
    }
    return c3;
}

int getGreen(int x, int y, framebuffer f) {
    int loc = (x+f.vinfo.xoffset) * (f.vinfo.bits_per_pixel/8) + 
            (y+f.vinfo.yoffset) * f.finfo.line_length;
    int c2 = *(f.fbp + loc + 1);
    if (c2 < 0) {
        c2 += 256;
    }
    return c2;
}

int getBlue(int x, int y, framebuffer f) {
    int loc = (x+f.vinfo.xoffset) * (f.vinfo.bits_per_pixel/8) + 
            (y+f.vinfo.yoffset) * f.finfo.line_length;
    int c1 = *(f.fbp + loc);
    if (c1 < 0) {
        c1 += 256;
    }
    return c1;
}

void drawCrosshair(point p, int r, framebuffer f) {
    line up = create_line(create_point(p.x, p.y-crosshair_middle, crosshair_blue, crosshair_green, crosshair_red),
        create_point(p.x, p.y-r, crosshair_blue, crosshair_green, crosshair_red));
    line down = create_line(create_point(p.x, p.y+crosshair_middle, crosshair_blue, crosshair_green, crosshair_red),
        create_point(p.x, p.y+r, crosshair_blue, crosshair_green, crosshair_red));
    line left = create_line(create_point(p.x-crosshair_middle, p.y, crosshair_blue, crosshair_green, crosshair_red),
        create_point(p.x-r, p.y, crosshair_blue, crosshair_green, crosshair_red));
    line right = create_line(create_point(p.x+crosshair_middle, p.y, crosshair_blue, crosshair_green, crosshair_red),
        create_point(p.x+r, p.y, crosshair_blue, crosshair_green, crosshair_red));

    draw_line(up, f);
    draw_line(down, f);
    draw_line(left, f);
    draw_line(right, f);
}

void readTempCrosshair(point parr[], point p, framebuffer f) {
    int j = 0;
    for (int i = crosshair_middle; i <= crosshair_radius; i++) {
        parr[j] = create_point(p.x, p.y-i, getBlue(p.x, p.y-i, f), getGreen(p.x, p.y-i, f), getRed(p.x, p.y-i, f));
        parr[j+1] = create_point(p.x, p.y+i, getBlue(p.x, p.y+i, f), getGreen(p.x, p.y+i, f), getRed(p.x, p.y+i, f));
        parr[j+2] = create_point(p.x-i, p.y, getBlue(p.x-i, p.y, f), getGreen(p.x-i, p.y, f), getRed(p.x-i, p.y, f));
        parr[j+3] = create_point(p.x+i, p.y, getBlue(p.x+i, p.y, f), getGreen(p.x+i, p.y, f), getRed(p.x+i, p.y, f));
        j += 4;
    }
}

void drawTempCrosshair(point parr[], framebuffer f) {
    for (int i = 0; i < ((crosshair_radius-crosshair_middle)+1)*4; i++) {
        draw_point(parr[i], f);
    }
}

int isCrosshairMoveValid(point c, int dx, int dy) {
    return c.x+dx-crosshair_radius > 0 && c.x+dx+crosshair_radius < dimension_x && 
        c.y-dy-crosshair_radius > 0 && c.y-dy+crosshair_radius < dimension_y;
}