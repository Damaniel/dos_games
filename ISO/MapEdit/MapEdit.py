# This Python file uses the following encoding: utf-8
import sys, os, pathlib, re 
from PySide6 import QtCore, QtWidgets, QtGui
from MainUI import Ui_DruIsoMapEdit
import Globals

def populate_res_file_struct():
    """Fills the RES_FILES structure with available resources."""
    Globals.RES_FILES.clear()
    res_list = os.scandir(Globals.RES_PATH)
    for res in res_list:
        if res.is_file():
            (name, ext) = str(res.name).rsplit('.', 1)
            if ext == Globals.RES_EXT:
                parse_res_file(str(res.path))


def parse_res_file(name):
    """Extracts information about the resource described in a file.
    
       Keyword arguments:
       name -- the name of the resource file
    """
    id_re = re.compile(r"^\s*name:\s+(.+)$")
    desc_re = re.compile(r"^\s*description:\s+(.+)$")
    tex_re = re.compile(r"^\s*texture:\s+(.+)$")
    tex_size_re = re.compile(r"^\s*tex_size:\s+(.+)$")
    iso_re = re.compile(r"^\s*iso_tile:\s+(.+)$")
    iso_size_re = re.compile(r"^\s*iso_size:\s+(.+)$")
    f = open(name, 'r')
    res_name = None
    res_desc = None
    res_tex = None
    res_tex_size = None
    res_iso = None
    res_iso_size = None
    for line in f:
        m = id_re.match(line)
        if m is not None:
            res_name = m.group(1)
        m = desc_re.match(line)
        if m is not None:
            res_desc = m.group(1)
        m = tex_re.match(line)
        if m is not None:
            res_tex = m.group(1)
        m = tex_size_re.match(line)
        if m is not None:
            res_tex_size = int(m.group(1))
        m = iso_re.match(line)
        if m is not None:
            res_iso = m.group(1)
        m = iso_size_re.match(line)
        if m is not None:
            res_iso_size = m.group(1)

    f.close()

    if res_name is None or res_desc is None or res_tex is None or res_iso is None:
        print("warning: couldn't parse " + name)
    else:
        Globals.RES_FILES[res_name] = {'name': res_name, 'description': res_desc, 'texture': res_tex, 
                               'tex_size': res_tex_size, 'iso_tile': res_iso , 'iso_size': res_iso_size }

class MainWindow(QtWidgets.QMainWindow, Ui_DruIsoMapEdit):
    """The MainWindow for the map editor application."""
    current_elevation = 0

    def __init__(self):
        super(MainWindow, self).__init__()
        self.setupUi(self)

        # Set up pixmaps
        self.EditorAreaPixmap = QtGui.QPixmap(Globals.TILE_WIDTH * Globals.TILEMAP_WIDTH, Globals.TILE_HEIGHT * Globals.TILEMAP_HEIGHT)
        self.EditorArea.setPixmap(self.EditorAreaPixmap)
        self.TexPreviewPixmap = QtGui.QPixmap(241, 275)
        self.TexPreviewPixmap.fill(QtGui.QColor(0, 0, 0))
        self.TexPreview.setPixmap(self.TexPreviewPixmap)

        self.initialize_map_area()
        self.render_map_area()
        self.populate_resource_list()

        # Initialize other values
        self.CurLevel.setText(str(self.current_elevation))

        # Set up signals
        self.PaletteEntryList.currentRowChanged.connect(self.update_palette_preview)
        self.UpButton.clicked.connect(self.increment_floor)
        self.DownButton.clicked.connect(self.decrement_floor)

        # Set up filters
        self.EditorArea.installEventFilter(self)

    def initialize_map_area(self):
        for i in range(Globals.TILEMAP_LAYERS):
            cols = []
            for j in range (Globals.TILE_WIDTH):
                row = []
                for k in range (Globals.TILE_HEIGHT):
                    row.append(-1)
                cols.append(row)
            Globals.MAP_DATA.append(cols)

    def eventFilter(self, object, event):
        if object == self.EditorArea:
            if event.type() == QtCore.QEvent.MouseButtonRelease:
                print("Mouse released!")
                print("Release is at " + str(event.position().x()) + ", " + str(event.position().y()))
                tile_x = int(event.position().x() / Globals.TILE_WIDTH)
                tile_y = int(event.position().y() / Globals.TILE_HEIGHT)
                print("Release tile is " + str(tile_x) + ", " + str(tile_y))
                self.place_tile(tile_x, tile_y)

        return False

    def place_tile(self, tile_x, tile_y):
        # Only place a tile if one is active
        if Globals.CURRENT_TILE is None:
            return

        # Add the tile index to the appropriate part of the map data
        Globals.MAP_DATA[self.current_elevation][tile_x][tile_y] = Globals.CURRENT_TILE[1]

        # Draw the current tile to the map area
        pixmap = self.EditorArea.pixmap()
        painter = QtGui.QPainter(pixmap)
        painter.drawImage(QtCore.QPoint(tile_x * Globals.TILE_WIDTH, tile_y * Globals.TILE_HEIGHT), Globals.CURRENT_TILE[0])
        painter.end()
        self.EditorArea.setPixmap(pixmap)
        self.EditorArea.update()

    def increment_floor(self):
        """Adds one to the currently active floor."""
        self.current_elevation = self.current_elevation + 1
        if self.current_elevation >= Globals.TILEMAP_LAYERS:
            self.current_elevation = Globals.TILEMAP_LAYERS - 1
        self.CurLevel.setText(str(self.current_elevation))

    def decrement_floor(self):
        """Subtracts one from the currently active floor."""
        self.current_elevation = self.current_elevation - 1
        if self.current_elevation < 0:
            self.current_elevation = 0
        self.CurLevel.setText(str(self.current_elevation))
    
    def render_map_area(self):
        """Draw the map area."""
        self.render_grid()
        self.render_tiles()
        self.EditorArea.update()

    def render_tiles(self):
        """Draws all tiles in the map area."""
        pass

    def render_grid(self):
        """Draws a grid in the map area."""
        pixmap = self.EditorArea.pixmap()

        gray = QtGui.QColor(128, 128, 128)
        pixmap.fill(QtGui.QColor(255, 255, 255))
        painter = QtGui.QPainter(pixmap)
        painter.setPen(gray)
        
        for x in range(0, Globals.TILEMAP_WIDTH):
            painter.drawLine(x * Globals.TILE_WIDTH, 0, x * Globals.TILE_WIDTH, Globals.TILE_HEIGHT * Globals.TILEMAP_HEIGHT - 1 )
        
        # Draw the horizontal lines
        for y in range(0, Globals.TILEMAP_HEIGHT):
            painter.drawLine(0, y * Globals.TILE_HEIGHT, Globals.TILE_WIDTH * Globals.TILEMAP_WIDTH - 1, y * Globals.TILE_HEIGHT)
        
        # Draw the bottom and right lines
        painter.drawLine(Globals.TILE_WIDTH * Globals.TILEMAP_WIDTH - 1, 0, Globals.TILE_WIDTH * Globals.TILEMAP_WIDTH -1, Globals.TILE_HEIGHT * Globals.TILEMAP_HEIGHT - 1)
        painter.drawLine(0, Globals.TILE_HEIGHT * Globals.TILEMAP_HEIGHT - 1, Globals.TILE_WIDTH * Globals.TILEMAP_WIDTH - 1, Globals.TILE_HEIGHT * Globals.TILEMAP_HEIGHT - 1)
        painter.end()

        self.EditorArea.setPixmap(pixmap)   

    def populate_resource_list(self):
        """Adds all resources to the palette entry list."""
        populate_res_file_struct()
        for key in Globals.RES_FILES.keys():
            self.PaletteEntryList.addItem(key)

    def update_palette_preview(self, row):
        """Updates the name, description and images to match the current palette entry.
        
        Keyword arguments:
        row -- the row of the palette entry list to preview
        """

        # Get the name of the currently active entry
        i = self.PaletteEntryList.item(row).text()
        res_item = Globals.RES_FILES[i]
        self.PalItemName.setText(res_item["name"])
        self.PalItemDescription.setText(res_item["description"])

        # Load the image files and render them to the palette preview label pixmap
        tex_file = QtGui.QImage(Globals.RES_PATH + "/" + res_item["texture"])
        Globals.CURRENT_TILE = [tex_file, row]
        iso_tile_file = QtGui.QImage(Globals.RES_PATH + "/" + res_item["iso_tile"])
        pixmap = self.TexPreview.pixmap()
        pixmap.fill(QtGui.QColor(0, 0, 0))
        painter = QtGui.QPainter(pixmap)
        painter.drawImage(QtCore.QPoint(85, 60), tex_file)
        painter.scale(1.5, 1.5)
        painter.drawImage(QtCore.QPoint(9, 95), iso_tile_file)
        painter.scale(.67, .67)
        painter.end()
        self.TexPreview.setPixmap(pixmap)
        self.TexPreview.update()

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)

    window = MainWindow()
    window.show()

    populate_res_file_struct()
    sys.exit(app.exec())
