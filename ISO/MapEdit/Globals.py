RES_PATH = 'res'
RES_EXT = 'res'

# A dict of items.  Each item is a dict containing a name, description, texture file and isometric tile spritesheet
RES_FILES = {}

# Size of the tiles used in the map editing area
TILE_WIDTH = 64
TILE_HEIGHT = 64

# Size of the map itself (in tiles)
TILEMAP_WIDTH = 32
TILEMAP_HEIGHT = 32

# The size of the isometric tiles
ISO_TILE_WIDTH = 48
ISO_TILE_HEIGHT = 24

# The offsets of the 3 tile brightnesses in the iso tile strip
NORMAL_TILE = 0
DARK_TILE = 1
DARKER_TILE = 2

# The number of levels of tiles that make up the Z axis of the map
TILEMAP_LAYERS = 16

# The number of tiles actually used in this map
NUM_TILES_USED = 0
# The actual tile indices from the tile list that were used in the map
TILES_USED = []

# The currently selected tile in the tile palette
CURRENT_TILE = -1

# A structure of all tile image data, in the form [id, QImage]
TILE_DATA = []
# A structure of all isometric image data, in the form [id, QImage]
ISO_DATA = []

# The names of the DOS files for each tile index
DOS_FILES = []

# The actual map data.  A 3 dimeensional array representing x, y, z coordinates
# and their respective tile data
MAP_DATA = []

# The currently loaded file
CURRENT_MAP_FILE = None

# Has the current map been modified since the last save?
IS_MODIFIED = False

ROTATION_0_DEGREES = 0
ROTATION_90_DEGREES = 1
ROTATION_180_DEGREES = 2
ROTATION_270_DEGREES = 3

g_preview_x = 2
g_preview_y = 2
g_preview_z = 2

g_rotation = ROTATION_0_DEGREES

g_offset_matrix = [(0, -4), (1, -3), (2, -2), (3, -1), (4, 0),
                   (-1, -3), (0, -2), (1, -1), (2, 0), (3, 1),
                   (-2, -2), (-1, -1), (0, 0), (1, 1), (2, 2),
                   (-3, -1), (-2, 0), (-1, 1), (0, 2), (1, 3),
                   (-4, 0), (-3, 1), (-2, 2), (-1, 3), (0, 4)]

g_lighting_matrix =[
                    [
                        [2, 2, 2, 2, 2],
                        [2, 2, 2, 2, 2],
                        [2, 2, 1, 2, 2],
                        [2, 2, 2, 2, 2],
                        [2, 2, 2, 2, 2]
                    ],
                    [
                        [2, 2, 2, 2, 2],
                        [2, 2, 1, 2, 2],
                        [2, 1, 0, 1, 2],
                        [2, 2, 1, 2, 2],
                        [2, 2, 2, 2, 2]
                    ],
                    [
                        [2, 2, 2, 2, 2],
                        [2, 1, 0, 1, 2],
                        [2, 0, 0, 0, 2],
                        [2, 1, 0, 1, 2],
                        [2, 2, 2, 2, 2]
                    ],
                    [
                        [2, 2, 2, 2, 2],
                        [2, 2, 1, 2, 2],
                        [2, 1, 0, 1, 2],
                        [2, 2, 1, 2, 2],
                        [2, 2, 2, 2, 2]
                    ],
                    [
                        [2, 2, 2, 2, 2],
                        [2, 2, 2, 2, 2],
                        [2, 2, 1, 2, 2],
                        [2, 2, 2, 2, 2],
                        [2, 2, 2, 2, 2]
                    ]
                ]

g_rotation_matrix = [[(-2, -2), (-2, -1), (-2, 0), (-2, 1), (-2, 2),
                     (-1, -2), (-1, -1), (-1, 0), (-1, 1), (-1, 2),
                     ( 0, -2), ( 0, -1), ( 0, 0), ( 0, 1), ( 0, 2),
                     ( 1, -2), ( 1, -1), ( 1, 0), ( 1, 1), ( 1, 2),
                     ( 2, -2), ( 2, -1), ( 2, 0), ( 2, 1), ( 2, 2)],
                     [(-2, 2), (-1, 2), (0, 2), (1, 2), (2, 2),
                      (-2, 1), (-1, 1), (0, 1), (1, 1), (2, 1),
                      (-2, 0), (-1, 0), (0, 0), (1, 0), (2, 0),
                      (-2, -1), (-1, -1), (0, -1), (1, -1), (2, -1),
                      (-2, -2), (-1, -2), (0, -2), (1, -2), (2, -2)],
                     [(2, 2), (2, 1), (2, 0), (2, -1), (2, -2),
                      (1, 2), (1, 1), (1, 0), (1, -1), (1, -2),
                      (0, 2), (0, 1), (0, 0), (0, -1), (0, -2),
                      (-1, 2), (-1, 1), (-1, 0), (-1, -1), (-1, -2),
                      (-2, 2), (-2, 1), (-2, 0), (-2, -1), (-2, -2)],
                     [(2, -2), (1, -2), (0, -2), (-1, -2), (-2, -2),
                      (2, -1), (1, -1), (0, -1), (-1, -1), (-2, -1),
                      (2, 0), (1, 0), (0, 0), (-1, 0), (-2, 0),
                      (2, 1), (1, 1), (0, 1), (-1, 1), (-2, 1),
                      (2, 2), (1, 2), (0, 2), (-1, 2), (-2, 2)]
                    ]