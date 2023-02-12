# This Python file uses the following encoding: utf-8
import sys, os, pathlib, re, struct, string
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
    dosname_re = re.compile(r"\s*dos_name:\s+(.+)$")
    tex_re = re.compile(r"^\s*texture:\s+(.+)$")
    tex_size_re = re.compile(r"^\s*tex_size:\s+(.+)$")
    iso_re = re.compile(r"^\s*iso_tile:\s+(.+)$")
    iso_size_re = re.compile(r"^\s*iso_size:\s+(.+)$")
    f = open(name, 'r')
    res_name = None
    res_desc = None
    res_dosname = None
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
        m = dosname_re.match(line)
        if m is not None:
            res_dosname = m.group(1)
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

    if None in [res_name, res_desc, res_dosname, res_tex, res_iso]:
        print("warning: couldn't parse " + name)
    else:
        Globals.RES_FILES[res_name] = {'name': res_name, 'description': res_desc, 'dosname': res_dosname, 'texture': res_tex, 
                               'tex_size': res_tex_size, 'iso_tile': res_iso , 'iso_size': res_iso_size }

class MainWindow(QtWidgets.QMainWindow, Ui_DruIsoMapEdit):
    """The MainWindow for the map editor application."""
    current_elevation = 0

    def __init__(self):
        super(MainWindow, self).__init__()
        self.setupUi(self)
        self.set_title(None)

        # Set up pixmaps
        self.EditorAreaPixmap = QtGui.QPixmap(Globals.TILE_WIDTH * Globals.TILEMAP_WIDTH, Globals.TILE_HEIGHT * Globals.TILEMAP_HEIGHT)
        self.EditorArea.setPixmap(self.EditorAreaPixmap)
        self.TexPreviewPixmap = QtGui.QPixmap(241, 275)
        self.TexPreviewPixmap.fill(QtGui.QColor(0, 0, 0))
        self.TexPreview.setPixmap(self.TexPreviewPixmap)

        self.populate_resource_list()
        self.initialize_map_area()
        self.render_map_area()

        # Initialize other values
        self.CurLevel.setText(str(self.current_elevation))

        # Set up signals
        self.PaletteEntryList.currentRowChanged.connect(self.update_palette_preview)
        self.UpButton.clicked.connect(self.increment_floor)
        self.DownButton.clicked.connect(self.decrement_floor)

        # Set up filters
        self.EditorArea.installEventFilter(self)

        # Set up actions
        self.actionNew.triggered.connect(self.process_new)
        self.actionSave.triggered.connect(self.process_save)
        self.actionSave_As.triggered.connect(self.process_save_as)
        self.actionOpen.triggered.connect(self.process_open)
        self.actionClose.triggered.connect(self.process_close)
        self.actionExit.triggered.connect(self.process_exit)

        # Set up miscellaneous stuff
        self.EditorArea.setMouseTracking(True)

    def set_title(self, title):
        if title is None:
            t = '<New file>'
        else:
            t = title
        self.setWindowTitle("DruIsoMapEdit - " + t)

    def process_new(self):
        self.set_title(None)
        # Clear the map structure and draw an empty map
        self.initialize_map_file()
        self.initialize_map_area()
        self.render_map_area()

    def initialize_map_file(self):
        Globals.TILES_USED.clear()
        Globals.NUM_TILES_USED = 0
        Globals.IS_MODIFIED = False

    def process_close(self):
        if Globals.IS_MODIFIED == True:
            # Pop a 'do you want to save your progress' dialog, and pop a save dialog if needed
            result = self.show_confirm_save()
            # If cancel was selected, do nothing
            if result == QtWidgets.QMessageBox.Cancel:
                return
            # If save was selected, either save or pop up the save as dialog as needed
            if result == QtWidgets.QMessageBox.Save:
                self.process_save()
        # In either of the cases that aren't 'cancel', reset everything
        self.process_new()

    def process_exit(self):
        if Globals.IS_MODIFIED == True:
            # Pop a 'do you want to save your progress' dialog and a save dialog if needed
            result = self.show_confirm_save()
            # If cancel was selected, do nothing
            if result == QtWidgets.QMessageBox.Cancel:
                return
            # If save was selected, either save or pop up the save as dialog as needed
            if result == QtWidgets.QMessageBox.Save:
                self.process_save() 
        # In either of the cases that aren't 'cancel', close the app          
        self.close()

    def show_confirm_save(self):
        msg_box = QtWidgets.QMessageBox()
        msg_box.setText("The map file has been modified.")
        msg_box.setInformativeText("Do you want to save your changes?")
        msg_box.setStandardButtons(QtWidgets.QMessageBox.Save |  QtWidgets.QMessageBox.Discard | QtWidgets.QMessageBox.Cancel)
        msg_box.setDefaultButton(QtWidgets.QMessageBox.Save)
        return msg_box.exec()

    def process_open(self):
        self.initialize_map_file()
        self.initialize_map_area()
        file_name = QtWidgets.QFileDialog(self).getOpenFileName(filter="Map files (*.map)")
        if file_name is not None:
            self.current_elevation = 0
            self.load_map_file(file_name[0])
            self.render_map_area()
            self.set_title(file_name[0])

    def process_save(self):
        if Globals.CURRENT_MAP_FILE is None:
            self.process_save_as()
        else:
            self.save_map_file(Globals.CURRENT_MAP_FILE)

    def process_save_as(self):
        file_name = QtWidgets.QFileDialog(self).getSaveFileName(filter="Map files (*.map)")
        if file_name is not None and file_name[0] != '':
            self.save_map_file(file_name[0])

    def initialize_map_area(self):
        # Delete any existing map data that's floating around
        Globals.MAP_DATA.clear()
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
            if event.type() == QtCore.QEvent.MouseMove:
                tile_x = int(event.position().x() / Globals.TILE_WIDTH)
                tile_y = int(event.position().y() / Globals.TILE_HEIGHT)
                self.CurXPos.setText(str(tile_x))
                self.CurYPos.setText(str(tile_y))
            if event.type() == QtCore.QEvent.MouseButtonPress:
                tile_x = int(event.position().x() / Globals.TILE_WIDTH)
                tile_y = int(event.position().y() / Globals.TILE_HEIGHT)
                if event.button() == QtCore.Qt.LeftButton:
                    self.place_tile(tile_x, tile_y, Globals.CURRENT_TILE)
                elif event.button() == QtCore.Qt.RightButton:
                    # 'Place' a tile.  Using index -2 will actually clear the tile
                    self.place_tile(tile_x, tile_y, -2)
        return False

    def place_tile(self, tile_x, tile_y, idx):
        # Only place a tile if one is active.  In this case, a tile index of -1 
        # is the 'default' value of 'no tile'
        #
        # Note that -2 is used as a value to represent 'delete this tile'.
        if idx == -1:
            return

        # Add the tile index to the appropriate part of the map data
        # 
        # in the case of 'delete this tile' (index -2), reset the map entry
        # to 'no tile' (i.e. -1)
        if idx == -2:
            Globals.MAP_DATA[self.current_elevation][tile_x][tile_y] = -1
        else:
            Globals.MAP_DATA[self.current_elevation][tile_x][tile_y] = idx

        # Draw the current tile to the map area
        pixmap = self.EditorArea.pixmap()
        painter = QtGui.QPainter(pixmap)
        # If -2 ('delete this tile'), draw a filled rectangle in the place
        # where the tile would go.  Otherwise, draw the appropriate tile.
        if idx == -2:
            painter.fillRect(tile_x * Globals.TILE_WIDTH + 1, tile_y * Globals.TILE_HEIGHT + 1, 
                             Globals.TILE_WIDTH -1, Globals.TILE_HEIGHT - 1, QtGui.QColor(255, 255, 255)) 
        else:
            painter.drawImage(QtCore.QPoint(tile_x * Globals.TILE_WIDTH, tile_y * Globals.TILE_HEIGHT), Globals.TILE_DATA[idx][1])
        painter.end()
        self.EditorArea.setPixmap(pixmap)
        self.EditorArea.update()

        if idx >= 0:
            if idx not in Globals.TILES_USED:
                Globals.TILES_USED.append(idx)
                Globals.NUM_TILES_USED = Globals.NUM_TILES_USED + 1

        # If a tile was drawn or erased, the map has now been modified
        Globals.IS_MODIFIED = True

    def increment_floor(self):
        """Adds one to the currently active floor."""
        self.current_elevation = self.current_elevation + 1
        if self.current_elevation >= Globals.TILEMAP_LAYERS:
            self.current_elevation = Globals.TILEMAP_LAYERS - 1
        self.CurLevel.setText(str(self.current_elevation))
        self.render_map_area()

    def decrement_floor(self):
        """Subtracts one from the currently active floor."""
        self.current_elevation = self.current_elevation - 1
        if self.current_elevation < 0:
            self.current_elevation = 0
        self.CurLevel.setText(str(self.current_elevation))
        self.render_map_area()
    
    def render_map_area(self):
        """Draw the map area."""
        self.render_grid()
        self.render_tiles(self.current_elevation)
        self.EditorArea.update()

    def render_tiles(self, elevation):
        """Draws all tiles in the map area."""
        # Get the pixmap
        # Clear the map area
        # Draw the grid
        # Load the images into a list of [index, QImage]
        # For each tile location (x, y, elevation)
        #  Draw the tile where <index> is the value at Globals.MAP_DATA[elevation][x][y]
        pixmap = self.EditorArea.pixmap()
        painter = QtGui.QPainter(pixmap)
        for i in range(Globals.TILEMAP_WIDTH):
            for j in range(Globals.TILEMAP_HEIGHT):
                if Globals.MAP_DATA[self.current_elevation][i][j] >= 0:
                    painter.drawImage(QtCore.QPoint(i * Globals.TILE_WIDTH, j * Globals.TILE_HEIGHT), Globals.TILE_DATA[Globals.MAP_DATA[self.current_elevation][i][j]][1])
        painter.end()
        self.EditorArea.setPixmap(pixmap)
        self.EditorArea.update()
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
        idx = 0
        populate_res_file_struct()
        for key in Globals.RES_FILES.keys():
            self.PaletteEntryList.addItem(key)
            Globals.TILE_DATA.append([idx, QtGui.QImage(Globals.RES_PATH + "/" + Globals.RES_FILES[key]["texture"])])
            Globals.ISO_DATA.append([idx, QtGui.QImage(Globals.RES_PATH + "/" + Globals.RES_FILES[key]["iso_tile"])])
            Globals.DOS_FILES.append([idx, Globals.RES_FILES[key]["dosname"]])
            idx = idx + 1
            
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
        tex_file = Globals.TILE_DATA[row][1] # QtGui.QImage(Globals.RES_PATH + "/" + res_item["texture"])
        Globals.CURRENT_TILE = Globals.TILE_DATA[row][0]
        iso_tile_file = Globals.ISO_DATA[row][1] # QtGui.QImage(Globals.RES_PATH + "/" + res_item["iso_tile"])
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

    # Saving and loading make the following assumptions:
    #  - All maps are derived from the same set of base resources (though the
    #    resources actually used in a map can vary).  That is, all maps are
    #    created using the exact set of resource files in the map editor folder.
    #    When maps are loaded, they'll also assume that the resources are 
    #    all the same as they were when the file was saved, so if resources
    #    are added/deleted, they'll need to be updated and the map resaved.
    #  - All DOS filenames use the full 8.3 character name, for a total of 
    #    12 bytes per entry.  This value is ignored when loading.  

    def save_map_file(self, file_name):
        # Write the following data:
        #   Header (2 bytes)
        #   Map width (1 byte)
        #   Map height (1 byte)
        #   Number of map layers (1 byte)
        #   Number of tile entries (1 byte)
        #   <for each tile entry>
        #     <dos_filename> (12 bytes - all filenames are fully 8.3)
        #   [width * height * layers] tile entries (1 byte each)
        with open(file_name, "wb") as f:
            f.write(b'DI')
            f.write(struct.pack("BBBB", Globals.TILEMAP_WIDTH, Globals.TILEMAP_HEIGHT, Globals.TILEMAP_LAYERS, Globals.NUM_TILES_USED))
            for i in range(Globals.NUM_TILES_USED):
                # Map the tile index from the TILES_USED struct to the DOS filename of the tile
                # Write the filename to the file as a bytestring
                f.write(Globals.DOS_FILES[Globals.TILES_USED[i]][1].encode(encoding="ascii"))
                pass
            for z in range(Globals.TILEMAP_LAYERS):
                for x in range(Globals.TILEMAP_WIDTH):
                    for y in range(Globals.TILEMAP_HEIGHT):
                        f.write(struct.pack("b", Globals.MAP_DATA[z][x][y]))
        # The file is saved, reset the modified flag
        Globals.IS_MODIFIED = False
        self.set_title(file_name)

    def load_map_file(self, file_name):
        # Read the following data
        #   Header
        #   Map width (not used)
        #   Map height (not used)
        #   Number of map layers (not used)
        #   Number of tile entries
        #   <num tile entries of dos flienames>
        #  [width * height * layers] tile entries
        with open(file_name, "rb") as f:
            magic = f.read(2)
            if magic != b'DI':
                return
            _ = f.read(3)

            Globals.NUM_TILES_USED = int.from_bytes(f.read(1), byteorder=sys.byteorder)
            for i in range(Globals.NUM_TILES_USED):
                dos_file = f.read(12)
                Globals.DOS_FILES.append([i, str(dos_file)])
            for z in range(Globals.TILEMAP_LAYERS):
                for x in range(Globals.TILEMAP_WIDTH):
                    for y in range(Globals.TILEMAP_HEIGHT):
                        Globals.MAP_DATA[z][x][y] = int.from_bytes(f.read(1), byteorder=sys.byteorder, signed=True)

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)

    window = MainWindow()
    window.show()

    populate_res_file_struct()
    sys.exit(app.exec())
