#include "framebuffer.h"
#include "point.h"
#include "line.h"
#include "polygon.h"

int main(){
	framebuffer f = init();
	
	polygon plane = create_polygon_from_file("plane.txt", 0, 255, 0, 1366, 768);
	draw_polygon(plane, f);
}