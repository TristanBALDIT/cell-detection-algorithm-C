#include "cbmp.h"
#include "core.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// COMPARISON MACROS

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define SAFE_IDX(x) ( (x) < 0 ? 0 : ((x) > 949 ? 949 : (x)) )

// EROSION MASKS

const int cross_mask[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
const int square_mask_3[8][2] = {{-1,0},{1,0},{0,-1},{0,1}, {-1,-1},{-1,1},{1,-1},{1,1}};
const int square_mask_5[24][2] = {{-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2},
                                {-1, -2}, {-1, -1}, {-1, 0}, {-1, 1}, {-1, 2},
                                {0, -2}, {0, -1}, {0, 1}, {0, 2},
                                {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2},
                                {2,-2}, {2, -1}, {2, 0}, {2, 1}, {2, 2}};
const int square_mask_7[48][2] = {{-3,-3}, {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3},
                                {-2, -3}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-2, 3},
                                {-1, -3}, {-1, -2}, {-1, -1}, {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
                                {0, -3}, {0, -2}, {0, -1}, {0, 1}, {0, 2}, {0, 3},
                                {1, -3}, {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}, {1,3},
                                {2, -3}, {2, -2}, {2, -1}, {2, 0}, {2, 1}, {2, 2}, {2,3},
                                {3, -3}, {3, -2}, {3, -1}, {3, 0}, {3, 1}, {3, 2}, {3, 3}};
const int sphere_mask[36][2] = {{-3, -1}, {-3, 0}, {-3, 1},
                                {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2},
                                {-1, -3}, {-1, -2}, {-1, -1}, {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
                                {0, -3}, {0, -2}, {0, -1}, {0, 1}, {0, 2}, {0, 3},
                                {1, -3}, {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}, {1,3},
                                {2, -2}, {2, -1}, {2, 0}, {2, 1}, {2, 2},
                                {3, -1}, {3, 0}, {3, 1}};

const int smooth_square_mask_7[44][2] = { {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2},
                                {-2, -3}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-2, 3},
                                {-1, -3}, {-1, -2}, {-1, -1}, {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3},
                                {0, -3}, {0, -2}, {0, -1}, {0, 1}, {0, 2}, {0, 3},
                                {1, -3}, {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}, {1,3},
                                {2, -3}, {2, -2}, {2, -1}, {2, 0}, {2, 1}, {2, 2}, {2,3},
                                {3, -2}, {3, -1}, {3, 0}, {3, 1}, {3, 2}};
const int smooth_square_mask_5[20][2] = {{-2, -1}, {-2, 0}, {-2, 1},
                                {-1, -2}, {-1, -1}, {-1, 0}, {-1, 1}, {-1, 2},
                                {0, -2}, {0, -1}, {0, 1}, {0, 2},
                                {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2},
                                {2, -1}, {2, 0}, {2, 1}};

const int sphere_mask_9[68][2] = {{-4, -2}, {-4, -1}, {-4, 0}, {-4, 1}, {-4, 2},
                                {-3, -3}, {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3},
                                {-2, -4}, {-2, -3}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-2, 3}, {-2, 4},
                                {-1, -4}, {-1, -3}, {-1, -2}, {-1, -1}, {-1, 0}, {-1, 1}, {-1, 2}, {-1, 3}, {-1, 4},
                                {0, -4}, {0, -3}, {0, -2}, {0, -1}, {0, 1}, {0, 2}, {0, 3}, {0, 4},
                                {1, -4}, {1, -3}, {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4},
                                {2, -4}, {2, -3}, {2, -2}, {2, -1}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4},
                                {3, -3}, {3, -2}, {3, -1}, {3, 0}, {3, 1}, {3, 2}, {3, 3},
                                {4, -2}, {4, -1}, {4, 0}, {4, 1}, {4, 2}};

typedef struct {
    const int (*mask)[2];   // mask array
    int size;               // mask array size
} MaskStruct;

const MaskStruct mask_list[] = {
    { cross_mask, 4 },
    { square_mask_3, 8 },
    { square_mask_5, 24 },
    {square_mask_7, 48},
    {sphere_mask, 36},
    {smooth_square_mask_5, 20},
    {smooth_square_mask_7, 44},
    {sphere_mask_9, 68}
};


// FUNCTIONS

// Set the gray value in all RGB channels of a pixel
static inline void set_gray_value (unsigned char pixel[3], unsigned char gray_value) {
    pixel[0] =  pixel[1] = pixel[2] =  gray_value;
}


// Return 1 if all mask pixels around (i, j) are white, else 0
int erosion_test_mask(unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int i, int j, MaskStruct m) {
    for (int k = 0; k < m.size; k++) {
        int y = SAFE_IDX(i + m.mask[k][0]);
        int x = SAFE_IDX(j + m.mask[k][1]);
        if (image[y][x][0] != 255) return 0;
    } return 1;
}

// Return 1 if a cell is contained in the square window of width ws around the white (i,j) pixel
static inline int cell_test(unsigned char image[BMP_HEIGTH][BMP_WIDTH][BMP_CHANNELS], int i, int j, int ws) {
    int a = ws/2 + 1;
    int b = ws/2;

    //Define corners of the test windows  within the image
    int top    = (i - b < 0) ? 0 : i - b;
    int bottom = (i + a >= BMP_HEIGTH) ? BMP_HEIGTH - 1 : i + a;
    int left   = (j - b < 0) ? 0 : j - b;
    int right  = (j + a >= BMP_WIDTH) ? BMP_WIDTH - 1 : j + a;

    // Check corners
    if (image[top][left][0]    == 255 ||
        image[top][right][0]   == 255 ||
        image[bottom][left][0] == 255 ||
        image[bottom][right][0] == 255)
        return 0;

    // Check top and bottom edges (excluding corners already checked)
    for (int x = left + 1; x < right; x++) {
        if (image[top][x][0] == 255 || image[bottom][x][0] == 255)
            return 0;
    }

    // Check left and right edges (excluding corners already checked)
    for (int y = top + 1; y < bottom; y++) {
        if (image[y][left][0] == 255 || image[y][right][0] == 255)
            return 0;
    }

    return 1;
}


static inline int cell_test2(unsigned char image[BMP_HEIGTH][BMP_WIDTH][BMP_CHANNELS], int i, int j, int ws, int split, int n) {
    int a = ws/2 + 1;
    int b = ws/2;

    /* window bounds */
    int top    = (i - b < 0) ? 0 : i - b;
    int bottom = (i + a >= BMP_HEIGTH) ? BMP_HEIGTH - 1 : i + a;
    int left   = (j - b < 0) ? 0 : j - b;
    int right  = (j + a >= BMP_WIDTH) ? BMP_WIDTH - 1 : j + a;

    /* Strict mode: reject if ANY border pixel (including corners) is white */
    if (!split) {
        /* corners */
        if (image[top][left][0] == 255 ||
            image[top][right][0] == 255 ||
            image[bottom][left][0] == 255 ||
            image[bottom][right][0] == 255)
            return 0;

        /* top/bottom edges (excluding corners) */
        for (int x = left + 1; x < right; x++) {
            if (image[top][x][0] == 255 || image[bottom][x][0] == 255)
                return 0;
        }


        /* left/right edges (excluding corners) */
        for (int y = top + 1; y < bottom; y++) {
            if (image[y][left][0] == 255 || image[y][right][0] == 255)
                return 0;
        }
        return 1;
    }

    /* Split mode: count whites on each side EXCLUDING corners first */
    int white_top = 0, white_bottom = 0;
    int white_left = 0, white_right = 0;

     /* non-corner horizontal pixels */
    for (int x = left + 1; x < right; x++) {
        if (image[top][x][0] == 255)    white_top++;
        if (image[bottom][x][0] == 255) white_bottom++;
    }


    /* non-corner vertical pixels */
    for (int y = top + 1; y < bottom; y++) {
        if (image[y][left][0] == 255)  white_left++;
        if (image[y][right][0] == 255) white_right++;
    }

    /* corner flags */
    int c_tl = (image[top][left][0] == 255);
    int c_tr = (image[top][right][0] == 255);
    int c_bl = (image[bottom][left][0] == 255);
    int c_br = (image[bottom][right][0] == 255);

    /* Assign corners to adjacent sides:
       - If exactly one adjacent side already has whites -> assign to that side.
       - If both adjacent sides already have whites -> count for both.
       - If neither has whites -> assign to vertical side by default (left/right).
    */

    if (c_tl) {
        if (white_left > 0 && white_top == 0)           white_left++;
        else if (white_top > 0 && white_left == 0)      white_top++;
        else if (white_left > 0 && white_top > 0) {     white_left++; white_top++; }
        else                                            white_left++; /* prefer vertical */
    }
    if (c_tr) {
        if (white_right > 0 && white_top == 0)          white_right++;
        else if (white_top > 0 && white_right == 0)     white_top++;
        else if (white_right > 0 && white_top > 0) {    white_right++; white_top++; }
        else                                            white_right++; /* prefer vertical */
    }
    if (c_bl) {
        if (white_left > 0 && white_bottom == 0)        white_left++;
        else if (white_bottom > 0 && white_left == 0)   white_bottom++;
        else if (white_left > 0 && white_bottom > 0) {  white_left++; white_bottom++; }
        else                                            white_left++; /* prefer vertical */
    }
    if (c_br) {
        if (white_right > 0 && white_bottom == 0)       white_right++;
        else if (white_bottom > 0 && white_right == 0)  white_bottom++;
        else if (white_right > 0 && white_bottom > 0) { white_right++; white_bottom++; }
        else                                            white_right++; /* prefer vertical */
    }

    /* Count how many sides actually contain whites now */
    int sides_with_white = 0;
    if (white_top > 0 ) sides_with_white++;
    if (white_bottom > 0) sides_with_white++;
    if (white_left   > 0) sides_with_white++;
    if (white_right  > 0) sides_with_white++;

    if (sides_with_white == 0) {
        return 1;
    }

    /* Valid only if exactly one side has whites and that side's count < n */
    if (sides_with_white == 1) {
        if (white_top > 5 && white_top < n)    return 2;
        if (white_bottom > 5 && white_bottom < n) return 3;
        if (white_left > 5 && white_left < n)  return 4;
        if (white_right > 5 && white_right < n) return 5;
    }

    return 0;
}



// Transform RGB image into it's gary level version
void RGB2gray(unsigned char color_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
    for (int i = 0; i < BMP_WIDTH; i++) {
        for (int j = 0; j < BMP_HEIGTH; j++) {
            unsigned char gray_value = (color_image[i][j][0] + color_image[i][j][1] + color_image[i][j][2])/3;
            set_gray_value(gray_image[i][j], gray_value);
        }
    }
}

// Transform gray image into it's Black&White only version using a threshold
void gray2BW(unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int threshold) {
    for (int i = 0; i < BMP_WIDTH; i++) {
        for (int j = 0; j < BMP_HEIGTH; j++) {
            if (gray_image[i][j][0] > threshold) {
                set_gray_value(gray_image[i][j], 255);
            }
            else {
                set_gray_value(gray_image[i][j], 0);
            }
        }
    }
}

// Executing the erosion step onto a BW image given an erosion pattern (style)
int erosion(unsigned char (*src)[BMP_HEIGTH][BMP_CHANNELS], unsigned char (*dst)[BMP_HEIGTH][BMP_CHANNELS], int style) {
    int fully_eroded = 1;
    memset(dst, 0, BMP_WIDTH * BMP_HEIGTH * BMP_CHANNELS * sizeof(unsigned char));
    for (int i = 1; i < BMP_WIDTH-1; i++) {
        for (int j = 1; j < BMP_HEIGTH-1; j++) {
            if (src[i][j][0] > 0 && erosion_test_mask(src, i, j, mask_list[style])) {
                set_gray_value(dst[i][j], 255);
                fully_eroded = 0;
            }
        }
    }
    return fully_eroded;
}

// Executing the detection of white cells onto a BW image given a window size (ws)
int detection(unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int cells_center[MAX_CELLS][2], int nb_cells, int ws, int split, int n) {
    for (int i = 1; i < BMP_WIDTH-1; i++) {
        for (int j = 1; j < BMP_HEIGTH-1; j++) {
            int d = cell_test2(image, i, j, ws, split, n);
            if (image[i][j][0] == 255 && (d > 0)) {
                cells_center[nb_cells][0] = i;
                cells_center[nb_cells][1] = j;
                nb_cells++;
                int a = ws/2 - 1;
                for (int k = 0; k < ws; k++) {
                    for (int l = 0; l < ws; l++) {
                        if (i-a+k > 0 && i-a+k < 949 && j-a+l > 0 && j-a+l < 949) {
                            set_gray_value(image[i-a+k][j-a+l], 0);
                        }
                    }
                }
            }
        }
    }
    return nb_cells;
}

// Generate a result image by drawing a red cross on the original image at each detected cell
void generate_image(unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char labelled_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int cells_center[MAX_CELLS][2], int nb_cells) {
    for (int i = 0; i < BMP_WIDTH; i++) {
        for (int j = 0; j < BMP_HEIGTH; j++) {
            set_gray_value(labelled_image[i][j], image[i][j][0]);
        }
    }
    for (int i = 0; i < nb_cells; i++) {
        int x = cells_center[i][0];
        int y = cells_center[i][1];

        labelled_image[x][y][1] = 0;
        labelled_image[x][y][2] = 0;
        labelled_image[x+1][y][1] = 0;
        labelled_image[x+1][y][2] = 0;
        labelled_image[x][y+1][1] = 0;
        labelled_image[x][y+1][2] = 0;
        labelled_image[x+1][y+1][1] = 0;
        labelled_image[x+1][y+1][2] = 0;

        for (int j = 0; j < 16; j++) {

            labelled_image[MIN(MAX(x-7 + j, 0),949)][y][0] = 255;
            labelled_image[MIN(MAX(x-7 + j, 0),949)][y][1] = 0;
            labelled_image[MIN(MAX(x-7 + j, 0),949)][y][2] = 0;

            labelled_image[MIN(MAX(x-7 + j, 0),949)][y+1][0] = 255;
            labelled_image[MIN(MAX(x-7 + j, 0),949)][y+1][1] = 0;
            labelled_image[MIN(MAX(x-7 + j, 0),949)][y+1][2] = 0;

            labelled_image[MIN(MAX(x-7 + j, 0),949)][y+1][0] = 255;
            labelled_image[x][MIN(MAX(y-7 + j, 0),949)][1] = 0;
            labelled_image[x][MIN(MAX(y-7 + j, 0),949)][2] = 0;

            labelled_image[MIN(MAX(x-7 + j, 0),949)][y+1][0] = 255;
            labelled_image[x+1][MIN(MAX(y-7 + j, 0),949)][1] = 0;
            labelled_image[x+1][MIN(MAX(y-7 + j, 0),949)][2] = 0;
        }
    }
}

// Main algorithm creating a result image giving an input image and a unused output image space
int main_algorithm(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int cells_center[MAX_CELLS][2], int nb_cells, int threshold) {

    //Formatting the Input Image
    RGB2gray(input_image, output_image);
    gray2BW(output_image, threshold);
    write_bitmap(output_image, "dbg.bmp");

    static unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];     // Temp Eroded Image
    unsigned char (*img_buffer[2])[BMP_HEIGTH][BMP_CHANNELS] = {output_image, eroded_image}; //Img swapping for erosion

    int done;
    int style = 6;      //First erosion pattern
    int swap_count = 0;

    do {

        // Apply erosion
        done = erosion(img_buffer[swap_count%2], img_buffer[(swap_count+1)%2], style); // returns 1 if fully eroded, 0 otherwise

        //DEBUG CODDE
        //char filename[64];
        //sprintf(filename, "debug%d.bmp", nb_cells);
        //write_bitmap(img_buffer[(swap_count+1)%2], filename);
        // END OF DEBUG CODE

        style = 0;

        if (!done) {
            // Not fully eroded → run detection and generation
            nb_cells = detection(img_buffer[(swap_count+1)%2], cells_center, nb_cells, 14,swap_count>4,(swap_count>6 ? 8 : 11));
            swap_count++;
        }
    } while (!done);

    generate_image(input_image, output_image, cells_center, nb_cells);
    return nb_cells;
}

//Calculates the otsu method threshold for a grey image
unsigned char otsu_method(unsigned char grey_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
    int histogram[256] = {0};
    int total = BMP_WIDTH * BMP_HEIGTH;
    int sum = 0;
    for (int i = 0; i < BMP_WIDTH; i++) {
        for (int j = 0; j < BMP_HEIGTH; j++) {
            histogram[grey_image[i][j][0]]++;
            sum += grey_image[i][j][0];
        }
    }

    // 3️⃣ Otsu variables
    long sumB = 0;      // sum of background
    int wB = 0;         // weight of background
    int wF = 0;         // weight of foreground
    double maxVar = -1; // max between-class variance
    unsigned char threshold = 0;

    for (int t = 0; t < 256; t++) {
        wB += histogram[t];            // background weight
        if (wB == 0) continue;

        wF = total - wB;                        // foreground weight
        if (wF == 0) break;

        sumB += t * histogram[t];                // background sum

        double mB = (double)sumB / wB;           // background mean
        double mF = (double)(sum - sumB) / wF;   // foreground mean

        // Between-class variance
        double varBetween = (double)wB * wF * (mB - mF) * (mB - mF);

        if (varBetween > maxVar) {
            maxVar = varBetween;
            threshold = t;
        }
    }

    return threshold;
}
