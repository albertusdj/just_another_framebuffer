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
#include "line.h"
#include "framebuffer.h"
#include "polygon.h"

void allocate_memory(polygon *pol, int size){
	pol->size = size;
	pol->arr = (int **)malloc((pol->size+1) * sizeof(int *));

	for (int i=0; i<(pol->size+1); i++){
         pol->arr[i] = (int *)malloc(2 * sizeof(int));
    }
}

polygon create_polygon_from_file(char *filename, int c1, int c2, int c3, int x_resolution, int y_resolution){
	polygon pol;
	pol.c1 = c1;
	pol.c2 = c2;
	pol.c3 = c3;
	pol.x_resolution = x_resolution;
	pol.y_resolution = y_resolution;

	FILE *fp;  
    
    int size;
    fp = fopen(filename, "r");
    fscanf(fp, "%d", &pol.size);

    allocate_memory(&pol, pol.size);

    int i = 0;
    int j = 0;

    int temp;
    while(fscanf(fp, "%d", &temp)!=EOF){  
        if(j%2==0){
            pol.arr[i][0] = temp;
        }
        else{
            pol.arr[i][1] = temp;
            i++;
        }
        j++;  
    }
    
    fclose(fp);

    //Find center of polygon
    int maxX = INT_MIN;
    int minX = INT_MAX;

    int maxY = INT_MIN;
    int minY = INT_MAX;

    for(int i=0; i<pol.size; i++){
    	if(maxX < pol.arr[i][0]){
    		maxX = pol.arr[i][0];
    	}
    	if(minX > pol.arr[i][0]){
    		minX = pol.arr[i][0];
    	}

    	if(maxY < pol.arr[i][1]){
    		maxY = pol.arr[i][1];
    	}
    	if(minY > pol.arr[i][1]){
    		minY = pol.arr[i][1];
    	}
    }

    pol.x_center = minX + (maxX - minX)/2;
    pol.y_center = minY + (maxY - minY)/2;

    return pol;
}

void draw_polygon(polygon pol, framebuffer f){
	int i,j,k,gd,gm,dy,dx;
    int x,y,temp;
    int xi[pol.size];
    float slope[pol.size];
   
    // Initiate Variables
    pol.arr[pol.size][0]=pol.arr[0][0];
    pol.arr[pol.size][1]=pol.arr[0][1];
  
    // Calculate slope
    for(i=0;i<pol.size;i++) {
        dy=pol.arr[i+1][1]-pol.arr[i][1];
        dx=pol.arr[i+1][0]-pol.arr[i][0];
        if(dy==0) slope[i]=1.0;
        if(dx==0) slope[i]=0.0;
        if((dy!=0)&&(dx!=0)) {
            slope[i]=(float) dx/dy;
        }
    }
  
    // Fill color
    for(y=0;y<pol.y_resolution;y++) {
        k=0;
        // Initiate xi
        for(i=0;i<pol.size;i++) {
            if (((pol.arr[i][1]<=y)&&(pol.arr[i+1][1]>y)) || ((pol.arr[i][1]>y)&&(pol.arr[i+1][1]<=y))) {
                xi[k]=(int)(pol.arr[i][0]+slope[i]*(y-pol.arr[i][1]));
                k++;
            }
        }
        // Sort xi
        for(j=0;j<k-1;j++)
            for(i=0;i<k-1;i++) {
                if(xi[i]>xi[i+1]) {
                    temp=xi[i];
                    xi[i]=xi[i+1];
                    xi[i+1]=temp;
                }
            }
        // Fill
        for(i=0;i<k;i+=2) {
        	point p1 = create_point(xi[i], y, pol.c1, pol.c2, pol.c3);
        	point p2 = create_point(xi[i+1]+1,y,pol.c1,pol.c2,pol.c3);
            
            line l = create_line(p1, p2);
            draw_line(l, f);
        }
    }
}

void set_color_polygon(polygon *pol, int c1, int c2, int c3){
	pol->c1 = c1;
	pol->c2 = c2;
	pol->c3 = c3;
}

int getY(float m, float c, int x, polygon frame) {
	float result = round((m * x) + c);
	if (result > frame.arr[0][1]) {
		result = frame.arr[0][1];
	} else if (result < frame.arr[2][1]) {
		result = frame.arr[2][1];
	}

	return (int)result;
}

int getX(float m, float c, int y, polygon frame) {
	float result = round(((float)y - (float)c)/(float)m);
	if (result < frame.arr[0][0]) {
		result = frame.arr[0][0];
	} else if (result > frame.arr[1][0]) {
		result = frame.arr[1][0];
	}

	return (int)result;
}

void add_point_to_polygon (polygon *res, int x, int y) {
	if (res->size == 0){
		res->arr[0][0] = x;
		res->arr[0][1] = y;
		res->size = res->size + 1;
	} else {
		if (res->arr[res->size-1][0] != x || res->arr[res->size-1][1] != y) {
			res->arr[res->size][0] = x;
			res->arr[res->size][1] = y;
			res->size = res->size + 1;
		} else {
		}
	}
}

void clip (polygon * res, int x1, int y1, int x2,int y2, polygon frame) {

	if (x1 == x2 || y1 == y2 || (y1 <= frame.arr[0][1] && y2 <= frame.arr[0][1]) || (y1 >= frame.arr[2][1] && y2 >= frame.arr[2][1]) || 
			(x1 <= frame.arr[0][0] && x2 <= frame.arr[0][0]) || (x1 >= frame.arr[1][0] && x2 >= frame.arr[1][0])) {

		if (x1 < frame.arr[0][0]) {
    		x1 = frame.arr[0][0];
		} else if (x1 > frame.arr[1][0]) {
			x1 = frame.arr[1][0];
		}

		if (x2 < frame.arr[0][0]) {
			x2 = frame.arr[0][0];
		} else if (x2 > frame.arr[1][0]) {
			x2 = frame.arr[1][0];
		}

		if (y1 < frame.arr[0][1]) {
			y1 = frame.arr[0][1];
		} else if (y1 > frame.arr[2][1]) {
			y1 = frame.arr[2][1];
		}

		if (y2 < frame.arr[0][1]) {
			y2 = frame.arr[0][1];
		} else if (y2 > frame.arr[2][1]) {
			y2 = frame.arr[2][1];
		}

	} else {

		y1 = -y1;
		y2 = -y2;
		frame.arr[2][1] = -frame.arr[2][1];
		frame.arr[0][1] = -frame.arr[0][1];

		float m = (float)(y2 - y1) / (float)(x2 - x1);
		float c = (float)y1 - (m * (float)x1);

		if (x1 < frame.arr[0][0] || x1 > frame.arr[1][0]) {
			if (x1 < frame.arr[0][0]) {
	    		x1 = frame.arr[0][0];
			} else if (x1 > frame.arr[1][0]) {
				x1 = frame.arr[1][0];
			}
			y1 = getY(m,c,x1,frame);
		}

		if (x2 < frame.arr[0][0] || x2 > frame.arr[1][0]) {
			if (x2 < frame.arr[0][0]) {
				x2 = frame.arr[0][0];
			} else if (x2 > frame.arr[1][0]) {
				x2 = frame.arr[1][0];
			}
			y2 = getY(m,c,x2,frame);
		}

		if (y1 > frame.arr[0][1] || y1 < frame.arr[2][1]) {
			if (y1 > frame.arr[0][1]) {
				y1 = frame.arr[0][1];
			} else if (y1 < frame.arr[2][1]) {
				y1 = frame.arr[2][1];
			}
			x1 = getX(m,c,y1,frame);
		}

		if (y2 > frame.arr[0][1] || y2 < frame.arr[2][1]) {
			if (y2 > frame.arr[0][1]) {
				y2 = frame.arr[0][1];
			} else if (y2 < frame.arr[2][1]) {
				y2 = frame.arr[2][1];
			}
			x2 = getX(m,c,y2,frame);
		}

		y1 = -y1;
		y2 = -y2;
		frame.arr[2][1] = -frame.arr[2][1];
		frame.arr[0][1] = -frame.arr[0][1];

	}

	add_point_to_polygon(res, x1, y1);
	add_point_to_polygon(res, x2, y2);

}

polygon dilate(polygon pol, polygon frame, int scale, int x_center, int y_center){

	pol.arr[pol.size][0]=pol.arr[0][0];
    pol.arr[pol.size][1]=pol.arr[0][1];

	int x1, x2, y1, y2, xmin, xmax, ymin, ymax, pidx, id;
	polygon res;	

	res.c1 = pol.c1;
	res.c2 = pol.c2;
	res.c3 = pol.c3;
	res.size = pol.size * 2 + 2;
	res.x_resolution = pol.x_resolution;
	res.y_resolution = pol.y_resolution;
	res.x_center = pol.x_center;
	res.y_center = pol.y_center;

	allocate_memory(&res, res.size);
	res.size = 0;

    for (int i = 0; i < (pol.size); i++) {

        x1 = pol.arr[i][0] * scale - (scale-1) * x_center;
        y1 = pol.arr[i][1] * scale - (scale-1) * y_center;
        x2 = pol.arr[i+1][0] * scale - (scale-1) * x_center;
        y2 = pol.arr[i+1][1] * scale - (scale-1) * y_center;

        clip(&res, x1, y1, x2, y2, frame);

    }

    return res;
}

polygon translate(polygon pol, polygon frame, int x_distance, int y_distance){
    pol.arr[pol.size][0]=pol.arr[0][0];
    pol.arr[pol.size][1]=pol.arr[0][1];

    int x1, y1, x2, y2;
	polygon res;

    res.c1 = pol.c1;
	res.c2 = pol.c2;
	res.c3 = pol.c3;
	res.size = pol.size * 2 + 2;
	res.x_resolution = pol.x_resolution;
	res.y_resolution = pol.y_resolution;
	res.x_center = pol.x_center + x_distance;
	res.y_center = pol.y_center + y_distance;

	allocate_memory(&res, res.size);
	res.size = 0;

	for(int i=0; i<(pol.size); i++){

		x1 = pol.arr[i][0] + x_distance;
        y1 = pol.arr[i][1] + y_distance;
        x2 = pol.arr[i+1][0] + x_distance;
        y2 = pol.arr[i+1][1] + y_distance;

		clip(&res, x1, y1, x2, y2, frame);

	}

	return res;
}

polygon rotate(polygon pol, int x_center, int y_center) {
    polygon res;
    allocate_memory(&res, pol.size);

    res.c1 = pol.c1;
	res.c2 = pol.c2;
	res.c3 = pol.c3;
	res.size = pol.size;
	res.x_resolution = pol.x_resolution;
	res.y_resolution = pol.y_resolution;
	res.x_center = pol.x_center;
	res.y_center = pol.y_center;

    for (int i = 0; i < res.size; i++) {
        res.arr[i][0] = (int)((float)(pol.arr[i][0]-x_center)*sqrt(2.0)/2.0 - (float)(pol.arr[i][1]-y_center)*sqrt(2.0)/2.0) + x_center;
     	res.arr[i][1] = (int)((float)(pol.arr[i][0]-x_center)*sqrt(2.0)/2.0 + (float)(pol.arr[i][1]-y_center)*sqrt(2.0)/2.0) + y_center;
    }

    return res;
}