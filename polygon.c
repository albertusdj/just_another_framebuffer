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

polygon create_polygon_from_file(char *filename, int c1, int c2, int c3, int x_resolution, int y_resolution){
	polygon pol;
	pol.c1 = c1;
	pol.c2 = c2;
	pol.c3 = c3;
	pol.x_resolution = x_resolution;
	pol.y_resolution = y_resolution;

	FILE *fp;  
    
    fp = fopen(filename, "r");
    fscanf(fp, "%d", &pol.size);

    pol.arr = (int **)malloc((pol.size+1) * sizeof(int *));
    
    for (int i=0; i<(pol.size+1); i++){
         pol.arr[i] = (int *)malloc(2 * sizeof(int));
    }

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