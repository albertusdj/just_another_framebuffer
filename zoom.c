#include "framebuffer.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include "crosshair.h"

#include <pthread.h>
#include <string.h>

framebuffer f;
polygon world;
polygon window;
polygon window_origin;
polygon view;
polygon image;
polygon image_origin;
polygon image2;
polygon image2_origin;
polygon *polygons;
polygon *polygons_origin;
int nPolygon;
int kantin_idx;
int jalan_idx;
int parkiran_idx;
int hijau_idx;
int ass_idx;
int current_scale;
char status;
int kantin_aktif;
int jalan_aktif;
int parkiran_aktif;
int hijau_aktif;
int ass_aktif;
int building_aktif;
int currWindow;
int sensitivity;
int is_mouse_shown;

void ignore_read(FILE *fp) {
	char buffer[64];
    char *b = buffer;
    size_t bufsize = 64;
    size_t characters;

    characters = getline(&b,&bufsize,fp);
    while (buffer[0] != '/') {
    	characters = getline(&b,&bufsize,fp);
    }
}

void init_polygons(char* filename, int x_resolution, int y_resolution) {
	FILE *fp;
     
    fp = fopen(filename, "r");
    ignore_read(fp);
    fscanf(fp, "%d", &nPolygon);
    ignore_read(fp);
    fscanf(fp, "%d", &kantin_idx);
    ignore_read(fp);
    fscanf(fp, "%d", &jalan_idx);
    ignore_read(fp);
    fscanf(fp, "%d", &parkiran_idx);
	ignore_read(fp);
    fscanf(fp, "%d", &hijau_idx);
    ignore_read(fp);
    fscanf(fp, "%d", &ass_idx);

    polygons = (polygon *) malloc (nPolygon * sizeof(polygon));
    polygons_origin = (polygon *) malloc (nPolygon * sizeof(polygon));

    int i = 0;
    int j,k;

    int temp;
    while(i < nPolygon){

    	if (i == 0 || i == kantin_idx || i == jalan_idx || i == parkiran_idx || i == hijau_idx || i == ass_idx) {
    		ignore_read(fp);
    	}

    	fscanf(fp, "%d", &temp);
    	polygons[i].size = temp;
    	polygons[i].c1 = 255;
		polygons[i].c2 = 102;
		polygons[i].c3 = 0;
		polygons[i].x_resolution = x_resolution;
		polygons[i].y_resolution = y_resolution;

		allocate_memory(&polygons[i],polygons[i].size);
    	allocate_memory(&polygons_origin[i], polygons[i].size);

		j = 0;

    	while (j < polygons[i].size) {
    		fscanf(fp, "%d", &temp);
            polygons[i].arr[j][0] = temp+100;
            fscanf(fp, "%d", &temp);
            polygons[i].arr[j][1] = temp+100;
	        j++;
    	}
    	
    	polygons_origin[i] = translate(polygons[i], world, 0, 0);
    	i++;
    }

    fclose(fp);

    building_aktif = 1;

    kantin_aktif = 1;
    for (int i=kantin_idx; i<jalan_idx; i++) {
		polygons[i].c1 = 51;
		polygons[i].c2 = 133;
		polygons[i].c3 = 255;
		polygons_origin[i].c1 = 51;
		polygons_origin[i].c2 = 133;
		polygons_origin[i].c3 = 255;
	}

	jalan_aktif = 1;
	for (int i=jalan_idx; i<parkiran_idx; i++) {
    	//printf("%d %d %d\n", polygons[i].c1, polygons[i].c2, polygons[i].c3);
		polygons[i].c1 = 99;
		polygons[i].c2 = 99;
		polygons[i].c3 = 99;
		polygons_origin[i].c1 = 99;
		polygons_origin[i].c2 = 99;
		polygons_origin[i].c3 = 99;
	}

	parkiran_aktif = 1;
	for (int i=parkiran_idx; i<hijau_idx; i++) {
    	//printf("%d %d %d\n", polygons[i].c1, polygons[i].c2, polygons[i].c3);
		polygons[i].c1 = 255;
		polygons[i].c2 = 99;
		polygons[i].c3 = 222;
		polygons_origin[i].c1 = 255;
		polygons_origin[i].c2 = 99;
		polygons_origin[i].c3 = 222;
	}

	hijau_aktif = 1;
	for (int i=hijau_idx; i<ass_idx; i++) {
    	//printf("%d %d %d\n", polygons[i].c1, polygons[i].c2, polygons[i].c3);
		polygons[i].c1 = 78;
		polygons[i].c2 = 189;
		polygons[i].c3 = 78;
		polygons_origin[i].c1 = 78;
		polygons_origin[i].c2 = 189;
		polygons_origin[i].c3 = 78;
	}

	ass_aktif = 1;
	for (int i=ass_idx; i<nPolygon; i++) {
    	//printf("%d %d %d\n", polygons[i].c1, polygons[i].c2, polygons[i].c3);
		polygons[i].c1 = 251;
		polygons[i].c2 = 255;
		polygons[i].c3 = 102;
		polygons_origin[i].c1 = 251;
		polygons_origin[i].c2 = 255;
		polygons_origin[i].c3 = 102;
	}

}

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
	int c1, c2, c3, c4, c5, c6;
	for (int i=0; i<nPolygon; i++) {
		c1 = polygons[i].c1;
		c2 = polygons[i].c2;
		c3 = polygons[i].c3;
		c4 = polygons_origin[i].c1;
		c5 = polygons_origin[i].c2;		
		c6 = polygons_origin[i].c3;
		polygons[i].c1 = 0;
		polygons[i].c2 = 0;
		polygons[i].c3 = 0;
		polygons_origin[i].c1 = 0;
		polygons_origin[i].c2 = 0;
		polygons_origin[i].c3 = 0;
		draw_polygon(polygons_origin[i], f);
		draw_polygon(viewport(polygons[i]), f);
		polygons[i].c1 = c1;
		polygons[i].c2 = c2;
		polygons[i].c3 = c3;
		polygons_origin[i].c1 = c4;
		polygons_origin[i].c2 = c5;
		polygons_origin[i].c3 = c6;
	}

	// Erase window
	drawWindow(0,0,0);
	// for (int i=0; i<nPolygon; i++) {
	// 	draw_polygon(polygons_origin[i], f);
	// }
}

void updateImage() {
	// Update images
	if (hijau_aktif) {
		for (int i=hijau_idx; i<ass_idx; i++) {
			polygons[i] = translate(polygons_origin[i], window, 0, 0);
			draw_polygon(polygons_origin[i], f);
			draw_polygon(viewport(polygons[i]), f);
		}	
	}
	if (building_aktif) {
		for (int i=0; i<kantin_idx; i++) {
			polygons[i] = translate(polygons_origin[i], window, 0, 0);
			draw_polygon(polygons_origin[i], f);
			draw_polygon(viewport(polygons[i]), f);
		}
	}
	if (kantin_aktif) {
		for (int i=kantin_idx; i<jalan_idx; i++) {
			polygons[i] = translate(polygons_origin[i], window, 0, 0);
			draw_polygon(polygons_origin[i], f);
			draw_polygon(viewport(polygons[i]), f);
		}	
	}
	if (jalan_aktif) {
		for (int i=jalan_idx; i<parkiran_idx; i++) {
			polygons[i] = translate(polygons_origin[i], window, 0, 0);
			draw_polygon(polygons_origin[i], f);
			draw_polygon(viewport(polygons[i]), f);
		}	
	}
	if (parkiran_aktif) {
		for (int i=parkiran_idx; i<hijau_idx; i++) {
			polygons[i] = translate(polygons_origin[i], window, 0, 0);
			draw_polygon(polygons_origin[i], f);
			draw_polygon(viewport(polygons[i]), f);
		}	
	}
	if (ass_aktif) {
		for (int i=ass_idx; i<nPolygon; i++) {
			polygons[i] = translate(polygons_origin[i], window, 0, 0);
			draw_polygon(polygons_origin[i], f);
			draw_polygon(viewport(polygons[i]), f);
		}	
	}

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
        else if (c == 's') {
        	status = 's';
        }
        else if (c == 'd') {
        	status = 'd';
        }
        else if (c == 'a') {
        	status = 'a';
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
        else if (c == 'l') { // kantin
        	status = 'l';
        }
        else if (c == 'k') { // jalan
        	status = 'k';
        }
        else if (c == 'j') { // parkiran
        	status = 'j';
        }
        else if (c == 'h') { // hijau
        	status = 'h';
        }
        else if (c == 'g') { // ass
        	status = 'g';
        }
        else if (c == 'f') { // building
        	status = 'f';
        }
        else if (c == 1){
        	is_mouse_shown ^= 1;
        }
        else{
        	break;
        }
     }

    system ("/bin/stty cooked");
}

int absolute(int x){
	if(x>=0){
		return x;
	}
	else{
		return -1*x;
	}
}

void* mouse_handler(void* arg){
	int fd, bytes;
    unsigned char data[3];

    const char *pDevice = "/dev/input/mice";

    // Open Mouse
    fd = open(pDevice, O_RDWR);
    if(fd == -1)
    {
        printf("ERROR Opening %s\n", pDevice);
        
    }

    point crosshair_temp[((crosshair_radius-crosshair_middle)+1)*4];

    // Create crosshair
    point crosshair = create_point(dimension_x/2, (dimension_y)/2, crosshair_blue, crosshair_green, crosshair_red);
    readTempCrosshair(crosshair_temp, crosshair, f);
    drawCrosshair(crosshair, crosshair_radius, f);

    int left, middle, right;
    signed char dx, dy;
    while(1)
    {	
    		bytes = read(fd, data, sizeof(data));
	    	
	    	if(!is_mouse_shown){
		        if(bytes > 0)
		        {
		            left = data[0] & 0x1;
		            right = data[0] & 0x2;
		            middle = data[0] & 0x4;

		            dx = data[1];
		            dy = data[2];

		           	char dominant;

		           	if(absolute(dx) >= absolute(dy)){
		           		dominant = 'x';
		           	}
		           	else{
		           		dominant = 'y';
		           	}

		           	if(dominant == 'x'){
		           		if(dx > 0){
		           			status = 'd';
		           		}
		           		else{
		           			status = 'a';
		           		}
		           	}
		           	else{
		           		if(dy > 0){
		           			status = 'w';
		           		}
		           		else{
		           			status = 's';
		           		}
		           	}
		        }
		    }
		    else{
		    	if (bytes > 0) {
		            left = data[0] & 0x1;
		            right = data[0] & 0x2;
		            middle = data[0] & 0x4;

		            dx = data[1];
		            dy = data[2];

		            if (isCrosshairMoveValid(crosshair, dx, dy)) {
		                drawTempCrosshair(crosshair_temp, f);

		                crosshair.x += dx;
		                crosshair.y -= dy;

		                readTempCrosshair(crosshair_temp, crosshair, f);
		                drawCrosshair(crosshair, crosshair_radius, f);
		            }

            
        		}
		    }   
    }
}

int isWValid() {
	return window.arr[0][1] > 100 && window.arr[1][1] > 100 && window.arr[2][1] > 100 && window.arr[3][1] > 100;
}

int isAValid() {
	return window.arr[0][0] > 120 && window.arr[1][0] > 120 && window.arr[2][0] > 120 && window.arr[3][0] > 120;
}

int isSValid() {
	return window.arr[0][1] < 520 && window.arr[1][1] < 520 && window.arr[2][1] < 520 && window.arr[3][1] < 520;
}

int isDValid() {
	return window.arr[0][0] < 460 && window.arr[1][0] < 460 && window.arr[2][0] < 460 && window.arr[3][0] < 460;
}

int isDilateValid(int d) {
	return window.arr[0][1] >= 100+d*20 && window.arr[1][1] >= 100+d*20 && window.arr[2][1] >= 100+d*20 && window.arr[3][1] >= 100+d*20 &&
		window.arr[0][0] >= 120+d*20 && window.arr[1][0] >= 120+d*20 && window.arr[2][0] >= 120+d*20 && window.arr[3][0] >= 120+d*20 &&
		window.arr[0][1] <= 520-d*20 && window.arr[1][1] <= 520-d*20 && window.arr[2][1] <= 520-d*20 && window.arr[3][1] <= 520-d*20 &&
		window.arr[0][0] <= 460-d*20 && window.arr[1][0] <= 460-d*20 && window.arr[2][0] <= 460-d*20 && window.arr[3][0] <= 460-d*20;
}

void geser(polygon *p, int x, int y){
	(*p).x_center += x;
	(*p).y_center += y;

	for(int i=0; i < (*p).size; i++){
		(*p).arr[i][0] += x;
		(*p).arr[i][1] += y;
	}
}

int main() {

	f = init();
	
	world = create_polygon_from_file("frame_world_test.txt", 255, 255, 255, 1366, 768);
	window_origin = create_polygon_from_file("frame_window_test.txt", 255, 255, 255, 1366, 768);
	view = create_polygon_from_file("frame_view_test.txt", 255, 255, 255, 1366, 768);

	init_polygons("output.txt", 1366, 768);

	window = translate(window_origin, world, 0, 0);
	updateImage();
	for (int i=0; i<nPolygon; i++) {
		polygons[i] = translate(polygons_origin[i], window, 0, 0);
		//draw_polygon(polygons_origin[i], f);
		draw_polygon(viewport(polygons[i]), f);
	}
	printf("%d %d %d\n", polygons[jalan_idx].c1, polygons[jalan_idx].c2, polygons[jalan_idx].c3);

	drawWindow(255,255,255);
	drawView(255,255,255);

	is_mouse_shown = 0;

	pthread_t tid;
	pthread_create(&tid, NULL, transformWindow, NULL);
	pthread_t mouse_thread_id;
	pthread_create(&mouse_thread_id, NULL, mouse_handler, NULL);

	status = 'o';

	int dWindowX = 0;
	int dWindowY = 0;
	currWindow = 1;

	sensitivity = 10;

	// Lethal hardcoding

	// Draw first button
	polygon char_1 = create_polygon_from_file("huruf/k.txt", 255, 255, 255, 1366, 768);
	polygon kotak_1;
	allocate_memory(&kotak_1, 4);
	kotak_1.arr[0][0] = 150;
	kotak_1.arr[0][1] = 600;
	kotak_1.arr[1][0] = 186;
	kotak_1.arr[1][1] = 600;
	kotak_1.arr[2][0] = 186;
	kotak_1.arr[2][1] = 654;
	kotak_1.arr[3][0] = 150;
	kotak_1.arr[3][1] = 654;
	kotak_1.c1 = 255;
	kotak_1.c2 = 102;
	kotak_1.c3 = 0;
	geser(&char_1, -45, 405);
	draw_polygon(kotak_1, f);
	draw_polygon(char_1, f);

	// Draw second button
	polygon char_2 = create_polygon_from_file("huruf/j.txt", 255, 255, 255, 1366, 768);
	polygon kotak_2;
	allocate_memory(&kotak_2, 4);
	kotak_2.arr[0][0] = 350;
	kotak_2.arr[0][1] = 600;
	kotak_2.arr[1][0] = 386;
	kotak_2.arr[1][1] = 600;
	kotak_2.arr[2][0] = 386;
	kotak_2.arr[2][1] = 654;
	kotak_2.arr[3][0] = 350;
	kotak_2.arr[3][1] = 654;
	kotak_2.c1 = 255;
	kotak_2.c2 = 102;
	kotak_2.c3 = 0;
	geser(&char_2, 155, 405);
	draw_polygon(kotak_2, f);
	draw_polygon(char_2, f);

	// Draw third button
	polygon char_3 = create_polygon_from_file("huruf/j.txt", 255, 255, 255, 1366, 768);
	polygon kotak_3;
	allocate_memory(&kotak_3, 4);
	kotak_3.arr[0][0] = 550;
	kotak_3.arr[0][1] = 600;
	kotak_3.arr[1][0] = 586;
	kotak_3.arr[1][1] = 600;
	kotak_3.arr[2][0] = 586;
	kotak_3.arr[2][1] = 654;
	kotak_3.arr[3][0] = 550;
	kotak_3.arr[3][1] = 654;
	kotak_3.c1 = 255;
	kotak_3.c2 = 102;
	kotak_3.c3 = 0;
	geser(&char_3, 355, 405);
	draw_polygon(kotak_3, f);
	draw_polygon(char_3, f);

	// Draw fourth button
	polygon char_4 = create_polygon_from_file("huruf/j.txt", 255, 255, 255, 1366, 768);
	polygon kotak_4;
	allocate_memory(&kotak_4, 4);
	kotak_4.arr[0][0] = 750;
	kotak_4.arr[0][1] = 600;
	kotak_4.arr[1][0] = 786;
	kotak_4.arr[1][1] = 600;
	kotak_4.arr[2][0] = 786;
	kotak_4.arr[2][1] = 654;
	kotak_4.arr[3][0] = 750;
	kotak_4.arr[3][1] = 654;
	kotak_4.c1 = 255;
	kotak_4.c2 = 102;
	kotak_4.c3 = 0;
	geser(&char_4, 555, 405);
	draw_polygon(kotak_4, f);
	draw_polygon(char_4, f);

	// Draw fifth button
	polygon char_5 = create_polygon_from_file("huruf/j.txt", 255, 255, 255, 1366, 768);
	polygon kotak_5;
	allocate_memory(&kotak_5, 4);
	kotak_5.arr[0][0] = 950;
	kotak_5.arr[0][1] = 600;
	kotak_5.arr[1][0] = 986;
	kotak_5.arr[1][1] = 600;
	kotak_5.arr[2][0] = 986;
	kotak_5.arr[2][1] = 654;
	kotak_5.arr[3][0] = 950;
	kotak_5.arr[3][1] = 654;
	kotak_5.c1 = 255;
	kotak_5.c2 = 102;
	kotak_5.c3 = 0;
	geser(&char_5, 755, 405);
	draw_polygon(kotak_5, f);
	draw_polygon(char_5, f);

	// Draw sixth button
	polygon char_6 = create_polygon_from_file("huruf/j.txt", 255, 255, 255, 1366, 768);
	polygon kotak_6;
	allocate_memory(&kotak_6, 4);
	kotak_6.arr[0][0] = 1150;
	kotak_6.arr[0][1] = 600;
	kotak_6.arr[1][0] = 1186;
	kotak_6.arr[1][1] = 600;
	kotak_6.arr[2][0] = 1186;
	kotak_6.arr[2][1] = 654;
	kotak_6.arr[3][0] = 1150;
	kotak_6.arr[3][1] = 654;
	kotak_6.c1 = 255;
	kotak_6.c2 = 102;
	kotak_6.c3 = 0;
	geser(&char_6, 955, 405);
	draw_polygon(kotak_6, f);
	draw_polygon(char_6, f);

	while(1) {
		if(status=='w' && isWValid()==1){
			eraseImage();
			window.arr[0][1] -= sensitivity;
			window.arr[1][1] -= sensitivity;
			window.arr[2][1] -= sensitivity;
			window.arr[3][1] -= sensitivity;
			dWindowY -= sensitivity;
			updateImage();
		}
		else if(status=='a' && isAValid()==1){
			eraseImage();
			window.arr[0][0] -= sensitivity;
			window.arr[1][0] -= sensitivity;
			window.arr[2][0] -= sensitivity;
			window.arr[3][0] -= sensitivity;
			dWindowX -= sensitivity;
			updateImage();
		}
		else if(status=='s' && isSValid()==1){
			eraseImage();
			window.arr[0][1] += sensitivity;
			window.arr[1][1] += sensitivity;
			window.arr[2][1] += sensitivity;
			window.arr[3][1] += sensitivity;
			dWindowY += sensitivity;
			updateImage();
		}
		else if(status=='d' && isDValid()==1){
			eraseImage();
			window.arr[0][0] += sensitivity;
			window.arr[1][0] += sensitivity;
			window.arr[2][0] += sensitivity;
			window.arr[3][0] += sensitivity;
			dWindowX += sensitivity;
			updateImage();
		}
		else if(status=='1' && (currWindow>1 || isDilateValid(1-currWindow)==1)){
			eraseImage();
			window = dilate(translate(window_origin,world,dWindowX,dWindowY), world, 1, (window.arr[0][0]+window.arr[1][0])/2, (window.arr[0][1]+window.arr[2][1])/2);
			currWindow = 1;
			updateImage();
		}
		else if(status=='2' && (currWindow>2 || isDilateValid(2-currWindow)==1)){
			eraseImage();
			window = dilate(translate(window_origin,world,dWindowX,dWindowY), world, 2, (window.arr[0][0]+window.arr[1][0])/2, (window.arr[0][1]+window.arr[2][1])/2);
			currWindow = 2;
			updateImage();
		}
		else if(status=='3' && (currWindow>3 || isDilateValid(3-currWindow)==1)){
			eraseImage();
			window = dilate(translate(window_origin,world,dWindowX,dWindowY), world, 3, (window.arr[0][0]+window.arr[1][0])/2, (window.arr[0][1]+window.arr[2][1])/2);
			currWindow = 3;
			updateImage();
		}
		else if(status=='4' && (currWindow>4 || isDilateValid(4-currWindow)==1)){
			eraseImage();
			window = dilate(translate(window_origin,world,dWindowX,dWindowY), world, 4, (window.arr[0][0]+window.arr[1][0])/2, (window.arr[0][1]+window.arr[2][1])/2);
			currWindow = 4;
			updateImage();
		}
		else if(status=='l') {
			eraseImage();
			if (kantin_aktif) {
        		kantin_aktif = 0;
        	} else {
        		kantin_aktif = 1;
        	}
			updateImage();
		}
		else if(status=='k') {
			eraseImage();
			if (jalan_aktif) {
        		jalan_aktif = 0;
        	} else {
        		jalan_aktif = 1;
        	}
			updateImage();
		}
		else if(status=='j') {
			eraseImage();
			if (parkiran_aktif) {
        		parkiran_aktif = 0;
        	} else {
        		parkiran_aktif = 1;
        	}
			updateImage();
		}
		else if(status=='h') {
			eraseImage();
			if (hijau_aktif) {
        		hijau_aktif = 0;
        	} else {
        		hijau_aktif = 1;
        	}
			updateImage();
		}
		else if(status=='g') {
			eraseImage();
			if (ass_aktif) {
        		ass_aktif = 0;
        	} else {
        		ass_aktif = 1;
        	}
			updateImage();
		}
		else if(status=='f') {
			eraseImage();
			if (building_aktif) {
        		building_aktif = 0;
        	} else {
        		building_aktif = 1;
        	}
			updateImage();
		}

		drawToScreen(&f);
	}
}