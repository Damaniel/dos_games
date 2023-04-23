# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'form.ui'
##
## Created by: Qt User Interface Compiler version 6.4.3
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QApplication, QCheckBox, QGroupBox, QLabel,
    QLineEdit, QMainWindow, QMenu, QMenuBar,
    QRadioButton, QScrollArea, QSizePolicy, QStatusBar,
    QVBoxLayout, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(1450, 900)
        self.actionNew = QAction(MainWindow)
        self.actionNew.setObjectName(u"actionNew")
        self.actionOpen = QAction(MainWindow)
        self.actionOpen.setObjectName(u"actionOpen")
        self.actionSave = QAction(MainWindow)
        self.actionSave.setObjectName(u"actionSave")
        self.actionSaveAs = QAction(MainWindow)
        self.actionSaveAs.setObjectName(u"actionSaveAs")
        self.actionExport = QAction(MainWindow)
        self.actionExport.setObjectName(u"actionExport")
        self.actionClose = QAction(MainWindow)
        self.actionClose.setObjectName(u"actionClose")
        self.actionExit = QAction(MainWindow)
        self.actionExit.setObjectName(u"actionExit")
        self.Editor = QWidget(MainWindow)
        self.Editor.setObjectName(u"Editor")
        self.MapScrollArea = QScrollArea(self.Editor)
        self.MapScrollArea.setObjectName(u"MapScrollArea")
        self.MapScrollArea.setGeometry(QRect(0, 0, 841, 871))
        sizePolicy = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.MapScrollArea.sizePolicy().hasHeightForWidth())
        self.MapScrollArea.setSizePolicy(sizePolicy)
        self.MapScrollArea.setMinimumSize(QSize(0, 0))
        self.MapScrollArea.setWidgetResizable(True)
        self.MapScrollAreaContents = QWidget()
        self.MapScrollAreaContents.setObjectName(u"MapScrollAreaContents")
        self.MapScrollAreaContents.setGeometry(QRect(0, 0, 839, 869))
        self.verticalLayout = QVBoxLayout(self.MapScrollAreaContents)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.MapArea = QLabel(self.MapScrollAreaContents)
        self.MapArea.setObjectName(u"MapArea")
        self.MapArea.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.MapArea)

        self.MapScrollArea.setWidget(self.MapScrollAreaContents)
        self.SettingsBox = QGroupBox(self.Editor)
        self.SettingsBox.setObjectName(u"SettingsBox")
        self.SettingsBox.setGeometry(QRect(1110, 730, 331, 121))
        self.DisplayLayersCheckbox = QCheckBox(self.SettingsBox)
        self.DisplayLayersCheckbox.setObjectName(u"DisplayLayersCheckbox")
        self.DisplayLayersCheckbox.setGeometry(QRect(20, 30, 231, 20))
        self.DisplayLayersCheckbox.setChecked(True)
        self.MapWidthLabel = QLabel(self.SettingsBox)
        self.MapWidthLabel.setObjectName(u"MapWidthLabel")
        self.MapWidthLabel.setGeometry(QRect(20, 60, 81, 16))
        self.MapHeightLabel = QLabel(self.SettingsBox)
        self.MapHeightLabel.setObjectName(u"MapHeightLabel")
        self.MapHeightLabel.setGeometry(QRect(180, 60, 71, 16))
        self.PageWidthLabel = QLabel(self.SettingsBox)
        self.PageWidthLabel.setObjectName(u"PageWidthLabel")
        self.PageWidthLabel.setGeometry(QRect(20, 90, 71, 16))
        self.PageHeightLabel = QLabel(self.SettingsBox)
        self.PageHeightLabel.setObjectName(u"PageHeightLabel")
        self.PageHeightLabel.setGeometry(QRect(180, 90, 101, 16))
        self.MapWidth = QLineEdit(self.SettingsBox)
        self.MapWidth.setObjectName(u"MapWidth")
        self.MapWidth.setGeometry(QRect(90, 60, 41, 22))
        self.MapHeight = QLineEdit(self.SettingsBox)
        self.MapHeight.setObjectName(u"MapHeight")
        self.MapHeight.setGeometry(QRect(260, 60, 41, 22))
        self.PageWidth = QLineEdit(self.SettingsBox)
        self.PageWidth.setObjectName(u"PageWidth")
        self.PageWidth.setGeometry(QRect(90, 90, 41, 22))
        self.PageHeight = QLineEdit(self.SettingsBox)
        self.PageHeight.setObjectName(u"PageHeight")
        self.PageHeight.setGeometry(QRect(260, 90, 41, 22))
        self.ActiveLayerBox = QGroupBox(self.Editor)
        self.ActiveLayerBox.setObjectName(u"ActiveLayerBox")
        self.ActiveLayerBox.setGeometry(QRect(1240, 570, 151, 151))
        self.BackgroundRadio = QRadioButton(self.ActiveLayerBox)
        self.BackgroundRadio.setObjectName(u"BackgroundRadio")
        self.BackgroundRadio.setGeometry(QRect(20, 30, 89, 20))
        self.BackgroundRadio.setChecked(True)
        self.DecorationRadio = QRadioButton(self.ActiveLayerBox)
        self.DecorationRadio.setObjectName(u"DecorationRadio")
        self.DecorationRadio.setGeometry(QRect(20, 70, 89, 20))
        self.FlagsRadio = QRadioButton(self.ActiveLayerBox)
        self.FlagsRadio.setObjectName(u"FlagsRadio")
        self.FlagsRadio.setGeometry(QRect(20, 110, 89, 20))
        self.PaletteBox = QGroupBox(self.Editor)
        self.PaletteBox.setObjectName(u"PaletteBox")
        self.PaletteBox.setGeometry(QRect(870, 0, 531, 551))
        self.Palette = QLabel(self.PaletteBox)
        self.Palette.setObjectName(u"Palette")
        self.Palette.setGeometry(QRect(10, 25, 512, 512))
        self.Palette.setMinimumSize(QSize(512, 512))
        self.Palette.setAlignment(Qt.AlignCenter)
        self.LocationBox = QGroupBox(self.Editor)
        self.LocationBox.setObjectName(u"LocationBox")
        self.LocationBox.setGeometry(QRect(870, 560, 331, 161))
        self.PageXLabel = QLabel(self.LocationBox)
        self.PageXLabel.setObjectName(u"PageXLabel")
        self.PageXLabel.setGeometry(QRect(10, 60, 49, 16))
        self.PageYLabel = QLabel(self.LocationBox)
        self.PageYLabel.setObjectName(u"PageYLabel")
        self.PageYLabel.setGeometry(QRect(180, 60, 49, 16))
        self.PageTileXLabel = QLabel(self.LocationBox)
        self.PageTileXLabel.setObjectName(u"PageTileXLabel")
        self.PageTileXLabel.setGeometry(QRect(10, 90, 61, 16))
        self.PageTileYLabel = QLabel(self.LocationBox)
        self.PageTileYLabel.setObjectName(u"PageTileYLabel")
        self.PageTileYLabel.setGeometry(QRect(180, 90, 61, 16))
        self.MapXLabel = QLabel(self.LocationBox)
        self.MapXLabel.setObjectName(u"MapXLabel")
        self.MapXLabel.setGeometry(QRect(10, 30, 49, 16))
        self.MapYLabel = QLabel(self.LocationBox)
        self.MapYLabel.setObjectName(u"MapYLabel")
        self.MapYLabel.setGeometry(QRect(180, 30, 49, 16))
        self.MapX = QLineEdit(self.LocationBox)
        self.MapX.setObjectName(u"MapX")
        self.MapX.setGeometry(QRect(80, 30, 61, 22))
        self.MapX.setReadOnly(True)
        self.PageX = QLineEdit(self.LocationBox)
        self.PageX.setObjectName(u"PageX")
        self.PageX.setGeometry(QRect(80, 60, 61, 22))
        self.PageX.setReadOnly(True)
        self.PageTileX = QLineEdit(self.LocationBox)
        self.PageTileX.setObjectName(u"PageTileX")
        self.PageTileX.setGeometry(QRect(80, 90, 61, 22))
        self.PageTileX.setReadOnly(True)
        self.MapY = QLineEdit(self.LocationBox)
        self.MapY.setObjectName(u"MapY")
        self.MapY.setGeometry(QRect(250, 30, 61, 22))
        self.MapY.setReadOnly(True)
        self.PageY = QLineEdit(self.LocationBox)
        self.PageY.setObjectName(u"PageY")
        self.PageY.setGeometry(QRect(250, 60, 61, 22))
        self.PageY.setReadOnly(True)
        self.PageTileY = QLineEdit(self.LocationBox)
        self.PageTileY.setObjectName(u"PageTileY")
        self.PageTileY.setGeometry(QRect(250, 90, 61, 22))
        self.PageTileY.setReadOnly(True)
        self.ImpassableStatusCheck = QCheckBox(self.LocationBox)
        self.ImpassableStatusCheck.setObjectName(u"ImpassableStatusCheck")
        self.ImpassableStatusCheck.setGeometry(QRect(10, 130, 81, 20))
        self.TeleportStatusCheck = QCheckBox(self.LocationBox)
        self.TeleportStatusCheck.setObjectName(u"TeleportStatusCheck")
        self.TeleportStatusCheck.setGeometry(QRect(130, 130, 76, 20))
        self.TBDStatusCheck = QCheckBox(self.LocationBox)
        self.TBDStatusCheck.setObjectName(u"TBDStatusCheck")
        self.TBDStatusCheck.setGeometry(QRect(240, 130, 76, 20))
        self.FlagsBox = QGroupBox(self.Editor)
        self.FlagsBox.setObjectName(u"FlagsBox")
        self.FlagsBox.setGeometry(QRect(870, 730, 221, 121))
        self.ImpassableCheck = QCheckBox(self.FlagsBox)
        self.ImpassableCheck.setObjectName(u"ImpassableCheck")
        self.ImpassableCheck.setGeometry(QRect(20, 30, 81, 20))
        self.TeleportCheck = QCheckBox(self.FlagsBox)
        self.TeleportCheck.setObjectName(u"TeleportCheck")
        self.TeleportCheck.setGeometry(QRect(20, 60, 76, 20))
        self.MapID = QLineEdit(self.FlagsBox)
        self.MapID.setObjectName(u"MapID")
        self.MapID.setGeometry(QRect(170, 60, 41, 22))
        self.MapIDLabel = QLabel(self.FlagsBox)
        self.MapIDLabel.setObjectName(u"MapIDLabel")
        self.MapIDLabel.setGeometry(QRect(120, 60, 41, 20))
        self.TBDCheck = QCheckBox(self.FlagsBox)
        self.TBDCheck.setObjectName(u"TBDCheck")
        self.TBDCheck.setGeometry(QRect(20, 90, 76, 20))
        MainWindow.setCentralWidget(self.Editor)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 1450, 22))
        self.menuFile = QMenu(self.menubar)
        self.menuFile.setObjectName(u"menuFile")
        self.menuEdit = QMenu(self.menubar)
        self.menuEdit.setObjectName(u"menuEdit")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuEdit.menuAction())
        self.menuFile.addAction(self.actionNew)
        self.menuFile.addAction(self.actionOpen)
        self.menuFile.addSeparator()
        self.menuFile.addAction(self.actionSave)
        self.menuFile.addAction(self.actionSaveAs)
        self.menuFile.addAction(self.actionExport)
        self.menuFile.addSeparator()
        self.menuFile.addAction(self.actionClose)
        self.menuFile.addAction(self.actionExit)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"DamNGU Map Editor", None))
        self.actionNew.setText(QCoreApplication.translate("MainWindow", u"New", None))
        self.actionOpen.setText(QCoreApplication.translate("MainWindow", u"Open", None))
        self.actionSave.setText(QCoreApplication.translate("MainWindow", u"Save", None))
        self.actionSaveAs.setText(QCoreApplication.translate("MainWindow", u"Save As...", None))
        self.actionExport.setText(QCoreApplication.translate("MainWindow", u"Export", None))
        self.actionClose.setText(QCoreApplication.translate("MainWindow", u"Close", None))
        self.actionExit.setText(QCoreApplication.translate("MainWindow", u"Exit", None))
        self.MapArea.setText(QCoreApplication.translate("MainWindow", u"Map", None))
        self.SettingsBox.setTitle(QCoreApplication.translate("MainWindow", u"Settings", None))
        self.DisplayLayersCheckbox.setText(QCoreApplication.translate("MainWindow", u"Display both layers", None))
        self.MapWidthLabel.setText(QCoreApplication.translate("MainWindow", u"Map width", None))
        self.MapHeightLabel.setText(QCoreApplication.translate("MainWindow", u"Map height", None))
        self.PageWidthLabel.setText(QCoreApplication.translate("MainWindow", u"Page width", None))
        self.PageHeightLabel.setText(QCoreApplication.translate("MainWindow", u"Page height", None))
        self.ActiveLayerBox.setTitle(QCoreApplication.translate("MainWindow", u"Active Layer", None))
        self.BackgroundRadio.setText(QCoreApplication.translate("MainWindow", u"Background", None))
        self.DecorationRadio.setText(QCoreApplication.translate("MainWindow", u"Decoration", None))
        self.FlagsRadio.setText(QCoreApplication.translate("MainWindow", u"Flags", None))
        self.PaletteBox.setTitle(QCoreApplication.translate("MainWindow", u"Palette", None))
        self.Palette.setText(QCoreApplication.translate("MainWindow", u"Palette", None))
        self.LocationBox.setTitle(QCoreApplication.translate("MainWindow", u"Location", None))
        self.PageXLabel.setText(QCoreApplication.translate("MainWindow", u"Page X", None))
        self.PageYLabel.setText(QCoreApplication.translate("MainWindow", u"Page Y", None))
        self.PageTileXLabel.setText(QCoreApplication.translate("MainWindow", u"Page Tile X", None))
        self.PageTileYLabel.setText(QCoreApplication.translate("MainWindow", u"Page Tile Y", None))
        self.MapXLabel.setText(QCoreApplication.translate("MainWindow", u"Map X", None))
        self.MapYLabel.setText(QCoreApplication.translate("MainWindow", u"Map Y", None))
        self.ImpassableStatusCheck.setText(QCoreApplication.translate("MainWindow", u"Impassable", None))
        self.TeleportStatusCheck.setText(QCoreApplication.translate("MainWindow", u"Teleport ", None))
        self.TBDStatusCheck.setText(QCoreApplication.translate("MainWindow", u"TBD", None))
        self.FlagsBox.setTitle(QCoreApplication.translate("MainWindow", u"Set Flags", None))
        self.ImpassableCheck.setText(QCoreApplication.translate("MainWindow", u"Impassable", None))
        self.TeleportCheck.setText(QCoreApplication.translate("MainWindow", u"Teleport", None))
        self.MapIDLabel.setText(QCoreApplication.translate("MainWindow", u"Map ID", None))
        self.TBDCheck.setText(QCoreApplication.translate("MainWindow", u"TBD", None))
        self.menuFile.setTitle(QCoreApplication.translate("MainWindow", u"File", None))
        self.menuEdit.setTitle(QCoreApplication.translate("MainWindow", u"Edit", None))
    # retranslateUi

