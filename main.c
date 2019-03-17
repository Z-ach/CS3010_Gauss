#include <stdio.h>
#include <math.h>
#include <math.h>
#include <math.h>
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
			//printf("running... buff currently %s, %d\n", number_buffer, last_char_space);
			if(last_char_space == 1 && character == ' ') continue;
			if(character == ' ' && number_buffer[0] != '\0'){
				//printf("buffer was %s\n", number_buffer);
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
	}
	fclose(file_pointer);
	*size = mat_size;
	return coeff;
}

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


void naive_gauss_elim(float **coeff, float *constants, int size){
	for(int i = 0; i < size - 1; i++){ //pivot
		for(int j = i + 1; j < size; j++){ //current equation
			float scale = - (coeff[j][i] / coeff[i][i]); 
			for(int k = i; k < size; k++){ //current coefficient
				coeff[j][k] = scale * coeff[i][k] + coeff[j][k];
			}
			constants[j] = scale * constants[i] + constants[j];
			//print_matrix(coeff, constants, size);
		}
	}
}


float *back_sub(float **coeff, float *constants, int size, int *order){
	float *solution = malloc(size * sizeof(float));
	solution[size - 1] = constants[order[size - 1]] / coeff[order[size - 1]][size - 1];
	printf("sol = %f\n", solution[size-1]);
	for(int i = size - 2; i >= 0; i--){
		float sum = 0;
		int curr_eq = order[i];
		printf("working on eq %d, sol will go into %d\n", curr_eq, i);
		for(int j = i + 1; j < size; j++){
			sum += solution[j] * coeff[curr_eq][j];
		}
		solution[i] = (constants[curr_eq] - sum) / coeff[curr_eq][i];
		printf("(%f - %f) / %f = %f\n", constants[curr_eq], sum, coeff[curr_eq][i], solution[i]); 
		printf("sol to %d is %f\n", i, solution[i]);
	}
	for(int i = 0; i < size; i++){
		//printf("%d ", order[i]);
		printf("%f ", solution[i]);
	}printf("\n");
	return solution;
}


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
        printf("max was selected as %d, or %d\n", piv_index, order[piv_index]);
        //if(!used[piv_index]) swap(&order[piv_index], &order[i]);
        //if(!used[piv_index]) printf("swapped %d with %d\n", order[i], order[piv_index]);
        int spot = 0;
        for(int ind = 0; ind < size; ind++){
			if(order[ind] == piv_index){
				spot = ind;
				break;
			}
        }
        printf("spot was %d\n", spot);
		swap(&order[spot], &order[i]);
		printf("swapped %d with %d\n", order[i], order[spot]);
		for(int i = 0; i < size; i++){
			printf("%d ", order[i]);
		}printf("\n");

		used[piv_index] = 1;

        for(int j = 0; j < size; j++){
            if(used[j] == 1) continue;
            float scale_factor = - coeff[j][i] / coeff[piv_index][i];
            for(int k = 0; k < size; k++){
                coeff[j][k] = coeff[j][k] + (scale_factor * coeff[piv_index][k]);
            }
            constants[j] = constants[j] + (scale_factor * constants[piv_index]);
            //print_matrix(coeff, constants, size);
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
	print_matrix(coeff, constants, size);

    int *order = NULL;

    if(spp == 0) naive_gauss_elim(coeff, constants, size);
    else order = partial_pivot_gauss(coeff, constants, size);
	
	if(!order){
	    order = malloc(size * sizeof(int));
	    for(int i = 0; i < size; i++){
            order[i] = i;
        }
    }

	print_matrix(coeff, constants, size);	

	float *solution = back_sub(coeff, constants, size, order);

	for(int i = 0; i < size; i++){
		printf("%f ", solution[i]);
	}
	printf("\n");	

	for(int i = 0; i < size; i++){
	    free(coeff[i]);
    }
    free(coeff);
    free(constants);
}
