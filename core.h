//
// Created by trist on 10/09/2025.
//

#ifndef CORE_H
#define CORE_H
#include <stdbool.h>

#define MAX_CELLS 1000

void RGB2gray(unsigned char color_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
void gray2BW(unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int threshold);
int erosion(unsigned char work_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int style, int target_channel);
int detection(unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int cells_center[MAX_CELLS][2], int nb_cells, int channel,  int ws, int split, int n_min, int n_max);
void generate_image(unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char labelled_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int cells_center[MAX_CELLS][2], int nb_cells);
int main_algorithm(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int cells_center[MAX_CELLS][2], int threshold);
unsigned char otsu_method(unsigned char grey_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);


#endif //CORE_H

