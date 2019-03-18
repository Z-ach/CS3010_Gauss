#include <stdio.h>
#include <math.h>
#include <math.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//Incorrect arguments were passed in, print proper usage and quit
void usage(char *name){
	printf("usage: %s [--spp] file.lin\n", name);
	exit(1);
}

//Check to ensure that the file extension is proper, if not quit
void check(char *filename){
	int length;
	for(length = 0; filename[length] != '\0'; filename++);
	char file_ext[5];
	memcpy(file_ext, &filename[length - 4], 4);
	file_ext[4] = '\0';
	if(strcmp(file_ext, ".lin") != 0){
		printf("Invalid file extension on input file. Expecting .lin\n");
		exit(1);
	}
}

//Parse the cmdline args passed in, set spp flag if needed. Return the filename
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
	while(argv[arg_index][index] != '\0'){
		index++;
	}
	check(argv[arg_index]);
	return argv[arg_index];
}

//Write the solution to a file
void write_to_file(float *solution, int size, char *filename){
	int filename_length;
	for(filename_length = 0; filename[filename_length] != '\0'; filename_length++);
	char file_out_name[filename_length + 1];
	strncpy(file_out_name, filename, filename_length - 3);
	strcat(file_out_name, "sol");
	printf("Printing to %s\n", file_out_name);
	FILE *file = fopen(file_out_name, "w");
	for(int i = 0; i < size; i++){
		fprintf(file, "%f ", solution[i]);
	}fprintf(file, "\n");
	fclose(file);
}

//Parse the input file, return a size + 1 x size 2d array
float **parse_file(char *filename, int *size){
	FILE *file_pointer = fopen(filename, "r");
	if(file_pointer == NULL) return NULL;
	const int BUFFSIZE = 100;
	char buffer[BUFFSIZE];
	fgets(buffer, BUFFSIZE, file_pointer);
	int mat_size = atoi(buffer);
	float **coeff = malloc((mat_size+1) * sizeof(float*));
	for(int i = 0; i < mat_size + 1; i++){
		coeff[i] = calloc(mat_size, sizeof(float));
	}
	int character = 0;
	char number_buffer[50];
	memset(number_buffer, '\0', 50);
	for(int i = 0; i < mat_size + 1; i++){
		int numb_buff_index = 0;
		int numb_index = 0;
		int last_char_space = 0;
		character = fgetc(file_pointer);
		do{
			if(last_char_space == 1 && character == ' ') continue;
			if((character == ' ' || character == '\n') && number_buffer[0] != '\0'){
				coeff[i][numb_index++] = atof(number_buffer);
				memset(number_buffer, '\0', 50);
				numb_buff_index = 0;
				last_char_space = 1;
			}
			else{
				number_buffer[numb_buff_index++] = character;
				last_char_space = 0;
			}
		}while((character = fgetc(file_pointer)) != '\n');
		coeff[i][numb_index] = atof(number_buffer);
		memset(number_buffer, '\0', 50);
	}
	fclose(file_pointer);
	*size = mat_size;
	return coeff;
}

//Print the matrix, used for debugging purposes
void print_matrix(float **coeff, float *consts, int size){
	for(int i = 0; i < size; i++){
		printf("|");
		for(int j = 0; j < size; j++){
			printf("%15.5f ", coeff[i][j]);
		}
		printf("|\t|%15.5f|\n", consts[i]);
	}
	printf("\n");
}

//Naive Gaussian Elimination Algorithm
void naive_gauss_elim(float **coeff, float *constants, int size){
	for(int i = 0; i < size - 1; i++){ //pivot
		for(int j = i + 1; j < size; j++){ //current equation
			float scale = - (coeff[j][i] / coeff[i][i]); 
			for(int k = i; k < size; k++){ //current coefficient
				coeff[j][k] = scale * coeff[i][k] + coeff[j][k];
			}
			constants[j] = scale * constants[i] + constants[j];
		}
	}
}

//Generic back substitution algorithm, requires order of execution
float *back_sub(float **coeff, float *constants, int size, int *order){
	float *solution = malloc(size * sizeof(float));
	solution[size - 1] = constants[order[size - 1]] / coeff[order[size - 1]][size - 1];
	for(int i = size - 2; i >= 0; i--){
		float sum = 0;
		int curr_eq = order[i];
		for(int j = i + 1; j < size; j++){
			sum += solution[j] * coeff[curr_eq][j];
		}
		solution[i] = (constants[curr_eq] - sum) / coeff[curr_eq][i];
	}
	return solution;
}

//Simple swap procedure
void swap(int *one, int *two){
    int temp = *one;
    *one = *two;
    *two = temp;
}

int max(float *array, int size){
    int index = 0;
    int max = array[0];
    for(int i = 0; i < size; i++){
        if(array[i] > max) {
            max = array[i];
            index = i;
        }
    }
    return index;
}

int *partial_pivot_gauss(float **coeff, float *constants, int size){
    float scale[size];
    int *order = malloc(size * sizeof(int));
    int used[size];
    for(int i = 0; i < size; i++){
        scale[i] = coeff[i][max(coeff[i], size)];
        order[i] = i;
        used[i] = 0;
    }
    for(int i = 0; i < size; i++){
        float pivot_picker[size];
        for(int j = 0; j < size; j++){
            pivot_picker[j] = fabs(coeff[j][i]/scale[j]);
            if(used[j] == 1) pivot_picker[j] = -1;
        }
        int piv_index = max(pivot_picker, size);
        int spot = 0;
        for(int ind = 0; ind < size; ind++){
			if(order[ind] == piv_index){
				spot = ind;
				break;
			}
        }
		swap(&order[spot], &order[i]);

		used[piv_index] = 1;

        for(int j = 0; j < size; j++){
            if(used[j] == 1) continue;
            float scale_factor = - coeff[j][i] / coeff[piv_index][i];
            for(int k = 0; k < size; k++){
                coeff[j][k] = coeff[j][k] + (scale_factor * coeff[piv_index][k]);
            }
            constants[j] = constants[j] + (scale_factor * constants[piv_index]);
        }
    }
    return order;
}


int main(int argc, char *argv[]){
	int spp = 0;
	char *filename = parse_args(argc, argv, &spp);
	int size = 0;
	float **coeff = parse_file(filename, &size);
	float *constants = malloc(size * sizeof(float));
	for(int i = 0; i < size; i++){
		constants[i] = coeff[size][i];
	}

    int *order = NULL;

    if(spp == 0) naive_gauss_elim(coeff, constants, size);
    else order = partial_pivot_gauss(coeff, constants, size);
	
	if(!order){
	    order = malloc(size * sizeof(int));
	    for(int i = 0; i < size; i++){
            order[i] = i;
        }
    }

	float *solution = back_sub(coeff, constants, size, order);
	
	write_to_file(solution, size, filename);

	for(int i = 0; i < size; i++){
	    free(coeff[i]);
    }
    free(coeff);
    free(constants);
}
