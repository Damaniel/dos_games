from PIL import Image
import sys

fileName = sys.argv[1]
offset = int(sys.argv[3])

tiles = {}

im = Image.open(fileName)
px = im.load()

if (im.width % 5 != 0 or im.height % 5 != 0):
    print("Image width and height must be a multiple of 5!")
    sys.exit(1)

# Loop through each block of 5 by 5 pixels and create a 75x1 array (RGBx25).  Use it as a key
# into a dictionary, where the value is a count of the number of tiles of that type

for y in range(0, im.height, 5):
    for x in range (0, im.width, 5):
        tempList = []
        for ya in range(0, 5):
            for xa in range(0, 5):
                tempList.append(px[x+xa, y+ya][0]);
                tempList.append(px[x+xa, y+ya][1]);
                tempList.append(px[x+xa, y+ya][2]);
        tempTuple=tuple(tempList)
        if (not tempTuple in tiles):
            tiles[tempTuple] = 1
        else:
            tiles[tempTuple] += 1

numTiles = len(tiles)

# Create a new image big enough to hold all of the tiles
outImage = Image.new("RGB", (numTiles * 5, 5), 0)
outPx = outImage.load()

counter = 0
tileList = []
for t in tiles:
    tileList.append(t)
    for y in range(0, 5):
        for x in range(0, 5):
            # Note - each pixel set is a group of 3, so you have to iterate by 3s
            # (hence the 3s in the next line.)
            pix = (t[(y*5+x)*3], t[(y*5+x)*3+1], t[(y*5+x)*3+2])
            outPx[(counter*5) + x, y] = pix
    counter = counter + 1

outImage.save(sys.argv[2])

# Pass 2:
# Now, re-iterate through each of the tiles in the image, and dump the indices of those
# tiles in tileList into an output file.
for y in range(0, im.height, 5):
    for x in range (0, im.width, 5):
        tempList = []
        for ya in range(0, 5):
            for xa in range(0, 5):
                tempList.append(px[x+xa, y+ya][0]);
                tempList.append(px[x+xa, y+ya][1]);
                tempList.append(px[x+xa, y+ya][2]);
        tempTuple=tuple(tempList)
        for i in range(0, len(tileList)):
            if (tileList[i] == tempTuple):
                print(i+offset, ', ', sep='', end='')

