#include "framebuffer.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include <pthread.h>

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
    	k = 0;

    	while (j < polygons[i].size * 2) {
    		fscanf(fp, "%d", &temp);
    		if(j%2==0){
	            polygons[i].arr[k][0] = temp+100;
	        }
	        else{
	            polygons[i].arr[k][1] = temp+100;
	            k++;
	        }
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
        if (c == '1') {
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
        else if(c == '='){
        	if(sensitivity<10){
        		sensitivity++;
        	}
        }
        else if(c == '-'){
        	if(sensitivity>1){
        		sensitivity--;
        	}
        }
        else{
        	break;
        }
     }

    system ("/bin/stty cooked");
}

// int my_handler(Gpm_Event *event, void *data)
// {       
//          if(event->buttons & GPM_B_LEFT){
//     		status = '2';
//     		printf("left\n");
//          }
//         else if(event->buttons & GPM_B_RIGHT){
//         	status = '3';
//         	printf("right\n");
//         }
//         return 0;         
// }

// void* listenMouseEvent(void* arg){
// 	printf("Mouse\n");
// 	Gpm_Connect conn;
//     int c;
//     conn.eventMask  = ~0;   /* Want to know about all the events */
//     conn.defaultMask = 0;   /* don't handle anything by default  */
//     conn.minMod     = 0;    /* want everything                   */  
//     conn.maxMod     = ~0;    all modifiers included            
        
//     if(Gpm_Open(&conn, 0) == -1)
//         printf("Cannot connect to mouse server\n");
        
//     gpm_handler = my_handler;
//     printf("handler\n");
//     while((c = Gpm_Getc(stdin)) != EOF){
//         printf("%c", c);
//         printf("lalala\n");
//     }

//     Gpm_Close();
//     printf("end\n");
// }

// void draw_mouse(int x, int y, int c1, int c2, int c3){
// 	polygon mouse;
// 	allocate_memory(&mouse, 4);
// 	mouse.arr[0][0] = x;
// 	mouse.arr[0][1] = y;
// 	mouse.arr[1][0] = x + 2;
// 	mouse.arr[1][1] = y;
// 	mouse.arr[2][0] = x + 2;
// 	mouse.arr[2][1] = y + 2;
// 	mouse.arr[3][0] = x;
// 	mouse.arr[3][1] = y + 2;
// 	mouse.c1 = c1;
// 	mouse.c2 = c2;
// 	mouse.c3 = c3;
// 	mouse.x_resolution = xR;
// 	mouse.y_resolution = yR;

// 	draw_polygon(mouse, f);
// }

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

    int left, middle, right;
    signed char x, y;
    while(1)
    {
        // Read Mouse     
        bytes = read(fd, data, sizeof(data));

        if(bytes > 0)
        {
            left = data[0] & 0x1;
            right = data[0] & 0x2;
            middle = data[0] & 0x4;

            x = data[1];
            y = data[2];
            

            // draw_mouse(currentX, currentY, 0, 0, 0);

            // int tempX = currentX + x;
            // int tempY = currentY - y;

            // if(tempX <= xR && tempY >=0 && tempY <= yR && tempY >= 0){
            // 	currentX = tempX;
            // 	currentY = tempY;
            // }

            // draw_mouse(currentX, currentY, 255, 255, 255);

           	char dominant;

           	if(absolute(x) >= absolute(y)){
           		dominant = 'x';
           	}
           	else{
           		dominant = 'y';
           	}

           	if(dominant == 'x'){
           		if(x > 0){
           			status = 'd';
           		}
           		else{
           			status = 'a';
           		}
           	}
           	else{
           		if(y > 0){
           			status = 'w';
           		}
           		else{
           			status = 's';
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

	pthread_t tid;
	pthread_create(&tid, NULL, transformWindow, NULL);
	pthread_t mouse_thread_id;
	pthread_create(&mouse_thread_id, NULL, mouse_handler, NULL);

	status = 'o';

	int dWindowX = 0;
	int dWindowY = 0;
	currWindow = 1;

	sensitivity = 1;

	while(1) {
		if(status=='w' && isWValid()==1){
			eraseImage();
			window.arr[0][1] -= sensitivity;
			window.arr[1][1] -= sensitivity;
			window.arr[2][1] -= sensitivity;
			window.arr[3][1] -= sensitivity;
			dWindowY -= 10;
			updateImage();
		}
		else if(status=='a' && isAValid()==1){
			eraseImage();
			window.arr[0][0] -= sensitivity;
			window.arr[1][0] -= sensitivity;
			window.arr[2][0] -= sensitivity;
			window.arr[3][0] -= sensitivity;
			dWindowX -= 10;
			updateImage();
		}
		else if(status=='s' && isSValid()==1){
			eraseImage();
			window.arr[0][1] += sensitivity;
			window.arr[1][1] += sensitivity;
			window.arr[2][1] += sensitivity;
			window.arr[3][1] += sensitivity;
			dWindowY += 10;
			updateImage();
		}
		else if(status=='d' && isDValid()==1){
			eraseImage();
			window.arr[0][0] += sensitivity;
			window.arr[1][0] += sensitivity;
			window.arr[2][0] += sensitivity;
			window.arr[3][0] += sensitivity;
			dWindowX += 10;
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
	}
}