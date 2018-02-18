#include "framebuffer.h"
#include "point.h"
#include "line.h"
#include "polygon.h"

int main(){
	framebuffer f = init();
	
	polygon plane_origin = create_polygon_from_file("plane.txt", 0, 255, 0, 1366, 768);
	polygon rotor1_origin = create_polygon_from_file("rotor.txt", 0, 0, 255, 1366, 768);
	polygon rotated_rotor1_origin = rotate(rotor1_origin, 592, 351);
	polygon rotor2_origin = translate(rotor1_origin, 160, 0);
	polygon rotated_rotor2_origin = rotate(rotor2_origin, 752, 351);
	polygon frame = create_polygon_from_file("frame.txt", 255, 255, 255, 1366, 768);
	draw_polygon(frame, f);

	//draw_polygon(plane_origin, f);
	//printf("x_center:%d\ny_center:%d\n", plane_origin.x_center, plane_origin.y_center);

	for(int scale=1; scale<=8; scale++){
		polygon plane = dilate(plane_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
		polygon rotor1 = dilate(rotor1_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
		polygon rotor2 = dilate(rotor2_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
		polygon rotated_rotor1 = dilate(rotated_rotor1_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
		polygon rotated_rotor2 = dilate(rotated_rotor2_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
		draw_polygon(plane, f);
		
		time_t begin, end;
		double time_spent = 0;
		begin = time(NULL);

		int count = 0;
		while(time_spent<=1){
			set_color_polygon(&rotor1, 0, 0, 255);
			set_color_polygon(&rotor2, 0, 0, 255);
			set_color_polygon(&rotated_rotor1, 0, 0, 255);
			set_color_polygon(&rotated_rotor2, 0, 0, 255);
			if(count%2==0){
				draw_polygon(rotor1, f);
				draw_polygon(rotor2, f);
				usleep(100000);
				set_color_polygon(&rotor1, 0, 255, 0);
				set_color_polygon(&rotor2, 0, 255, 0);
				draw_polygon(rotor1, f);
				draw_polygon(rotor2, f);
			}
			else{
				draw_polygon(rotated_rotor1, f);
				draw_polygon(rotated_rotor2, f);
				usleep(100000);
				set_color_polygon(&rotated_rotor1, 0, 255, 0);
				set_color_polygon(&rotated_rotor2, 0, 255, 0);
				draw_polygon(rotated_rotor1, f);
				draw_polygon(rotated_rotor2, f);
			}
			count++;
			end = time(NULL);
			time_spent = (double) (end - begin);
			printf("%lf\n", time_spent);
		}

		if (scale != 8) {
			set_color_polygon(&plane, 255, 255, 255);
		}
		draw_polygon(plane, f);
		printf("\n");
	}
}