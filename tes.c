#include <stdio.h>

void print(int arr[][2], int n){
	for(int i=0; i<n; i++){
		for(int j=0; j<2; j++){
			printf("%d ", arr[i][j]);
		}
		printf("\n");
	}
}

int main(){
	int arr[][2];

	for(int i=0; i<5; i++){
		arr[i][0] = 10;
		arr[i][1] = 1;
	}

	print(arr, 5);
}