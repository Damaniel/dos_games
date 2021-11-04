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

/* Stuff to think about:
    - Does screen even need a palette?
    - If so, should getting the palette automatically load it into 
      screen's palette?
    - Should setting the palette mirror the palette data to 
      screen's palette?
    - setpalette/getpalette/updatepalette?
      - setpalette = copy user palette to screen palette
      - getpalette = copy screen palette to user palette
      - updatepalette - copy VGA palette data to screen palette
      
      Usage: user calls setpalette to update the palette, 
             screen does the update
             user calls getpalette to get a copy of the palette,
             screen gets the palette from VGA and updates their
             palette, then sends it to the user.
             (That way, all palette changes should go through
             the screen, since the screen should match the
             currently used palette.)             
*/

int main(void) {
  Color c;
  int i, j;
  Sprite *testsprite;
  Bitmap *b;
  // int loopcount;
  srand(time(NULL));
  
  if (!graphinit())
    return -1;
  
  setmode(MODE13H);

  /* Force load the palette */
  b = loadpcx("test/credits.pcx", 1);
  setpalette(b->palette);
  
  clear(screen, 0);
  
  testsprite = loadsprite("test/sprites.pcx", 16, 16, 3, 0);
  
  //loopcount=0;
  
  //while(!kbhit()) { 
    //blitsprite(screen, testsprite, rand()%3, rand()%20 * 16, rand()%12 * 16);
    //loopcount++;
    /*drawsysfontchar(screen, 
                    (rand()%40) * 8,
                    (rand()%25) * 8,
                    (rand()%94) + 33,
                    (rand()%255) + 1,
                    0);*/
  //}  
 
  blit(screen, b, 0, 0, 0, 0, 320, 200);
  blitsprite(screen, testsprite, 0, 8, 8);

  sleep(3);
  
  setmode(TEXTMODE);

  //printf("%d\n", loopcount);
  freesprite(testsprite);
  freebitmap(b);
  graphshutdown();
  
  return 0;
}
 