# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'form.ui'
##
## Created by: Qt User Interface Compiler version 6.4.2
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
from PySide6.QtWidgets import (QApplication, QFrame, QGroupBox, QHBoxLayout,
    QLabel, QLineEdit, QListWidget, QListWidgetItem,
    QMainWindow, QMenu, QMenuBar, QPushButton,
    QScrollArea, QSizePolicy, QStatusBar, QTabWidget,
    QVBoxLayout, QWidget)

class Ui_DruIsoMapEdit(object):
    def setupUi(self, DruIsoMapEdit):
        if not DruIsoMapEdit.objectName():
            DruIsoMapEdit.setObjectName(u"DruIsoMapEdit")
        DruIsoMapEdit.resize(1024, 768)
        DruIsoMapEdit.setMinimumSize(QSize(1024, 768))
        DruIsoMapEdit.setMaximumSize(QSize(1024, 768))
        self.actionNew = QAction(DruIsoMapEdit)
        self.actionNew.setObjectName(u"actionNew")
        self.actionOpen = QAction(DruIsoMapEdit)
        self.actionOpen.setObjectName(u"actionOpen")
        self.actionSave = QAction(DruIsoMapEdit)
        self.actionSave.setObjectName(u"actionSave")
        self.actionSave_As = QAction(DruIsoMapEdit)
        self.actionSave_As.setObjectName(u"actionSave_As")
        self.actionExit = QAction(DruIsoMapEdit)
        self.actionExit.setObjectName(u"actionExit")
        self.actionExit_2 = QAction(DruIsoMapEdit)
        self.actionExit_2.setObjectName(u"actionExit_2")
        self.actionHelp = QAction(DruIsoMapEdit)
        self.actionHelp.setObjectName(u"actionHelp")
        self.actionAbout = QAction(DruIsoMapEdit)
        self.actionAbout.setObjectName(u"actionAbout")
        self.actionUndo = QAction(DruIsoMapEdit)
        self.actionUndo.setObjectName(u"actionUndo")
        self.actionPreferences = QAction(DruIsoMapEdit)
        self.actionPreferences.setObjectName(u"actionPreferences")
        self.centralwidget = QWidget(DruIsoMapEdit)
        self.centralwidget.setObjectName(u"centralwidget")
        self.verticalLayout = QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.MainTabs = QTabWidget(self.centralwidget)
        self.MainTabs.setObjectName(u"MainTabs")
        self.EditorTab = QWidget()
        self.EditorTab.setObjectName(u"EditorTab")
        self.horizontalLayout = QHBoxLayout(self.EditorTab)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.PaletteFrame = QFrame(self.EditorTab)
        self.PaletteFrame.setObjectName(u"PaletteFrame")
        self.PaletteFrame.setFrameShape(QFrame.StyledPanel)
        self.PaletteFrame.setFrameShadow(QFrame.Raised)
        self.EditorScrollArea = QScrollArea(self.PaletteFrame)
        self.EditorScrollArea.setObjectName(u"EditorScrollArea")
        self.EditorScrollArea.setGeometry(QRect(19, 19, 651, 591))
        sizePolicy = QSizePolicy(QSizePolicy.Maximum, QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.EditorScrollArea.sizePolicy().hasHeightForWidth())
        self.EditorScrollArea.setSizePolicy(sizePolicy)
        self.EditorScrollArea.setWidgetResizable(True)
        self.EditorScrollWidget = QWidget()
        self.EditorScrollWidget.setObjectName(u"EditorScrollWidget")
        self.EditorScrollWidget.setGeometry(QRect(0, 0, 649, 589))
        self.verticalLayout_2 = QVBoxLayout(self.EditorScrollWidget)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.EditorArea = QLabel(self.EditorScrollWidget)
        self.EditorArea.setObjectName(u"EditorArea")
        sizePolicy1 = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.EditorArea.sizePolicy().hasHeightForWidth())
        self.EditorArea.setSizePolicy(sizePolicy1)
        self.EditorArea.setAlignment(Qt.AlignCenter)

        self.verticalLayout_2.addWidget(self.EditorArea)

        self.EditorScrollArea.setWidget(self.EditorScrollWidget)
        self.PaletteEntryList = QListWidget(self.PaletteFrame)
        self.PaletteEntryList.setObjectName(u"PaletteEntryList")
        self.PaletteEntryList.setGeometry(QRect(690, 20, 261, 271))
        self.TexPreviewArea = QGroupBox(self.PaletteFrame)
        self.TexPreviewArea.setObjectName(u"TexPreviewArea")
        self.TexPreviewArea.setGeometry(QRect(690, 440, 261, 211))
        sizePolicy.setHeightForWidth(self.TexPreviewArea.sizePolicy().hasHeightForWidth())
        self.TexPreviewArea.setSizePolicy(sizePolicy)
        self.verticalLayout_3 = QVBoxLayout(self.TexPreviewArea)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.TexPreview = QLabel(self.TexPreviewArea)
        self.TexPreview.setObjectName(u"TexPreview")
        sizePolicy1.setHeightForWidth(self.TexPreview.sizePolicy().hasHeightForWidth())
        self.TexPreview.setSizePolicy(sizePolicy1)
        self.TexPreview.setAlignment(Qt.AlignCenter)

        self.verticalLayout_3.addWidget(self.TexPreview)

        self.PalItemDetails = QGroupBox(self.PaletteFrame)
        self.PalItemDetails.setObjectName(u"PalItemDetails")
        self.PalItemDetails.setGeometry(QRect(690, 300, 261, 131))
        self.PalItemNameLabel = QLabel(self.PalItemDetails)
        self.PalItemNameLabel.setObjectName(u"PalItemNameLabel")
        self.PalItemNameLabel.setGeometry(QRect(10, 20, 49, 16))
        self.PalItemDescriptionLabel = QLabel(self.PalItemDetails)
        self.PalItemDescriptionLabel.setObjectName(u"PalItemDescriptionLabel")
        self.PalItemDescriptionLabel.setGeometry(QRect(10, 70, 71, 21))
        self.PalItemDescription = QLineEdit(self.PalItemDetails)
        self.PalItemDescription.setObjectName(u"PalItemDescription")
        self.PalItemDescription.setGeometry(QRect(10, 100, 241, 22))
        self.PalItemName = QLineEdit(self.PalItemDetails)
        self.PalItemName.setObjectName(u"PalItemName")
        self.PalItemName.setGeometry(QRect(10, 40, 241, 22))
        self.UpButton = QPushButton(self.PaletteFrame)
        self.UpButton.setObjectName(u"UpButton")
        self.UpButton.setGeometry(QRect(180, 630, 101, 24))
        self.DownButton = QPushButton(self.PaletteFrame)
        self.DownButton.setObjectName(u"DownButton")
        self.DownButton.setGeometry(QRect(20, 630, 101, 24))
        self.CurLevel = QLineEdit(self.PaletteFrame)
        self.CurLevel.setObjectName(u"CurLevel")
        self.CurLevel.setGeometry(QRect(130, 630, 41, 21))
        self.CurLevel.setReadOnly(True)
        self.CurXPos = QLineEdit(self.PaletteFrame)
        self.CurXPos.setObjectName(u"CurXPos")
        self.CurXPos.setGeometry(QRect(480, 630, 31, 22))
        self.CurXPos.setReadOnly(True)
        self.CurYPos = QLineEdit(self.PaletteFrame)
        self.CurYPos.setObjectName(u"CurYPos")
        self.CurYPos.setGeometry(QRect(520, 630, 31, 21))
        self.CurYPos.setReadOnly(True)
        self.CurTileName = QLineEdit(self.PaletteFrame)
        self.CurTileName.setObjectName(u"CurTileName")
        self.CurTileName.setGeometry(QRect(560, 630, 113, 22))
        self.CurTileName.setReadOnly(True)

        self.horizontalLayout.addWidget(self.PaletteFrame)

        self.MainTabs.addTab(self.EditorTab, "")
        self.PreviewTab = QWidget()
        self.PreviewTab.setObjectName(u"PreviewTab")
        sizePolicy1.setHeightForWidth(self.PreviewTab.sizePolicy().hasHeightForWidth())
        self.PreviewTab.setSizePolicy(sizePolicy1)
        self.PreviewFrame = QFrame(self.PreviewTab)
        self.PreviewFrame.setObjectName(u"PreviewFrame")
        self.PreviewFrame.setGeometry(QRect(9, 9, 981, 611))
        sizePolicy1.setHeightForWidth(self.PreviewFrame.sizePolicy().hasHeightForWidth())
        self.PreviewFrame.setSizePolicy(sizePolicy1)
        self.PreviewFrame.setFrameShape(QFrame.StyledPanel)
        self.PreviewFrame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_4 = QVBoxLayout(self.PreviewFrame)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.PreviewArea = QLabel(self.PreviewFrame)
        self.PreviewArea.setObjectName(u"PreviewArea")
        self.PreviewArea.setAlignment(Qt.AlignCenter)

        self.verticalLayout_4.addWidget(self.PreviewArea)

        self.PositionalFrame = QFrame(self.PreviewTab)
        self.PositionalFrame.setObjectName(u"PositionalFrame")
        self.PositionalFrame.setGeometry(QRect(9, 630, 981, 41))
        self.PositionalFrame.setFrameShape(QFrame.StyledPanel)
        self.PositionalFrame.setFrameShadow(QFrame.Raised)
        self.XPosLabel = QLabel(self.PositionalFrame)
        self.XPosLabel.setObjectName(u"XPosLabel")
        self.XPosLabel.setGeometry(QRect(150, 10, 71, 21))
        self.XPosLabel.setAlignment(Qt.AlignCenter)
        self.XPosEdit = QLineEdit(self.PositionalFrame)
        self.XPosEdit.setObjectName(u"XPosEdit")
        self.XPosEdit.setGeometry(QRect(230, 10, 113, 22))
        self.XPosEdit.setReadOnly(True)
        self.YPosLabel = QLabel(self.PositionalFrame)
        self.YPosLabel.setObjectName(u"YPosLabel")
        self.YPosLabel.setGeometry(QRect(360, 11, 71, 20))
        self.YPosLabel.setAlignment(Qt.AlignCenter)
        self.YPosEdit = QLineEdit(self.PositionalFrame)
        self.YPosEdit.setObjectName(u"YPosEdit")
        self.YPosEdit.setGeometry(QRect(440, 11, 113, 21))
        self.YPosEdit.setReadOnly(True)
        self.ElevationLabel = QLabel(self.PositionalFrame)
        self.ElevationLabel.setObjectName(u"ElevationLabel")
        self.ElevationLabel.setGeometry(QRect(570, 10, 71, 21))
        self.ElevationLabel.setAlignment(Qt.AlignCenter)
        self.ElevationEdit = QLineEdit(self.PositionalFrame)
        self.ElevationEdit.setObjectName(u"ElevationEdit")
        self.ElevationEdit.setGeometry(QRect(650, 11, 113, 21))
        self.ElevationEdit.setReadOnly(True)
        self.MainTabs.addTab(self.PreviewTab, "")

        self.verticalLayout.addWidget(self.MainTabs)

        DruIsoMapEdit.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(DruIsoMapEdit)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 1024, 22))
        self.menuFile = QMenu(self.menubar)
        self.menuFile.setObjectName(u"menuFile")
        self.menuEdit = QMenu(self.menubar)
        self.menuEdit.setObjectName(u"menuEdit")
        self.menuHelp = QMenu(self.menubar)
        self.menuHelp.setObjectName(u"menuHelp")
        DruIsoMapEdit.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(DruIsoMapEdit)
        self.statusbar.setObjectName(u"statusbar")
        DruIsoMapEdit.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuEdit.menuAction())
        self.menubar.addAction(self.menuHelp.menuAction())
        self.menuFile.addAction(self.actionNew)
        self.menuFile.addAction(self.actionOpen)
        self.menuFile.addSeparator()
        self.menuFile.addAction(self.actionSave)
        self.menuFile.addAction(self.actionSave_As)
        self.menuFile.addSeparator()
        self.menuFile.addAction(self.actionExit)
        self.menuFile.addAction(self.actionExit_2)
        self.menuEdit.addAction(self.actionUndo)
        self.menuEdit.addSeparator()
        self.menuEdit.addAction(self.actionPreferences)
        self.menuHelp.addAction(self.actionHelp)
        self.menuHelp.addAction(self.actionAbout)

        self.retranslateUi(DruIsoMapEdit)

        self.MainTabs.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(DruIsoMapEdit)
    # setupUi

    def retranslateUi(self, DruIsoMapEdit):
        DruIsoMapEdit.setWindowTitle(QCoreApplication.translate("DruIsoMapEdit", u"DruIso Map Editor", None))
        self.actionNew.setText(QCoreApplication.translate("DruIsoMapEdit", u"New", None))
        self.actionOpen.setText(QCoreApplication.translate("DruIsoMapEdit", u"Open", None))
        self.actionSave.setText(QCoreApplication.translate("DruIsoMapEdit", u"Save", None))
        self.actionSave_As.setText(QCoreApplication.translate("DruIsoMapEdit", u"Save As", None))
        self.actionExit.setText(QCoreApplication.translate("DruIsoMapEdit", u"Close", None))
        self.actionExit_2.setText(QCoreApplication.translate("DruIsoMapEdit", u"Exit", None))
        self.actionHelp.setText(QCoreApplication.translate("DruIsoMapEdit", u"Help", None))
        self.actionAbout.setText(QCoreApplication.translate("DruIsoMapEdit", u"About", None))
        self.actionUndo.setText(QCoreApplication.translate("DruIsoMapEdit", u"Undo", None))
        self.actionPreferences.setText(QCoreApplication.translate("DruIsoMapEdit", u"Preferences", None))
        self.EditorArea.setText(QCoreApplication.translate("DruIsoMapEdit", u"TextLabel", None))
        self.TexPreviewArea.setTitle(QCoreApplication.translate("DruIsoMapEdit", u"Preview", None))
        self.TexPreview.setText(QCoreApplication.translate("DruIsoMapEdit", u"TextLabel", None))
        self.PalItemDetails.setTitle(QCoreApplication.translate("DruIsoMapEdit", u"Details", None))
        self.PalItemNameLabel.setText(QCoreApplication.translate("DruIsoMapEdit", u"Name", None))
        self.PalItemDescriptionLabel.setText(QCoreApplication.translate("DruIsoMapEdit", u"Description", None))
        self.UpButton.setText(QCoreApplication.translate("DruIsoMapEdit", u"Up one level", None))
        self.DownButton.setText(QCoreApplication.translate("DruIsoMapEdit", u"Down one level", None))
        self.MainTabs.setTabText(self.MainTabs.indexOf(self.EditorTab), QCoreApplication.translate("DruIsoMapEdit", u"Editor", None))
        self.PreviewArea.setText(QCoreApplication.translate("DruIsoMapEdit", u"TextLabel", None))
        self.XPosLabel.setText(QCoreApplication.translate("DruIsoMapEdit", u"X Position", None))
        self.YPosLabel.setText(QCoreApplication.translate("DruIsoMapEdit", u"Y Position", None))
        self.ElevationLabel.setText(QCoreApplication.translate("DruIsoMapEdit", u"Elevation", None))
        self.MainTabs.setTabText(self.MainTabs.indexOf(self.PreviewTab), QCoreApplication.translate("DruIsoMapEdit", u"Preview", None))
        self.menuFile.setTitle(QCoreApplication.translate("DruIsoMapEdit", u"File", None))
        self.menuEdit.setTitle(QCoreApplication.translate("DruIsoMapEdit", u"Edit", None))
        self.menuHelp.setTitle(QCoreApplication.translate("DruIsoMapEdit", u"Help", None))
    # retranslateUi

