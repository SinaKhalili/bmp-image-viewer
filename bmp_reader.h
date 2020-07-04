#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

typedef struct bmp_file_t {
  FILE *fd;
  uint8_t error;
  uint8_t signature[2];
  uint32_t size;
  uint8_t reserved[4];
  uint32_t data_offset;
  uint32_t info_size; // 4
  uint32_t width; // 4
  uint32_t height; // 4
  uint16_t planes; // 2
  uint16_t bits_per_pixel;  // 2
  uint32_t compression; // 4
  uint32_t image_size; // 4
  uint32_t x_pixels_per_m; // 4
  uint32_t y_pixels_per_m; // 4
  uint32_t colours_used; // 4
  uint32_t important_colours; // 4
  uint32_t num_pixels; // 4
  uint8_t* pixel_data; /* 24 bit only supported */
} bmp_file_t;

bmp_file_t parse_bmp_file(char* filename);
