#include "framebuffer.h"
#include "point.h"
#include "line.h"
#include "polygon.h"

int main(){
	framebuffer f = init();
	
	polygon plane_origin = create_polygon_from_file("plane.txt", 0, 255, 0, 1366, 768);
	polygon frame = create_polygon_from_file("frame.txt", 255, 255, 255, 1366, 768);
	draw_polygon(frame, f);

	//draw_polygon(plane_origin, f);
	printf("x_center:%d\ny_center:%d\n", plane_origin.x_center, plane_origin.y_center);

	for(int scale=1; scale<=10; scale++){
		polygon plane = dilate(plane_origin, scale, plane_origin.x_center,plane_origin.y_center);
		draw_polygon(plane, f);
		sleep(1);
		clear_polygon(&plane);
		draw_polygon(plane, f);
	}
}