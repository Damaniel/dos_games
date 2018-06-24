The contents of this folder:

MOVEIT.SPR - the sprite sheet for the game, readable by LOCGAED
CGATILER.PY - a Python script that takes an image with the 16 CGA text colors and does the following:
                 * divides it into 5x5 tiles
                 * extracts all unique tiles into a PNG sprite strip
                 * generates a list mapping tiles in the image into tiles in the sprite strip, suitable
                   for placing into a byte (or integer, if >255) array.
             (Note that this script doesn't import directly into LOCGAED - you'll still have to draw the
              tiles into your sprite strip of choice.)
             (Note 2: The program also takes an offset parameter, which is used to specify that the tiles
              will start at the 'offset'th value in the destination LOCGAED sprite sheet.  This is useful 
              if you're embedding sprites at the end of an existing sprite sheet.)