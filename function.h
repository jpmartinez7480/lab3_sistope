FILE *open_bmp_file(char *bmp_file_name);
int get_padding_size(int width);
void *read_bmp_file(void *n_bmp_file);
void *to_gray_scale(void *tdata);
void *to_binary_image(void *tdata);
void *sort_out_image_binary(void *tdata);
void print_result(thread_data *tdata, char *name);
void write_bmp_file(bmp_image *image, char *bmp_file);
void execute_task(int amount_images, int amount_threads, pthread_t *threads, int threshold, int cat, int flag);