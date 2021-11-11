/* Copyright 2021 Shaun Brandt
   
   Permission is hereby granted, free of charge, to any person obtaining a 
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
   DEALINGS IN THE SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>

/* Expand

   This tool is meant to fix an issue with GIMP's export of PCX files versus
   the expectations of Allegro.  If a PCX has 16 or fewer colors, GIMP will
   (correctly) output the 16 colors into the header.  However, when loading
   PCX files as an 8 bit image, Allegro expects a full 256 color palette after
   the image data.  This program does the following:
   
   - reads in an image with 16 or fewer colors
      (checked by looking for a value < 8 in byte 4) 
   - modifies the header to make it an 8 bit per pixel one plane image
   - decompresses the RLE 4-bpp data
   - expands that data to 8-bpp and recompresses it
   - creates a new 256 color palette with the original 16 colors of the 
     palette in the header plus 240 dummy colors
   - saves the new header, image data, and palette into the output file.

   Ugh.
   
   Usage: expand <input image> <output image>
*/
int main(int argc, char *argv[]) {
    FILE *input, *output;
    short bytes_per_line, width, height;
    int i, j;
    int data_size;
    int current_byte, run_length, run_val, cur_val;
    unsigned char c;
    char header[128];
    char palette[768];
   
    short *short_header;
    char *raw_image_data;
    char *image_data;

    if (argc <3) {
        printf("Usage: expand <input file> <output file>\n");
        exit(1);
    }

    input = fopen(argv[1], "rb");
    if (input == NULL) {
        printf("Unable to open input file!\n");
        exit(1);
    }

    output = fopen(argv[2], "wb");
    if (output == NULL) {
        printf("Unable to open output file!\n");
        fclose(input);
        exit(1);
    }

    fread(header, 128, sizeof(char), input);
    if(header[3] != 4 ) {
        printf("Image must be 16 or fewer colors!\n");
        fclose(input);
        fclose(output);
        exit(1);
    }

    short_header = (short *)header;

    /* Since the data itself is encoded in 4 bits per pixel format, it 
       has to be expanded.  */

    /* Expand the RLE data into a buffer */
    bytes_per_line = short_header[33];
    width = short_header[4] - short_header[2] + 1;
    height = short_header[5] - short_header[3] + 1;
    data_size = bytes_per_line * height * sizeof(char);

    image_data = (char *)malloc(width * height * sizeof(char));    
    raw_image_data = (char *)malloc(data_size * sizeof(char));

    current_byte = 0;
    while (current_byte < data_size) {
        c = fgetc(input);
        if (c < 192) {
            raw_image_data[current_byte] = c;
            current_byte++;
        } else {
            run_length = c - 192;
            c = fgetc(input);
            for(i=0; i<run_length; i++) 
                raw_image_data[current_byte+i] = c;
            current_byte+= run_length;
        }
    }

    current_byte = 0;
    for(j=0;j<height;j++) {
        /* Even */
        if(width % 2 == 0) {
            for (i=0;i< width/2; i++) {
                c = raw_image_data[j*bytes_per_line+i];
                image_data[current_byte] = c >>4;
                image_data[current_byte+1] = c &0x0F;
                current_byte+=2;
            }
        }
        else {
            for (i=0;i< width/2; i++) {
                c = raw_image_data[j*bytes_per_line+i];
                image_data[current_byte] = c >>4;
                image_data[current_byte+1] = c &0x0F;
                current_byte+=2;
            }
            c = raw_image_data[j*bytes_per_line + (bytes_per_line-1)];
            image_data[current_byte] = c >> 4;
            current_byte +=1;
        }
    }

    /* Fix the color data */

    /* Set the image bit count per plane to 8 */
    header[3] = 8;

    /* Recalculate the bytes per line value */
    bytes_per_line = short_header[33];

    /* Update new bytes per pixel (which is just width) */
    short_header[33] = width;

    /* Create the new palette */
    for(i = 0; i < 48; i++) {
        palette[i] = header[16+i];
    }
    for(i = 48; i < 768; i++ ) {
        palette[i] = 0;
    }

    /* Clear the color data out of the header */
    for(i = 16; i < 64; i++ ) {
        header[i] = 0;
    }

    fwrite(header, 128, sizeof(char), output);

    /* Write the new image data, recompressing as we go */
    /* Note: do this one line at a time! Apparently,
       all tools support line-limited RLE, but some don't
       support RLE that crosses lines.  Weird. */
    current_byte = 0;
    for(i=0; i< height; i++) {
        current_byte = 0;
        while(current_byte < width ) {
            run_length = 0;
            cur_val = image_data[i*width + current_byte];
            run_val = cur_val;
            while(run_val == cur_val && run_length < 63 && 
                  current_byte < width) {
                run_length++;
                current_byte++;
                cur_val = image_data[i* width +current_byte];
            }
            if(run_length == 1) {
                fputc(run_val, output);
            } else {
                fputc(run_length + 192, output);
                fputc(run_val, output);            
            }
        }
    }

    /* Write the palette data */
    fputc(0x0C, output);
    fwrite(palette, 768, sizeof(char), output);


    free(raw_image_data);
    free(image_data);

    fclose(input);
    fclose(output);

    exit(0);
}