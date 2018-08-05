#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "function.h"
#include "thread_data.h"

extern int errno;
pthread_barrier_t barrier;
pthread_mutex_t mutex_amount;
pthread_mutex_t mutex_father;
int amount_px_black = 0;
void *image;
bmp_image *image2;

/*
Funcion que abre abre el archivo con el nombre indicado en el parametro de entrada.
entrada: string que indica el nombre del archivo a leer.
salida: puntero al archivo a leer.
*/
FILE *open_bmp_file(char *bmp_file_name){
	FILE *file;
	if(!(file = fopen(bmp_file_name,"rb"))){
		fprintf(stderr,"Error opening the file: %s\n",strerror(errno));
		return NULL;
	}
	else{
		return file;
	}
}

int get_padding_size(int width){
	int padding = 0;
	int pitch = width*3;
	if(pitch % 4 != 0) pitch+= 4 - (pitch % 4);
	padding = pitch - (3*width);
	return padding;
}

/*
Funcion que lee el archivo bmp dado con el nombre dado como parametro.
entrada: string que indica el nombre del archivo
salida: puntero a un struct que representa la imagen leida.
*/

void *read_bmp_file(void *n_bmp_file){
	FILE *bmp_file;
	bmp_image *image;
	bmp_header header;
    char *n_bmp_file2 = (char *)n_bmp_file;
	int i,j;
	bmp_file = open_bmp_file(n_bmp_file2);
	if(bmp_file){
		image = (bmp_image*)malloc(sizeof(bmp_image));
		fread(&header, sizeof(bmp_header), 1, bmp_file);
		image->header = header;
		image->pixel_array = malloc(image->header.height*sizeof(Pixel*));
		for(i= 0; i < image->header.height; i++)
			image->pixel_array[i] = malloc(image->header.width*sizeof(Pixel));
		fseek(bmp_file,header.offset,SEEK_SET);
		switch(image->header.bits_per_pixel){
			case 24:
				if(image->header.width % 4 != 0){
					int padding = get_padding_size(header.width);
					for(i = 0; i < image->header.height; i++){
						for(j = 0; j < image->header.width; j++){
							fread(&image->pixel_array[i][j].b, sizeof(char), 1, bmp_file);
							fread(&image->pixel_array[i][j].g, sizeof(char), 1, bmp_file);
							fread(&image->pixel_array[i][j].r, sizeof(char), 1, bmp_file);
						}
						fread(&image->pixel_array[i][j].p,sizeof(char),padding,bmp_file);
					}
				}
	
				else{
					for(i = 0; i < image->header.height; i++){
						for(j = 0; j < image->header.width; j++){
							fread(&image->pixel_array[i][j].b, sizeof(char), 1, bmp_file);
							fread(&image->pixel_array[i][j].g, sizeof(char), 1, bmp_file);
							fread(&image->pixel_array[i][j].r, sizeof(char), 1, bmp_file);
						}
					}
				}
				break;

			case 32:
				for(i = 0; i < image->header.height; i++){
					for(j = 0; j < image->header.width; j++){
						fread(&image->pixel_array[i][j].b, sizeof(char), 1, bmp_file);
						fread(&image->pixel_array[i][j].g, sizeof(char), 1, bmp_file);
						fread(&image->pixel_array[i][j].r, sizeof(char), 1, bmp_file);
						fread(&image->pixel_array[i][j].p, sizeof(char), 1, bmp_file);
					}
				}
				break;
		}
		fclose(bmp_file);
		pthread_barrier_wait(&barrier);
		return (void *)image;
	}
	else return NULL;	
}

/*
Funcion que convierte la imagen indicada como parametro a escala de grises.
entrada: puntero a un struct que indica la imagen a convertir.
*/

void *to_gray_scale(void *tdata){
	thread_data *thread = (thread_data *)tdata;
    int r2 = thread->row;
	int amount = thread->threads;
    int column;
	int row;
    for(row = r2; row < image2->header.height; row = row + amount){
		for(column = 0; column < image2->header.width; column++){
			int gray = image2->pixel_array[row][column].r*0.3 + image2->pixel_array[row][column].g*0.59 + image2->pixel_array[row][column].b*0.11;
            image2->pixel_array[row][column].r = gray;
            image2->pixel_array[row][column].g = gray;
            image2->pixel_array[row][column].b = gray; 
        }
	}
    pthread_barrier_wait(&barrier);
	return NULL;
}

/*
Funcion que transforma la imagen en escala de grises dada como parametro y segun un umbral, binariza la imagen
entrada: puntero a un struct que representa la imagen en escala de grises y un entero que indica el umbral 
*/

void *to_binary_image(void  *tdata){
	//debe recibir struct
	thread_data *data = (thread_data *)tdata;
	int row,column;
	int r2 = data->row; 
	int threshold = data->threshold; 
	int amount = data->threads;
	for(row = r2; row < image2->header.height; row = row + amount){
		for(column = 0; column < image2->header.width; column++){
			if(image2->pixel_array[row][column].b > threshold){
				image2->pixel_array[row][column].b = 255;
				image2->pixel_array[row][column].g = 255;
				image2->pixel_array[row][column].r = 255;
			}
			else{
				image2->pixel_array[row][column].b = 0;
				image2->pixel_array[row][column].g = 0;
				image2->pixel_array[row][column].r = 0;
			} 
		}
	}
	pthread_barrier_wait(&barrier);
	return NULL;
}

void *sort_out_image_binary(void *tdata){
	thread_data *data = (thread_data *)tdata;
	int row,column;
	int r2 = data->row;
	int cat = data->categorization;
	int amount = data->threads;

	for(row = r2; row < image2->header.height; row = row + amount){
		for(column = 0; column < image2->header.width; column++){
			if(image2->pixel_array[row][column].b == 0) {
				pthread_mutex_lock(&mutex_amount);
				amount_px_black++;
				pthread_mutex_unlock(&mutex_amount);
			}
		}
	}
	
}

void print_result(thread_data *tdata, char * name){
	int row,column;
	int r2 = tdata->row;
	int cat = tdata->categorization;
	int amount = tdata->threads;
	
	if(((amount_px_black*100)/((image2->header.height*image2->header.width))) >= cat){
		amount_px_black = 0;
		printf("|   %-5s   |   yes            |\n", name);
	}
	else{
		amount_px_black = 0;
		printf("|   %-5s   |   no             |\n", name);
	}
}

void write_bmp_file(bmp_image *image, char *bmp_file){
	int i,j;
	int padding;
	int is_nearly_black = 0;
	char aux1[30] = "res_";
	strcat(aux1,bmp_file);
	FILE *aux = fopen(aux1,"wb");
	fwrite(&image->header,sizeof(bmp_header),1,aux);
	fseek(aux,image->header.offset,SEEK_SET);
	switch(image->header.bits_per_pixel){
		case 24:
			if(image->header.width % 4 != 0){
				int padding = get_padding_size(image->header.width); 
				for(i = 0; i < image->header.height; i++){
					for(j = 0; j < image->header.width; j++){
						fwrite(&image->pixel_array[i][j].b,sizeof(char),1,aux);
						fwrite(&image->pixel_array[i][j].g,sizeof(char),1,aux);
						fwrite(&image->pixel_array[i][j].r,sizeof(char),1,aux);
					}
					fwrite(&image->pixel_array[i][j].p,sizeof(char),padding,aux);
				}
			}
			else{
				for(i = 0; i < image->header.height; i++){
					for(j  = 0; j < image->header.width; j++){
						fwrite(&image->pixel_array[i][j].b,sizeof(char),1,aux);
						fwrite(&image->pixel_array[i][j].g,sizeof(char),1,aux);
						fwrite(&image->pixel_array[i][j].r,sizeof(char),1,aux);
					}
				}
			}
			break;
		case 32:	
			for(i = 0; i < image->header.height; i++){
				for(j = 0; j < image->header.width; j++){
					fwrite(&image->pixel_array[i][j].b,sizeof(char),1,aux);
					fwrite(&image->pixel_array[i][j].g,sizeof(char),1,aux);
					fwrite(&image->pixel_array[i][j].r,sizeof(char),1,aux);
					fwrite(&image->pixel_array[i][j].p,sizeof(char),1,aux);
				}
			}
			break;
		
		fclose(aux);
	}
}




void execute_task(int amount_images, int amount_threads, pthread_t *threads, int threshold, int cat, int flag ){
    int cnt = 1;
	int status = 0;
    while(cnt <= amount_images){
		char nfile[20] = "imagen_";
		char format[] = ".bmp";
		char n[2];
		
		sprintf(n,"%d",cnt);
		strcat(nfile,n);
		strcat(nfile,format);

        int t;
		
		thread_data *data;
		data = malloc(sizeof(thread_data)*amount_threads);
		for(t = 0; t < amount_threads; t++){
			data[t].row = t;
			data[t].threshold = threshold;
			data[t].categorization = cat;
			data[t].threads = amount_threads;
		}
		
		/* first thread to read_bmp_file */
		if(pthread_barrier_init(&barrier, NULL, 1))
			printf("Could not create a barrier to read_bmp_file");
        pthread_create(&threads[0], NULL, read_bmp_file, (void *) &nfile);
        pthread_barrier_destroy(&barrier);
		pthread_join(threads[0],&image);
		/* primera hebra creada lee imagen */
		
		/* barrier to_gray_scale */
		 if(pthread_barrier_init(&barrier, NULL, amount_threads))
			printf("Could not create a barrier to_gray_scale\n");
		image2 = (bmp_image *)image;
        for(t = 0; t < amount_threads; t++)
            pthread_create(&threads[t], NULL, &to_gray_scale, (void *)&data[t]);
		for(t = 0; t < amount_threads; t++)
			pthread_join(threads[t],&image);
        pthread_barrier_destroy(&barrier);
		/* finished to_gray_scale */

		/*barrier to binarized_image */
		if(pthread_barrier_init(&barrier,NULL,amount_threads))
			printf("Could not create barrier to binarized image\n");
		for(t = 0; t < amount_threads; t++)
			pthread_create(&threads[t], NULL, &to_binary_image, (void* )&data[t]);
		for(t = 0; t < amount_threads; t++)
			pthread_join(threads[t], &image);
		pthread_barrier_destroy(&barrier);
		/*finished to binarized_image */

		/* start mutex to get if the image is nearly black */
		for(t = 0; t < amount_threads; t++){
			if(pthread_mutex_init(&mutex_amount, NULL))
				printf("Could not create a mutex to last stage");
			for(t = 0; t < amount_threads; t++)
				pthread_create(&threads[t], NULL, &sort_out_image_binary, (void*)&data[t]);
			for(t = 0; t < amount_threads; t++)
				pthread_join(threads[t], &image);
		}
		/* end mutex to get if the image is nearly black */

		print_result(data, nfile);
		write_bmp_file(image2, nfile);
		cnt+=1;
		
	}
	

}