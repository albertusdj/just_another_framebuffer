#include "framebuffer.h"
#include "point.h"
#include "line.h"
#include "polygon.h"

#define MAX_ZOOM_IN 8
#define MIN_ZOOM_OUT 1

framebuffer f;
polygon frame;
polygon plane_origin;
polygon rotor1_origin;
polygon rotated_rotor1_origin;
polygon rotor2_origin;
polygon rotated_rotor2_origin;
polygon pilot_origin;
polygon plane;
polygon pilot;
polygon rotor1;
polygon rotor2;
polygon rotated_rotor1;
polygon rotated_rotor2;
int current_scale;
char status;
int current_x_center;
int current_y_center;

// void dilate_all(int scale){
// 	plane = dilate(plane_origin, frame, current_scale, current_x_center, current_y_center);
// 	pilot = dilate(pilot_origin, frame, current_scale, current_x_center, current_y_center);
// 	rotor1 = dilate(rotor1_origin, frame, current_scale, current_x_center, current_y_center);
// 	rotor2 = dilate(rotor2_origin, frame, current_scale, current_x_center, current_y_center);
// 	rotated_rotor1 = dilate(rotated_rotor1_origin, frame, current_scale, current_x_center, current_y_center);
// 	rotated_rotor2 = dilate(rotated_rotor2_origin, frame, current_scale, current_x_center, current_y_center);
// }

void update_all(int dx, int dy) {
	plane = translate(plane_origin, frame, dx, dy);
	pilot = translate(pilot_origin, frame, dx, dy);
	rotor1 = translate(rotor1_origin, frame, dx, dy);
	rotor2 = translate(rotor2_origin, frame, dx, dy);
	rotated_rotor1 = translate(rotated_rotor1_origin, frame, dx, dy);
	rotated_rotor2 = translate(rotated_rotor2_origin, frame, dx, dy);

	plane = dilate(plane, frame, current_scale, current_x_center, current_y_center);
	pilot = dilate(pilot, frame, current_scale, current_x_center, current_y_center);
	rotor1 = dilate(rotor1, frame, current_scale, current_x_center, current_y_center);
	rotor2 = dilate(rotor2, frame, current_scale, current_x_center, current_y_center);
	rotated_rotor1 = dilate(rotated_rotor1, frame, current_scale, current_x_center, current_y_center);
	rotated_rotor2 = dilate(rotated_rotor2, frame, current_scale, current_x_center, current_y_center);

}



void* zoom(void *arg){
	system ("/bin/stty raw");
    char c;
    while (c = getchar()) {
        if (c == 'z') {
            status = 'z';
        }
        else if (c == 'x') {
            status = 'x';
        }
        else{
        	break;
        }
    }
    system ("/bin/stty cooked");
}

int main(){
	f = init();
	
	current_scale = 1;

	frame = create_polygon_from_file("frame_plane.txt", 255, 255, 255, 1366, 768);
	plane_origin = create_polygon_from_file("plane.txt", 0, 255, 0, 1366, 768);
	rotor1_origin = create_polygon_from_file("rotor.txt", 0, 0, 255, 1366, 768);
	rotated_rotor1_origin = rotate(rotor1_origin, 592, 351);
	rotor2_origin = translate(rotor1_origin, frame, 160, 0);
	rotated_rotor2_origin = rotate(rotor2_origin, 752, 351);
	pilot_origin = create_polygon_from_file("pilot.txt", 255, 0, 0, 1366, 768);
	
	plane = translate(plane_origin, frame, 0, 0);
	pilot = translate(pilot_origin, frame, 0, 0);
	rotor1 = translate(rotor1_origin, frame, 0, 0);
	rotor2 = translate(rotor2_origin, frame, 0, 0);
	rotated_rotor1 = translate(rotated_rotor1_origin, frame, 0, 0);
	rotated_rotor2 = translate(rotated_rotor2_origin, frame, 0, 0);

	draw_polygon(frame, f);
	drawToScreen(&f);

	pthread_t tid;
	pthread_create(&tid, NULL, zoom, NULL);

	int limit = 4;
	int move_count = 0;
	int dx = 5;
	int dy;
	char direction = 'r';
	int current_x = current_x_center;
	int current_y = current_y_center;

	status = 'o';

	while(1){
		draw_polygon(plane, f);
		draw_polygon(pilot, f);

		drawToScreen(&f);

		time_t begin, end;
		double time_spent = 0;
		begin = time(NULL);

		int count = 0;
		while(time_spent<=0.05){
			set_color_polygon(&rotor1, 0, 0, 255);
			set_color_polygon(&rotor2, 0, 0, 255);
			set_color_polygon(&rotated_rotor1, 0, 0, 255);
			set_color_polygon(&rotated_rotor2, 0, 0, 255);
			if(count%2==0){
				draw_polygon(rotor1, f);
				draw_polygon(rotor2, f);
				drawToScreen(&f);
				usleep(10000);
				set_color_polygon(&rotor1, 0, 255, 0);
				set_color_polygon(&rotor2, 0, 255, 0);
				draw_polygon(rotor1, f);
				draw_polygon(rotor2, f);
				drawToScreen(&f);
			}
			else{
				draw_polygon(rotated_rotor1, f);
				draw_polygon(rotated_rotor2, f);
				drawToScreen(&f);
				usleep(10000);
				set_color_polygon(&rotated_rotor1, 0, 255, 0);
				set_color_polygon(&rotated_rotor2, 0, 255, 0);
				draw_polygon(rotated_rotor1, f);
				draw_polygon(rotated_rotor2, f);
				drawToScreen(&f);
			}
			count++;
			end = time(NULL);
			time_spent = (double) (end - begin);
		}

		draw_polygon(frame, f);
		drawToScreen(&f);

		if(direction=='r' && move_count<limit){
			move_count++;
		}
		else if(direction=='r' && move_count>=limit){
			dx = -5;
			direction ='l';
		}
		else if(direction=='l' && move_count > (-1*limit)){
			move_count--;
		}
		else if(direction=='l' && dx <= (-1*limit)){
			dx = 5;
			direction = 'r';
		}
		current_x += dx;
		current_y += dy;

		update_all(current_x,current_y);
		//dilate_all(current_scale);

		// plane = translate(plane, frame, dx, 0);
		// pilot = translate(pilot, frame, dx, 0);
		// rotor1 = translate(rotor1, frame, dx, 0);
		// rotor2 = translate(rotor2, frame, dx, 0);
		// rotated_rotor1 = translate(rotated_rotor1, frame, dx, 0);
		// rotated_rotor2 = translate(rotated_rotor2, frame, dx, 0);

		
		current_x_center = plane.x_center;
		current_y_center = plane.y_center;

		if(status=='z'){
			if(current_scale < MAX_ZOOM_IN){
				current_scale++;
				update_all(current_x,current_y);
				//dilate_all(current_scale);
			}
		}
		else if(status=='x'){
			if(current_scale > MIN_ZOOM_OUT){
            	current_scale--;
            	update_all(current_x,current_y);
				//dilate_all(current_scale);
            }
		}
		status = 'o';
	}

	// for(int scale=1; scale<=8; scale++){
	// 	polygon plane = dilate(plane_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
	// 	polygon rotor1 = dilate(rotor1_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
	// 	polygon rotor2 = dilate(rotor2_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
	// 	polygon rotated_rotor1 = dilate(rotated_rotor1_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
	// 	polygon rotated_rotor2 = dilate(rotated_rotor2_origin, frame, scale, plane_origin.x_center,plane_origin.y_center);
	// 	draw_polygon(plane, f);
		
	// 	time_t begin, end;
	// 	double time_spent = 0;
	// 	begin = time(NULL);

	// 	int count = 0;
	// 	while(time_spent<=1){
	// 		set_color_polygon(&rotor1, 0, 0, 255);
	// 		set_color_polygon(&rotor2, 0, 0, 255);
	// 		set_color_polygon(&rotated_rotor1, 0, 0, 255);
	// 		set_color_polygon(&rotated_rotor2, 0, 0, 255);
	// 		if(count%2==0){
	// 			draw_polygon(rotor1, f);
	// 			draw_polygon(rotor2, f);
	// 			usleep(100000);
	// 			set_color_polygon(&rotor1, 0, 255, 0);
	// 			set_color_polygon(&rotor2, 0, 255, 0);
	// 			draw_polygon(rotor1, f);
	// 			draw_polygon(rotor2, f);
	// 		}
	// 		else{
	// 			draw_polygon(rotated_rotor1, f);
	// 			draw_polygon(rotated_rotor2, f);
	// 			usleep(100000);
	// 			set_color_polygon(&rotated_rotor1, 0, 255, 0);
	// 			set_color_polygon(&rotated_rotor2, 0, 255, 0);
	// 			draw_polygon(rotated_rotor1, f);
	// 			draw_polygon(rotated_rotor2, f);
	// 		}
	// 		count++;
	// 		end = time(NULL);
	// 		time_spent = (double) (end - begin);
	// 	}

	// 	if (scale != 8) {
	// 		set_color_polygon(&plane, 255, 255, 255);
	// 	}
	// 	draw_polygon(plane, f);
	// 	printf("\n");
	// }
}