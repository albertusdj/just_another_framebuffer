#include "framebuffer.h"
#include "point.h"
#include "line.h"
#include "polygon.h"

framebuffer f;
polygon world;
polygon window;
polygon window_origin;
polygon view;
polygon image;
polygon image_origin;
polygon image2;
polygon image2_origin;
int current_scale;
char status;

polygon viewport(polygon p) {
	// Initate variables
	int xwmin = window.arr[0][0];
	int xwmax = window.arr[0][0];
	int ywmin = window.arr[0][1];
	int ywmax = window.arr[0][1];
	int xvmin = view.arr[0][0];
	int xvmax = view.arr[0][0];
	int yvmin = view.arr[0][1];
	int yvmax = view.arr[0][1];
	for(int i=1; i < 4; i++) {
		if (window.arr[i][0] < xwmin) {
			xwmin = window.arr[i][0];
		}
		if (window.arr[i][0] > xwmax) {
			xwmax = window.arr[i][0];
		}
		if (window.arr[i][1] < ywmin) {
			ywmin = window.arr[i][1];
		}
		if (window.arr[i][1] > ywmax) {
			ywmax = window.arr[i][1];
		}
		if (view.arr[i][0] < xvmin) {
			xvmin = view.arr[i][0];
		}
		if (view.arr[i][0] > xvmax) {
			xvmax = view.arr[i][0];
		}
		if (view.arr[i][1] < yvmin) {
			yvmin = view.arr[i][1];
		}
		if (view.arr[i][1] > yvmax) {
			yvmax = view.arr[i][1];
		}
	}
	float sx = (float)(xvmax - xvmin) / (float)(xwmax - xwmin);
	float sy = (float)(yvmax - yvmin) / (float)(ywmax - ywmin);
	float tx = (float)(xwmax*xvmin - xwmin*xvmax) / (float)(xwmax - xwmin);
	float ty = (float)(ywmax*yvmin - ywmin*yvmax) / (float)(ywmax - ywmin);

	// Count view coordinates
	polygon p_temp = translate(p,world,0,0);
	for (int i=0; i < p_temp.size; i++) {
		p_temp.arr[i][0] = (int)(sx*(float)p_temp.arr[i][0]+tx);
		p_temp.arr[i][1] = (int)(sy*(float)p_temp.arr[i][1]+ty);
	}

	return p_temp;
}

void drawWindow(int c1, int c2, int c3) {
	// Initiate points
	point p1 = create_point(window.arr[0][0],window.arr[0][1],c1,c2,c3);
	point p2 = create_point(window.arr[1][0],window.arr[1][1],c1,c2,c3);
	point p3 = create_point(window.arr[2][0],window.arr[2][1],c1,c2,c3);
	point p4 = create_point(window.arr[3][0],window.arr[3][1],c1,c2,c3);

	// Draw lines
	draw_line(create_line(p1,p2), f);
	draw_line(create_line(p2,p3), f);
	draw_line(create_line(p3,p4), f);
	draw_line(create_line(p4,p1), f);
}

void drawView(int c1, int c2, int c3) {
	// Initiate points
	point p1 = create_point(view.arr[0][0],view.arr[0][1],c1,c2,c3);
	point p2 = create_point(view.arr[1][0],view.arr[1][1],c1,c2,c3);
	point p3 = create_point(view.arr[2][0],view.arr[2][1],c1,c2,c3);
	point p4 = create_point(view.arr[3][0],view.arr[3][1],c1,c2,c3);

	// Draw lines
	draw_line(create_line(p1,p2), f);
	draw_line(create_line(p2,p3), f);
	draw_line(create_line(p3,p4), f);
	draw_line(create_line(p4,p1), f);
}

void eraseImage() {
	// Erase image 1
	image.c1 = 0;
	image.c2 = 0;
	image.c3 = 0;
	draw_polygon(image,f);
	draw_polygon(viewport(image), f);
	image.c1 = 0;
	image.c2 = 255;
	image.c3 = 0;

	// Erase image 2
	image2.c1 = 0;
	image2.c2 = 0;
	image2.c3 = 0;
	draw_polygon(image2,f);
	draw_polygon(viewport(image2), f);
	image2.c1 = 255;
	image2.c2 = 0;
	image2.c3 = 0;

	// Erase window
	drawWindow(0,0,0);
}

void updateImage() {
	// Update image 1
	image = translate(image_origin, window, 0, 0);
	draw_polygon(image,f);
	draw_polygon(viewport(image), f);

	// Update image 2
	image2 = translate(image2_origin, window, 0, 0);
	draw_polygon(image2,f);
	draw_polygon(viewport(image2), f);

	// Update window
	drawWindow(255,255,255);

	drawView(255,255,255);

	status = 'o';
}

void* transformWindow(void *arg) {
	system ("/bin/stty raw");
    char c;
    while (c = getchar()) {
        if (c == 'w') {
        	status = 'w';
        }
        else if (c == 'a') {
        	status = 'a';
        }
        else if (c == 's') {
        	status = 's';
        }
        else if (c == 'd') {
        	status = 'd';
        }
        else if (c == '1') {
        	status = '1';
        }
        else if (c == '2') {
        	status = '2';
        }
        else if (c == '3') {
        	status = '3';
        }
        else if (c == '4') {
        	status = '4';
        }
        else{
        	break;
        }
    }
    system ("/bin/stty cooked");
}

int main(){
	f = init();
	
	world = create_polygon_from_file("frame_world_test.txt", 255, 255, 255, 1366, 768);
	window_origin = create_polygon_from_file("frame_window_test.txt", 255, 255, 255, 1366, 768);
	view = create_polygon_from_file("frame_view_test.txt", 255, 255, 255, 1366, 768);
	image_origin = create_polygon_from_file("image_test.txt", 0, 255, 0, 1366, 768);
	image2_origin = create_polygon_from_file("image2_test.txt", 255, 0, 0, 1366, 768);

	window = translate(window_origin, world, 0, 0);
	image = translate(image_origin, window, 0, 0);
	image2 = translate(image2_origin, window, 0, 0);
	draw_polygon(image, f);
	draw_polygon(image2, f);
	draw_polygon(viewport(image), f);
	draw_polygon(viewport(image2), f);
	drawWindow(255,255,255);
	drawView(255,255,255);

	pthread_t tid;
	pthread_create(&tid, NULL, transformWindow, NULL);

	status = 'o';

	int dWindowX = 0;
	int dWindowY = 0;

	while(1) {
		if(status=='w'){
			eraseImage();
			window.arr[0][1] -= 10;
			window.arr[1][1] -= 10;
			window.arr[2][1] -= 10;
			window.arr[3][1] -= 10;
			dWindowY -= 10;
			updateImage();
		}
		else if(status=='a'){
			eraseImage();
			window.arr[0][0] -= 10;
			window.arr[1][0] -= 10;
			window.arr[2][0] -= 10;
			window.arr[3][0] -= 10;
			dWindowX -= 10;
			updateImage();
		}
		else if(status=='s'){
			eraseImage();
			window.arr[0][1] += 10;
			window.arr[1][1] += 10;
			window.arr[2][1] += 10;
			window.arr[3][1] += 10;
			dWindowY += 10;
			updateImage();
		}
		else if(status=='d'){
			eraseImage();
			window.arr[0][0] += 10;
			window.arr[1][0] += 10;
			window.arr[2][0] += 10;
			window.arr[3][0] += 10;
			dWindowX += 10;
			updateImage();
		}
		else if(status=='1'){
			eraseImage();
			window = dilate(translate(window_origin,world,dWindowX,dWindowY), world, 1, (window.arr[0][0]+window.arr[1][0])/2, (window.arr[0][1]+window.arr[2][1])/2);
			updateImage();
		}
		else if(status=='2'){
			eraseImage();
			window = dilate(translate(window_origin,world,dWindowX,dWindowY), world, 2, (window.arr[0][0]+window.arr[1][0])/2, (window.arr[0][1]+window.arr[2][1])/2);
			updateImage();
		}
		else if(status=='3'){
			eraseImage();
			window = dilate(translate(window_origin,world,dWindowX,dWindowY), world, 3, (window.arr[0][0]+window.arr[1][0])/2, (window.arr[0][1]+window.arr[2][1])/2);
			updateImage();
		}
		else if(status=='4'){
			eraseImage();
			window = dilate(translate(window_origin,world,dWindowX,dWindowY), world, 4, (window.arr[0][0]+window.arr[1][0])/2, (window.arr[0][1]+window.arr[2][1])/2);
			updateImage();
		}
	}
}