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

        self.palette_index = 0

        self.palette_image = QtGui.QImage("res/palette.png")

        self.map_pixmap = QtGui.QPixmap(self.map_width * DefaultTileSize, self.map_height * DefaultTileSize)
        self.ui.MapArea.setPixmap(self.map_pixmap)
        self.palette_pixmap = QtGui.QPixmap(512, 512)
        self.ui.Palette.setPixmap(self.palette_pixmap)

        self.ui.MapArea.installEventFilter(self)
        self.ui.Palette.installEventFilter(self)

        # Set mouse tracking on the map screen
        self.ui.MapArea.setMouseTracking(True)
        self.render_ui()

    def eventFilter(self, object, event):
        if event.type() == QtCore.QEvent.MouseButtonPress:
            x = int(event.position().x() / DefaultTileSize)
            y = int(event.position().y() / DefaultTileSize)
            if object == self.ui.MapArea:
                if event.button() == QtCore.Qt.RightButton:
                    self.render_tile(x, y, -1)
                else:
                    self.render_tile(x, y, self.palette_index)
            if object == self.ui.Palette:
                self.palette_index = y * DefaultTilemapWidth + x
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

    def render_tile(self, x, y, index):
        pixmap = self.ui.MapArea.pixmap()
        painter = QtGui.QPainter(pixmap)
        if index == -1:
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

        else:
            p = self.palette_index
            painter.drawImage(x * DefaultTileSize, y * DefaultTileSize, self.palette_image, (p % DefaultTilemapWidth) * DefaultTileSize, int(p / DefaultTilemapWidth) * DefaultTileSize, DefaultTileSize, DefaultTileSize)
            #painter.drawImage()
            pass 
        painter.end()
        self.ui.MapArea.setPixmap(pixmap)
        self.ui.MapArea.update()

    def update_location_info(self, mx, my, px, py, ptx, pty):
        self.ui.MapX.setText(str(mx))
        self.ui.MapY.setText(str(my))
        self.ui.PageX.setText(str(px))
        self.ui.PageY.setText(str(py))
        self.ui.PageTileX.setText(str(ptx))
        self.ui.PageTileY.setText(str(pty))

    def render_map(self):
        self.render_grid()

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

    def render_palette(self, idx):
        x_off = idx % DefaultTilemapWidth * DefaultTileSize
        y_off = idx / DefaultTilemapWidth * DefaultTileSize
        
        # Draw the palette
        pixmap = self.ui.Palette.pixmap()
        pixmap.fill(QtGui.QColor(0, 0, 0))
        painter = QtGui.QPainter(pixmap)

        painter.drawImage(QtCore.QPoint(0, 0), self.palette_image)

        # Draw a box around the currently active item in the palette

        painter.end()
        self.ui.Palette.setPixmap(pixmap)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = Widget()
    widget.show()
    sys.exit(app.exec())
