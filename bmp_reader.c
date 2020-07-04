#include"bmp_reader.h"

int compare_bytes(uint8_t *s1, uint8_t *s2, size_t size) {
  for (int i = 0; i < size; i++) {
    if (s1[i] != s2[i])
      return 0;
  }
  return 1;
}

bmp_file_t parse_bmp_file(char* filename){
  bmp_file_t bmp_file;
  bmp_file.error = 1; /*  Sets error, if parse makes it to the end, set error to 0 */
  FILE *fd = fopen(filename, "r");
  if(!fd){
    printf("Error: Invalid file\n");
    return bmp_file;
  }
  bmp_file.fd = fd;

  /*  Begin reading the file  */
  size_t res = fread(bmp_file.signature, 2, 1, bmp_file.fd);
  if (!res)
    return bmp_file;

  /*  Check the magic number */
  uint8_t signature[2] = {'B', 'M'};
  if (!compare_bytes(signature, bmp_file.signature, 2)){
    printf("Error: Invalid file type\n");
    return bmp_file;
  }

  /*  Get the size of the file - 32 bits */
  res = fread(&bmp_file.size, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;

  printf("File size is %d bytes\n", bmp_file.size);

  /*  Get the header type of the file */
  res = fread(bmp_file.reserved, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;

  /*  Get the data offset */
  res = fread(&bmp_file.data_offset, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;

  printf("Data offset is %d bytes\n", bmp_file.data_offset);

  /*  Beginning of the data format information */
  res = fread(&bmp_file.info_size, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;

  /*  Beginning of the data format information */
  res = fread(&bmp_file.width, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;

  /*  Beginning of the data format information */
  res = fread(&bmp_file.height, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;

  printf("Width and height : %d by %d\n", bmp_file.width, bmp_file.height);

  res = fread(&bmp_file.planes, 2, 1, bmp_file.fd);
  if (!res)
    return bmp_file;
  printf("Planes: %d\n", bmp_file.planes);

  res = fread(&bmp_file.bits_per_pixel, 2, 1, bmp_file.fd);
  if (!res)
    return bmp_file;
  printf("Bits per pixel: %d \n", bmp_file.bits_per_pixel);

  res = fread(&bmp_file.compression, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;
  printf("Compression type: %d (0 is no compression) \n", bmp_file.compression);

  res = fread(&bmp_file.image_size, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;
  printf("Image size: %d \n", bmp_file.image_size);

  res = fread(&bmp_file.x_pixels_per_m, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;

  res = fread(&bmp_file.y_pixels_per_m, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;
  printf("x, y pixels per m : %d , %d \n",bmp_file.x_pixels_per_m,
                                          bmp_file.y_pixels_per_m);

  /*These next two values are not important*/
  res = fread(&bmp_file.colours_used, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;
  
  printf("Number of colors used: %d \n", bmp_file.colours_used); 
  res = fread(&bmp_file.important_colours, 4, 1, bmp_file.fd);
  if (!res)
    return bmp_file;
  
  fseek(bmp_file.fd, bmp_file.data_offset, SEEK_SET);

  bmp_file.num_pixels = bmp_file.width * bmp_file.height;
  
  /* We need to find the padding */
  uint8_t padding = 4 - ((bmp_file.width * 3) % 4);
  if(padding == 4)
      padding = 0;
  printf("Padding required: %d \n", padding);
  
  bmp_file.pixel_data = malloc(bmp_file.num_pixels * 3);

  for(int i = 0; i < bmp_file.height; i++){
    /* Read in a row */
    res = fread(bmp_file.pixel_data + i*(bmp_file.width*3), 1, (bmp_file.width * 3), bmp_file.fd);
    
    /* Now seek forward the padding amount */
    fseek(bmp_file.fd, padding, SEEK_CUR);
  }
  if (!res)
    return bmp_file;
  
  if(feof(bmp_file.fd))
    printf("We've reached the end of the file\n");
  
  bmp_file.error = 0; /* Successful parse */
  return bmp_file;
}
