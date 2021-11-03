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
#include "damgraph.h"
#include "imageops.h"
#include <stdlib.h>
#include <time.h>
#include <pc.h>

int main(void) {
  Color c;
  int i, j;
  
  PCX image;
  
  if (!graphinit())
    return -1;
  
  setmode(MODE13H);

  srand(time(NULL));
  
  clear(screen, 0);
  
  while(!kbhit()) {
    drawsysfontchar(screen, 
                    (rand()%40) * 8,
                    (rand()%25) * 8,
                    (rand()%94) + 33,
                    (rand()%255) + 1,
                    0);
  }  
  loadpcx("test/credits.pcx", &image, 1);
  setpalette(image.palette);
  memcpy(screen, image.buffer, 64000*sizeof(char));
  
  sleep(3);
  
  setmode(TEXTMODE);
 
  printpcxheader(&image);
  freepcx(&image);
  
  graphshutdown();
  
  return 0;
}
 