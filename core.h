//
// Created by trist on 10/09/2025.
//

#ifndef CORE_H
#define CORE_H
#include <stdbool.h>

#define MAX_CELLS 1000

void RGB2gray(unsigned char color_image[950][950][3], unsigned char gray_image[950][950][3]);
void gray2BW(unsigned char gray_image[950][950][3], int threshold);
int erosion(unsigned char (*src)[BMP_HEIGTH][BMP_CHANNELS], unsigned char (*dst)[BMP_HEIGTH][BMP_CHANNELS], int style);
int detection(unsigned char image[950][950][3], int cells_center[MAX_CELLS][2], int nb_cells, int ws, int split, int n_min, int n_max);
void generate_image(unsigned char image[950][950][3], unsigned char labelled_image[950][950][3], int cells_center[MAX_CELLS][2], int nb_cells);
int main_algorithm(unsigned char input_image[950][950][3], unsigned char output_image[950][950][3], int cells_center[MAX_CELLS][2], int nb_cells, int threshold);
unsigned char otsu_method(unsigned char grey_image[950][950][3]);


#endif //CORE_H

