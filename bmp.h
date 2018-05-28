typedef struct __attribute__((packed)){
	uint16_t type;
	uint32_t size;
	uint16_t resv1;
	uint16_t resv2;
	uint32_t offset;
	uint32_t header_size;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compress;
	uint32_t img_size;
	int32_t resolution_bmp_x;
	int32_t resolution_bmp_y;
	uint32_t color_image;
	uint32_t important_colors;	
}bmp_header;

typedef struct __attribute__((packed)){
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char p;
}Pixel;

typedef struct {
	bmp_header header;
	Pixel **pixel_array;
}bmp_image;