# This Python file uses the following encoding: utf-8
import sys, os, pathlib, re 
from PySide6 import QtCore, QtWidgets, QtGui
from MainUI import Ui_DruIsoMapEdit

RES_PATH = 'res'
RES_EXT = 'res'

# A dict of items.  Each item is a dict containing a name, description, texture file and isometric tile spritesheet
RES_FILES = {}

def populate_res_file_struct():
    # Scan the res directory
    # Call the parser on each res file
    RES_FILES.clear()
    res_list = os.scandir(RES_PATH)
    for res in res_list:
        if res.is_file():
            (name, ext) = str(res.name).rsplit('.', 1)
            if ext == RES_EXT:
                parse_res_file(str(res.path))


def parse_res_file(name):
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
        RES_FILES[res_name] = {'name': res_name, 'description': res_desc, 'texture': res_tex, 
                               'tex_size': res_tex_size, 'iso_tile': res_iso , 'iso_size': res_iso_size }

    # Extract the ID, description, texture and isometric tile sheet
    # Append that data to the RES_FILES structure

class MainWindow(QtWidgets.QMainWindow, Ui_DruIsoMapEdit):
    tile_width = 64
    tile_height = 64
    tilemap_width = 32
    tilemap_height = 32
    tilemap_layers = 16

    def __init__(self):
        super(MainWindow, self).__init__()
        self.setupUi(self)

        # Set up pixmaps
        self.EditorAreaPixmap = QtGui.QPixmap(self.tile_width * self.tilemap_width, self.tile_height * self.tilemap_height)
        self.EditorArea.setPixmap(self.EditorAreaPixmap)
        self.TexPreviewPixmap = QtGui.QPixmap(241, 275)
        self.TexPreviewPixmap.fill(QtGui.QColor(0, 0, 0))
        self.TexPreview.setPixmap(self.TexPreviewPixmap)

        self.render_grid()
        self.populate_resource_list()

        # Set up signals
        self.PaletteEntryList.currentRowChanged.connect(self.update_palette_preview)

    def render(self):
        self.render_grid()
        self.render_tiles()
        self.EditorArea.update()

    def render_tiles(self):
        pass

    def render_grid(self):
        pixmap = self.EditorArea.pixmap()

        gray = QtGui.QColor(128, 128, 128)
        pixmap.fill(QtGui.QColor(255, 255, 255))
        painter = QtGui.QPainter(pixmap)
        painter.setPen(gray)
        
        for x in range(0, self.tilemap_width):
            painter.drawLine(x * self.tile_width, 0, x * self.tile_width, self.tile_height * self.tilemap_height - 1 )
        
        # Draw the horizontal lines
        for y in range(0, self.tilemap_height):
            painter.drawLine(0, y * self.tile_height, self.tile_width * self.tilemap_width - 1, y * self.tile_height)
        
        # Draw the bottom and right lines
        painter.drawLine(self.tile_width * self.tilemap_width - 1, 0, self.tile_width * self.tilemap_width -1, self.tile_height * self.tilemap_height - 1)
        painter.drawLine(0, self.tile_height * self.tilemap_height - 1, self.tile_width * self.tilemap_width - 1, self.tile_height * self.tilemap_height - 1)
        painter.end()

        self.EditorArea.setPixmap(pixmap)   

    def populate_resource_list(self):
        populate_res_file_struct()
        for key in RES_FILES.keys():
            self.PaletteEntryList.addItem(key)

    def update_palette_preview(self, row):
        # Get the name of the currently active entry
        i = self.PaletteEntryList.item(row).text()
        res_item = RES_FILES[i]
        self.PalItemName.setText(res_item["name"])
        self.PalItemDescription.setText(res_item["description"])

        # Load the image files and render them to the palette preview label pixmap
        tex_file = QtGui.QImage(RES_PATH + "/" + res_item["texture"])
        iso_tile_file = QtGui.QImage(RES_PATH + "/" + res_item["iso_tile"])
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
