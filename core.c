#include "cbmp.h"
#include "core.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* COMPARISON MACROS */

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define SAFE_IDX(x) ( (x) < 0 ? 0 : ((x) > 949 ? 949 : (x)) )

/* EROSION MASKS */

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


/* FUNCTIONS */

/* Set the gray value of a pixel in a specific channel */
static inline void set_gray_value (unsigned char pixel[3], unsigned char gray_value, int channel) {
    pixel[channel]  =  gray_value;
}


/* Return 1 if all mask pixels around (i, j) are white, else 0 */
int erosion_test_mask(unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int i, int j, int channel, MaskStruct m) {
    for (int k = 0; k < m.size; k++) {
        int y = SAFE_IDX(i + m.mask[k][0]);
        int x = SAFE_IDX(j + m.mask[k][1]);
        if (image[y][x][channel] != 255) return 0;
    } return 1;
}

/* Return 1 if a cell is contained in the square window of width ws around the white (i,j) pixel */
static inline int cell_test(unsigned char image[BMP_HEIGTH][BMP_WIDTH][BMP_CHANNELS], int i, int j, int channel, int ws, int split, int n_min, int n_max) {
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
        if (image[top][left][channel] == 255 ||
            image[top][right][channel] == 255 ||
            image[bottom][left][channel] == 255 ||
            image[bottom][right][channel] == 255)
            return 0;

        /* top/bottom edges (excluding corners) */
        for (int x = left + 1; x < right; x++) {
            if (image[top][x][channel] == 255 || image[bottom][x][channel] == 255)
                return 0;
        }


        /* left/right edges (excluding corners) */
        for (int y = top + 1; y < bottom; y++) {
            if (image[y][left][channel] == 255 || image[y][right][channel] == 255)
                return 0;
        }
        return 1;
    }

    /* Split mode: count whites on each side EXCLUDING corners first */
    int white_top = 0, white_bottom = 0;
    int white_left = 0, white_right = 0;

     /* non-corner horizontal pixels */
    for (int x = left + 1; x < right; x++) {
        white_top    += (image[top][x][channel] == 255);
        white_bottom += (image[bottom][x][channel] == 255);
    }

    /* non-corner vertical pixels */
    for (int y = top + 1; y < bottom; y++) {
        white_left  += (image[y][left][channel] == 255);
        white_right += (image[y][right][channel] == 255);
    }

    /* corner flags */
    int c_tl = (image[top][left][channel] == 255);
    int c_tr = (image[top][right][channel] == 255);
    int c_bl = (image[bottom][left][channel] == 255);
    int c_br = (image[bottom][right][channel] == 255);

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
    int sides_with_white = (white_top    > 0) +
                           (white_bottom > 0) +
                           (white_left   > 0) +
                           (white_right  > 0);

    if (sides_with_white == 0) {
        return 1;
    }

    /* Valid only if exactly one side has whites and that side's count < n */
    if (sides_with_white == 1) {
        if (white_top > n_min && white_top < n_max)    return 2;
        if (white_bottom > n_min && white_bottom < n_max) return 3;
        if (white_left > n_min && white_left < n_max)  return 4;
        if (white_right > n_min && white_right < n_max) return 5;
    }

    return 0;
}

/* Transform RGB image into it's gray level version */
void RGB2gray(unsigned char color_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
    for (int i = 0; i < BMP_WIDTH; i++) {
        for (int j = 0; j < BMP_HEIGTH; j++) {
            unsigned char gray_value = (color_image[i][j][0] + color_image[i][j][1] + color_image[i][j][2])/3;
            set_gray_value(gray_image[i][j], gray_value, 0);
        }
    }
}

/* Transform gray image into it's Black&White only version using a threshold */
void gray2BW(unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int threshold) {
    for (int i = 0; i < BMP_WIDTH; i++) {
        for (int j = 0; j < BMP_HEIGTH; j++) {
            if (gray_image[i][j][0] > threshold) {
                set_gray_value(gray_image[i][j], 255, 0);
            }
            else {
                set_gray_value(gray_image[i][j], 0, 0);
            }
        }
    }
}

/* Executing the erosion step onto a image given an erosion pattern (style) */
int erosion(unsigned char work_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int style, int target_channel) {
    int fully_eroded = 1;
    int input_channel = (target_channel+1)%2;

    // reset the target channel storing the new eroded gray image
    for (int i = 0; i < BMP_WIDTH; i++) {
        for (int j = 0; j < BMP_HEIGTH; j++) {
            set_gray_value(work_image[i][j], 0, target_channel);
        }
    }

    // erosion process, read on input channel and write on target channel
    for (int i = 1; i < BMP_WIDTH-1; i++) {
        for (int j = 1; j < BMP_HEIGTH-1; j++) {
            unsigned char *pixel = work_image[i][j];
            if (pixel[input_channel] == 255 && erosion_test_mask(work_image, i, j, input_channel, mask_list[style])) {
                set_gray_value(pixel, 255, target_channel);
                fully_eroded = 0;
            }

            // special value to do area skipping during detection
            else if (pixel[input_channel] == 100) {
                set_gray_value(pixel, 100, target_channel);
            }
        }
    }
    return fully_eroded;
}

/* Executing the detection of white cells onto a BW image given a window size (ws) */
int detection(unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int cells_center[MAX_CELLS][2], int nb_cells, int channel, int ws, int split, int n_min, int n_max) {
    for (int i = 1; i < BMP_WIDTH-1; i++) {
        for (int j = 1; j < BMP_HEIGTH-1; j++) {

            /* Flagged Zones Skip */
            int pixel = image[i][j][channel];
            if (pixel == 100) {
                j = j + ws;
            }
            else if (pixel == 255 && (cell_test(image, i, j, channel, ws, split, n_min, n_max) > 0)) {
                cells_center[nb_cells][0] = i;
                cells_center[nb_cells][1] = j;
                nb_cells++;
                int a = ws/2 - 1;
                for (int k = 0; k < ws; k++) {
                    for (int l = 0; l < ws; l++) {
                        if (i-a+k > 0 && i-a+k < (BMP_WIDTH-1) && j-a+l > 0 && j-a+l < (BMP_HEIGTH-1)) {
                            set_gray_value(image[i-a+k][j-a+l], 100, channel);
                        }
                    }
                }
            }
        }
    }
    return nb_cells;
}

/* Generate a result image by drawing a red cross on the original image at each detected cell */
void generate_image(unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char labelled_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int cells_center[MAX_CELLS][2], int nb_cells) {
    memcpy(labelled_image, image, sizeof(unsigned char) * BMP_WIDTH * BMP_HEIGTH * BMP_CHANNELS);
    for (int i = 0; i < nb_cells; i++) {
        int x = cells_center[i][0];
        int y = cells_center[i][1];

        /* Setting the GB channels of the cross to 0*/
        for (int j = 0; j < 16; j++) {

            labelled_image[SAFE_IDX(x-7 + j)][y][1] = 0;
            labelled_image[SAFE_IDX(x-7 + j)][y][2] = 0;

            labelled_image[SAFE_IDX(x-7 + j)][y+1][1] = 0;
            labelled_image[SAFE_IDX(x-7 + j)][y+1][2] = 0;

            labelled_image[x][SAFE_IDX(y-7 + j)][1] = 0;
            labelled_image[x][SAFE_IDX(y-7 + j)][2] = 0;

            labelled_image[x+1][SAFE_IDX(y-7 + j)][1] = 0;
            labelled_image[x+1][SAFE_IDX(y-7 + j)][2] = 0;
        }
    }
}

/* Main algorithm creating a result image giving an input image and an unused output image space */
int main_algorithm(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], int cells_center[MAX_CELLS][2], int threshold) {

    int nb_cells = 0;

    /* Formatting the Input Image */
    RGB2gray(input_image, output_image);
    gray2BW(output_image, threshold);

    /* DEBUG CODE */
    //write_bitmap(output_image, "debug_initial.bmp");
    /* END OF DEBUG CODE */

    int cnt_erosion = 0;     // Swap counter to remember where is the image

    /* Initial hard erosion */
    int done = erosion(output_image, 6, (cnt_erosion+1)%2);
    cnt_erosion++;
    do {
        int target_channel = (cnt_erosion+1)%2;

        /* Apply standard cross erosion */
        done = erosion(output_image, 0, target_channel);

        /* DEBUG CODE */
        // char filename[64];
        // sprintf(filename, "debug%d.bmp", nb_cells);
        // write_bitmap(output_image, filename);
        /* END OF DEBUG CODE */

        if (!done) {
            // Not fully eroded → run detection and generation
            nb_cells = detection(output_image, cells_center, nb_cells, target_channel, 14,(cnt_erosion>4),5, 11);
            cnt_erosion++;
        }
    } while (!done);

    /* Generate output image*/
    generate_image(input_image, output_image, cells_center, nb_cells);
    return nb_cells;
}

/* Calculates the otsu method threshold for a grey image */
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

    long sumB = 0;                                  // sum of background
    int wB = 0;                                     // weight of background
    int wF = 0;                                     // weight of foreground
    double maxVar = -1;                             // max between-class variance
    unsigned char threshold = 0;                    //threshold

    for (int t = 0; t < 256; t++) {
        wB += histogram[t];                         // background weight
        if (wB == 0) continue;

        wF = total - wB;                            // foreground weight
        if (wF == 0) break;

        sumB += t * histogram[t];                   // background sum

        double mB = (double)sumB / wB;              // background mean
        double mF = (double)(sum - sumB) / wF;      // foreground mean

        // Between-class variance
        double varBetween = (double)wB * wF * (mB - mF) * (mB - mF);

        if (varBetween > maxVar) {
            maxVar = varBetween;
            threshold = t;
        }
    }

    return threshold;
}
