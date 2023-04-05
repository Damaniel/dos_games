# This Python file uses the following encoding: utf-8
import sys

from PySide6.QtWidgets import QApplication, QWidget
from PySide6 import QtGui, QtCore

DefaultPageWidth = 14
DefaultPageHeight = 10
DefaultMapWidth = DefaultPageWidth * 20
DefaultMapHeight = DefaultPageHeight * 20
DefaultTileSize = 32


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

        self.palette_index = 0

        self.palette_image = QtGui.QImage("res/palette.png")

        self.map_pixmap = QtGui.QPixmap(self.map_width * DefaultTileSize, self.map_height * DefaultTileSize)
        self.ui.MapArea.setPixmap(self.map_pixmap)
        self.palette_pixmap = QtGui.QPixmap(512, 512)
        self.ui.Palette.setPixmap(self.palette_pixmap)

        self.render_ui()

    def render_map(self):
        self.render_grid()

    def render_grid(self):
        pixmap = self.ui.MapArea.pixmap()
        gray = QtGui.QColor(128, 128, 128)
        pixmap.fill(QtGui.QColor(255, 255, 255))
        painter = QtGui.QPainter(pixmap)
        painter.setPen(gray)

        # Draw the vertical lines
        for x in range(0, self.map_width):
            painter.drawLine(x * DefaultTileSize, 0, x * DefaultTileSize, self.map_height * DefaultTileSize - 1)

        # Draw the horizontal lines
        for y in range(0, self.map_height):
            painter.drawLine(0, y * DefaultTileSize, self.map_width * DefaultTileSize - 1, y * DefaultTileSize)

        painter.end()
        self.ui.MapArea.setPixmap(pixmap)

    def render_ui(self):
        self.render_map()
        self.render_palette(self.palette_index)

    def render_palette(self, idx):
        x_off = idx % 16 * DefaultTileSize
        y_off = idx / 16 * DefaultTileSize
        
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
