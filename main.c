//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"
#include "core.h"
#include <windows.h>

//Setup for better time detection
LARGE_INTEGER start, end, freq;
double elapsed_ms;

//Function to invert pixels of an image (negative)
void invert(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      for (int c = 0; c < BMP_CHANNELS; c++)
      {
      output_image[x][y][c] = 255 - input_image[x][y][c];
      }
    }
  }
}

  //Declaring the array to store the image (unsigned char = unsigned 8 bit)
  unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
  unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

//Main function
int main(int argc, char** argv)
{
  //argc counts how may arguments are passed
  //argv[0] is a string with the name of the program
  //argv[1] is the first command line argument (input image)
  //argv[2] is the second command line argument (output image)


  //Checking that 2 arguments are passed
  if (argc != 3)
  {
      fprintf(stderr, "Usage: %s <input file path> <output file path>\n", argv[0]);
      exit(1);
  }

  printf("Example program - 02132 - A1\n");

  //Load image from file
  read_bitmap(argv[1], input_image);

  int nb_cells = 0;
  int cells_center[MAX_CELLS][2];

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&start);

  // Otsu threshold calculation
  unsigned char threshold =  otsu_method(input_image);

  QueryPerformanceCounter(&end);
  elapsed_ms = (end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
  printf("Total time for Otsu: %.3f ms\n", elapsed_ms);
  printf("Otsu Threshold is: %d\n", threshold);

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&start);

  // Main Detection algorithm (by default the threshold is : Otsu's threshold - 10)
  nb_cells = main_algorithm(input_image, output_image, cells_center, threshold-10);

  QueryPerformanceCounter(&end);

  // Exec time for the main algorithm
  elapsed_ms = (end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
  printf("Total time for main algo: %.3f ms\n", elapsed_ms);

  // Print the result data
  printf("Number of cells: %d\n", nb_cells);

  //Save image to file
  write_bitmap(output_image, argv[2]);

  printf("Done!\n");
  return 0;
}
