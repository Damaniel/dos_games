# This Python file uses the following encoding: utf-8
import sys

from PySide6.QtWidgets import QApplication, QWidget
from PySide6 import QtGui, QtCore

DefaultPageWidth = 14
DefaultPageHeight = 10
DefaultMapWidth = DefaultPageWidth * 10
DefaultMapHeight = DefaultPageHeight * 10
DefaultTileSize = 32

DefaultTilemapWidth = 16

BackgroundLayerTiles = []
DecorationLayerTiles = []

# Important:
# You need to run the following command to generate the ui_form.py file
#     pyside6-uic form.ui -o ui_form.py, or
#     pyside2-uic form.ui -o ui_form.py
from ui_form import Ui_Widget

class Widget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)
        self.setWindowTitle("DamNGU Map Editor")

        self.page_width = DefaultPageWidth;
        self.page_height = DefaultPageHeight;
        self.map_width = DefaultMapWidth
        self.map_height = DefaultMapHeight
        self.page_x = 0
        self.page_y = 0
        self.map_x = 0
        self.map_y = 0
        self.page_tile_x = 0
        self.page_tile_y = 0

        self.display_both_layers = False
        # 0 is the bottom layer, 1 is the top layer
        self.display_active_layer = 0

        self.palette_index = 0

        self.palette_image = QtGui.QImage("res/palette.png")

        self.initialize_map_area_pixmap()
        self.palette_pixmap = QtGui.QPixmap(512, 512)
        self.ui.Palette.setPixmap(self.palette_pixmap)

        self.ui.MapArea.installEventFilter(self)
        self.ui.Palette.installEventFilter(self)

        # Any time a change is made to selected layer or layer visibility, redraw all the tiles
        self.ui.DecorationRadio.clicked.connect(self.layer_ui_update)
        self.ui.BackgroundRadio.clicked.connect(self.layer_ui_update)
        self.ui.DisplayLayersCheckbox.clicked.connect(self.layer_ui_update)

        # Any time a change is made to the map or page size, do stuff
        self.ui.MapWidth.editingFinished.connect(self.map_width_update)
        self.ui.MapHeight.editingFinished.connect(self.map_height_update)
        self.ui.PageWidth.editingFinished.connect(self.page_width_update)
        self.ui.PageHeight.editingFinished.connect(self.page_height_update)

        # Set mouse tracking on the map screen
        self.ui.MapArea.setMouseTracking(True)

        # Clear the map data
        self.initialize_map_data()

        self.render_ui()

    def map_width_update(self):
        print(f"Map width is now {int(self.ui.MapWidth.text())}")
        self.map_width = int(self.ui.MapWidth.text())
        self.initialize_map_area_pixmap()
        self.initialize_map_data()
        self.layer_ui_update()

    def map_height_update(self):
        print(f"Map height is now {int(self.ui.MapHeight.text())}")
        self.map_height = int(self.ui.MapHeight.text())
        self.initialize_map_area_pixmap()
        self.initialize_map_data()
        self.layer_ui_update()

    def page_width_update(self):
        print(f"Page width is now {int(self.ui.PageWidth.text())}")
        self.page_width = int(self.ui.PageWidth.text())
        self.initialize_map_area_pixmap()
        self.initialize_map_data()
        self.layer_ui_update()

    def page_height_update(self):
        print(f"Page height is now {int(self.ui.PageHeight.text())}")
        self.page_height = int(self.ui.PageHeight.text())
        self.initialize_map_area_pixmap()
        self.initialize_map_data()
        self.layer_ui_update()

    def initialize_map_area_pixmap(self):
        self.map_pixmap = QtGui.QPixmap(self.map_width * DefaultTileSize, self.map_height * DefaultTileSize)
        self.ui.MapArea.setPixmap(self.map_pixmap)

    def layer_ui_update(self):
        # Just re-render all the tiles in the map with appropriate layer visibility
        self.render_all_tiles()

    def eventFilter(self, object, event):
        if event.type() == QtCore.QEvent.MouseButtonPress:
            x = int(event.position().x() / DefaultTileSize)
            y = int(event.position().y() / DefaultTileSize)
            if object == self.ui.MapArea:
                if event.button() == QtCore.Qt.RightButton:
                    self.set_tile(x, y, -1)
                    self.render_tile(x, y)
                else:
                    self.set_tile(x, y, self.palette_index)
                    self.render_tile(x, y)
            if object == self.ui.Palette:
                self.palette_index = y * DefaultTilemapWidth + x
                self.render_palette(self.palette_index)
        if event.type() == QtCore.QEvent.MouseMove:
            x = int(event.position().x() / DefaultTileSize)
            y = int(event.position().y() / DefaultTileSize)
            if object == self.ui.MapArea:
                self.map_x = x
                self.map_y = y
                self.page_x = int(self.map_x / self.page_width)
                self.page_y = int(self.map_y / self.page_height)
                self.page_tile_x = int(self.map_x - (self.page_x * self.page_width))
                self.page_tile_y = int(self.map_y - (self.page_y * self.page_height))
                self.update_location_info(self.map_x, self.map_y, self.page_x, self.page_y, self.page_tile_x, self.page_tile_y)

        return False

    def initialize_map_data(self):
        BackgroundLayerTiles.clear()
        DecorationLayerTiles.clear()
        for i in range(self.map_width):
            back_row = []
            dec_row = []
            for j in range(self.map_height):
                back_row.append(-1)
                dec_row.append(-1)
            BackgroundLayerTiles.append(back_row)
            DecorationLayerTiles.append(dec_row)

    def set_tile(self, x, y, index):
        if self.ui.BackgroundRadio.isChecked():
            BackgroundLayerTiles[x][y] = index
        if self.ui.DecorationRadio.isChecked():
            DecorationLayerTiles[x][y] = index

    def render_tile(self, x, y):
        pixmap = self.ui.MapArea.pixmap()
        painter = QtGui.QPainter(pixmap)
        self.render_tile_on_pixmap(x, y, painter)    
        painter.end()
        self.ui.MapArea.setPixmap(pixmap)
        self.ui.MapArea.update()

    def render_all_tiles(self):
        pixmap = self.ui.MapArea.pixmap()
        painter = QtGui.QPainter(pixmap)
        for x in range(self.map_width):
            for y in range(self.map_height):
                self.render_tile_on_pixmap(x, y, painter)
        painter.end()
        self.ui.MapArea.setPixmap(pixmap)
        self.ui.MapArea.update()

    def render_tile_on_pixmap(self, x, y, painter):
        p_base = BackgroundLayerTiles[x][y]
        p_dec = DecorationLayerTiles[x][y]

        # Clear the tile space entirely
        # Erase the tile square
        painter.fillRect(x * DefaultTileSize, y * DefaultTileSize, DefaultTileSize, DefaultTileSize, QtGui.QColor(255, 255, 255))
            
        # Redraw the lines on the top and left sides of the square.  Note that either line could be on a page boundary, so
        # pick the pen color appropriately for each side
        # Top line
        gray = QtGui.QColor(192, 192, 192)
        black = QtGui.QColor(0, 0, 0)
        if y % self.page_height == 0:
            painter.setPen(black)
        else:
            painter.setPen(gray)
        painter.drawLine(x * DefaultTileSize, y * DefaultTileSize, (x + 1) * DefaultTileSize, y * DefaultTileSize)
        # Left line
        if x % self.page_width == 0:
            painter.setPen(black)
        else:
            painter.setPen(gray)
        painter.drawLine(x * DefaultTileSize, y * DefaultTileSize, x * DefaultTileSize, (y + 1) * DefaultTileSize)

        # if background or both, draw background
        # if foreground or both, draw foreground
        if p_base != -1:
            if self.ui.BackgroundRadio.isChecked() or self.ui.DisplayLayersCheckbox.isChecked():
                painter.drawImage(x * DefaultTileSize, y * DefaultTileSize, self.palette_image, (p_base % DefaultTilemapWidth) * DefaultTileSize, int(p_base / DefaultTilemapWidth) * DefaultTileSize, DefaultTileSize, DefaultTileSize)
        if p_dec != -1:
            if self.ui.DecorationRadio.isChecked() or self.ui.DisplayLayersCheckbox.isChecked():
                painter.drawImage(x * DefaultTileSize, y * DefaultTileSize, self.palette_image, (p_dec % DefaultTilemapWidth) * DefaultTileSize, int(p_dec / DefaultTilemapWidth) * DefaultTileSize, DefaultTileSize, DefaultTileSize)
            
    def update_location_info(self, mx, my, px, py, ptx, pty):
        self.ui.MapX.setText(str(mx))
        self.ui.MapY.setText(str(my))
        self.ui.PageX.setText(str(px))
        self.ui.PageY.setText(str(py))
        self.ui.PageTileX.setText(str(ptx))
        self.ui.PageTileY.setText(str(pty))

    def update_map_size_info(self):
        self.ui.MapWidth.setText(str(self.map_width))
        self.ui.MapHeight.setText(str(self.map_height))
        self.ui.PageWidth.setText(str(self.page_width))
        self.ui.PageHeight.setText(str(self.page_height))

    def render_map(self):
        self.render_grid()
        self.render_all_tiles()

    def render_grid(self):
        pixmap = self.ui.MapArea.pixmap()
        gray = QtGui.QColor(192, 192, 192)
        black = QtGui.QColor(0, 0, 0)
        pixmap.fill(QtGui.QColor(255, 255, 255))
        painter = QtGui.QPainter(pixmap)
        painter.setPen(gray)

        # Draw the vertical lines
        for x in range(0, self.map_width):
            painter.drawLine(x * DefaultTileSize, 0, x * DefaultTileSize, self.map_height * DefaultTileSize - 1)

        # Draw the horizontal lines
        for y in range(0, self.map_height):
            painter.drawLine(0, y * DefaultTileSize, self.map_width * DefaultTileSize - 1, y * DefaultTileSize)

        # Draw the lower and right corner lines if they don't end on a page boundary
        if (self.map_width % self.page_width != 0):
            pass
        if (self.map_height % self.page_height != 0):
            pass

        # Switch to black
        painter.setPen(black)
        # Draw the page boundaries with darker lines
        for x in range(0, int(self.map_width / self.page_width) ):
            painter.drawLine(x * self.page_width * DefaultTileSize, 0, x * self.page_width * DefaultTileSize, self.map_height * DefaultTileSize - 1)

        for y in range(0, int(self.map_height / self.page_height)):
            painter.drawLine(0, y * self.page_height * DefaultTileSize, self.map_width * DefaultTileSize - 1, y * self.page_height * DefaultTileSize)

        # Draw the lower and right corner lines if they do end on a page boundary
        if (self.map_width % self.page_width == 0):
            pass
        if (self.map_height % self.page_height == 0):
            pass

        painter.end()
        self.ui.MapArea.setPixmap(pixmap)

    def render_ui(self):
        self.render_map()
        self.render_palette(self.palette_index)
        self.update_location_info(self.map_x, self.map_y, self.page_x, self.page_y, self.page_tile_x, self.page_tile_y)
        self.update_map_size_info()

    def render_palette(self, idx):
        x_off = (idx % DefaultTilemapWidth) * DefaultTileSize
        y_off = int(idx / DefaultTilemapWidth) * DefaultTileSize
        
        # Draw the palette
        pixmap = self.ui.Palette.pixmap()
        pixmap.fill(QtGui.QColor(0, 0, 0))
        painter = QtGui.QPainter(pixmap)

        painter.drawImage(QtCore.QPoint(0, 0), self.palette_image)

        # Draw a box around the currently active item in the palette
        yellow = QtGui.QColor(255, 255, 0)
        pen = QtGui.QPen(yellow, 2)
        painter.setPen(pen)

        painter.drawRect(x_off - 1, y_off - 1, DefaultTileSize + 2, DefaultTileSize + 2)

        painter.end()
        self.ui.Palette.setPixmap(pixmap)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = Widget()
    widget.show()
    sys.exit(app.exec())
