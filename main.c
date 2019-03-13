#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(char *name){
	printf("usage: %s [--spp] file.sln\n", name);
	exit(1);
}

char *parse_args(int argc, char *argv[], int *spp){
	int index = 0;
	int arg_index = 1;
	if(argc < 2){
		usage(argv[0]);
	}
	if(argc == 3 && strcmp(argv[1], "--spp") == 0){
		*spp = 1;
		arg_index++;
	}
	printf("checking argv[%d]\n", arg_index);
	while(argv[arg_index][index] != '\0'){
		index++;
	}
	return argv[arg_index];
}

void allocate_array(int **coeff, int size){
	coeff = malloc(size * sizeof(int));
	for(int i = 0; i < size; i++){
		coeff[i] = malloc(size * sizeof(int));
	}
	printf("testing insertion...\n");
	coeff[1][1] = 0;
	memset(coeff[1], 1, size);
	printf("insertion complete. value is %d\n", coeff[1][1]);
}

int parse_file(char *filename, float *coeff[], float  *constants){
	FILE *file_pointer = fopen(filename, "r");
	if(file_pointer == NULL) return -1;
	const int BUFFSIZE = 100;
	char buffer[BUFFSIZE];
	fgets(buffer, BUFFSIZE, file_pointer);
	int mat_size = atoi(buffer);
	coeff = malloc(mat_size * sizeof(float));
	for(int i = 0; i < mat_size; i++){
		coeff[i] = calloc(mat_size, sizeof(float));
	}
	constants = malloc(mat_size * sizeof(float));
	int character = 0;
	char number_buffer[50];
	memset(number_buffer, '\0', 50);
	coeff[2][0] = 50;
	for(int i = 0; i < mat_size + 1; i++){
		printf("%d\n", coeff);
		printf("%d\n", coeff[0]);
		coeff[1][0] = 100;
		int numb_buff_index = 0;
		int numb_index = 0;
		while((character = fgetc(file_pointer)) != '\n'){
			if(character == ' '){
				printf("%f, [%d][%d] = ", atof(number_buffer), i, numb_index);
				if(i < mat_size) coeff[i][numb_index++] = atof(number_buffer);
				else constants[numb_index++] = atof(number_buffer);
				printf("%f\n", coeff[i][numb_index - 1]);
				memset(number_buffer, '\0', 50);
				numb_buff_index = 0;
			}
			else{
				number_buffer[numb_buff_index++] = character;
				printf("buffer: %s\n", number_buffer);
			}
		}
	}
	fclose(file_pointer);
	return mat_size;
}

int main(int argc, char *argv[]){
	int spp = 0;
	char *filename = parse_args(argc, argv, &spp);
	float **coeff;
	float  *constants;
	int size = parse_file(filename, coeff, constants);
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			printf("%f ", coeff[i][j]);
		}
		printf("%f\n", constants[i]);
	}
}