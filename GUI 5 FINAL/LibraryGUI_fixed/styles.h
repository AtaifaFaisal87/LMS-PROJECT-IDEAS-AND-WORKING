#pragma once
#include <QString>

// ============================================================
//  PARCHMENT CODEX — Colour Palette
//  Background:  Aged parchment   #F5F0E8
//  Surface:     Cream card       #EDE8DC
//  Primary:     Deep burgundy    #7A1C2E
//  Accent:      Dark sepia ink   #5C3D1E
//  Secondary:   Warm tan         #B8956A
//  Success:     Forest green     #2D5E1E
//  Warning:     Amber ochre      #C67C00
//  Danger:      Crimson          #A01020
//  Text:        Dark brown ink   #2C1A0E
//  Muted:       Faded sepia      #8A7560
// ============================================================

static const QString APP_STYLE = R"(
/* ---- Global ---- */
QWidget {
    background-color: #F5F0E8;
    color: #2C1A0E;
    font-family: "Georgia", "Times New Roman", serif;
    font-size: 13px;
}

/* ---- Main Window / Dialogs ---- */
QMainWindow, QDialog {
    background-color: #F5F0E8;
}

/* ---- Menu Bar ---- */
QMenuBar {
    background-color: #EDE8DC;
    color: #2C1A0E;
    border-bottom: 2px solid #B8956A;
    font-family: "Georgia", serif;
    font-size: 13px;
}
QMenuBar::item {
    background: transparent;
    padding: 6px 12px;
}
QMenuBar::item:selected {
    background-color: #7A1C2E;
    color: #F5F0E8;
    border-radius: 3px;
}
QMenu {
    background-color: #EDE8DC;
    border: 1px solid #B8956A;
    color: #2C1A0E;
}
QMenu::item:selected {
    background-color: #7A1C2E;
    color: #F5F0E8;
}

/* ---- Group Boxes ---- */
QGroupBox {
    background-color: #EDE8DC;
    border: 1.5px solid #B8956A;
    border-radius: 6px;
    margin-top: 18px;
    padding: 12px;
    font-weight: bold;
    color: #7A1C2E;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 2px 10px;
    color: #7A1C2E;
    font-size: 14px;
    font-family: "Georgia", serif;
}

/* ---- Push Buttons ---- */
QPushButton {
    background-color: #7A1C2E;
    color: #F5F0E8;
    border: 1px solid #5C1020;
    border-radius: 4px;
    padding: 8px 18px;
    font-weight: bold;
    font-size: 12px;
    font-family: "Georgia", serif;
    min-width: 80px;
}
QPushButton:hover {
    background-color: #9B2436;
    border-color: #7A1C2E;
}
QPushButton:pressed {
    background-color: #5C1020;
}
QPushButton:disabled {
    background-color: #C4B89A;
    color: #8A7560;
    border-color: #B8956A;
}
QPushButton#dangerBtn {
    background-color: #A01020;
    border-color: #7A0818;
}
QPushButton#dangerBtn:hover {
    background-color: #C01828;
}
QPushButton#successBtn {
    background-color: #2D5E1E;
    border-color: #1E4010;
}
QPushButton#successBtn:hover {
    background-color: #3C7828;
}
QPushButton#warningBtn {
    background-color: #C67C00;
    color: #2C1A0E;
    border-color: #9A5E00;
}
QPushButton#warningBtn:hover {
    background-color: #E08E00;
}
QPushButton#cyanBtn {
    background-color: #5C3D1E;
    color: #F5F0E8;
    border-color: #3E2810;
}
QPushButton#cyanBtn:hover {
    background-color: #7A5228;
}

/* ---- Line Edits ---- */
QLineEdit {
    background-color: #FAF7F2;
    border: 1.5px solid #B8956A;
    border-radius: 4px;
    padding: 7px 10px;
    color: #2C1A0E;
    selection-background-color: #7A1C2E;
    selection-color: #F5F0E8;
}
QLineEdit:focus {
    border: 2px solid #7A1C2E;
    background-color: #FFFEF9;
}

/* ---- Combo Boxes ---- */
QComboBox {
    background-color: #FAF7F2;
    border: 1.5px solid #B8956A;
    border-radius: 4px;
    padding: 6px 10px;
    color: #2C1A0E;
    min-width: 120px;
}
QComboBox:focus {
    border: 2px solid #7A1C2E;
}
QComboBox QAbstractItemView {
    background-color: #EDE8DC;
    border: 1px solid #B8956A;
    selection-background-color: #7A1C2E;
    selection-color: #F5F0E8;
    color: #2C1A0E;
}
QComboBox::drop-down {
    border: none;
    width: 24px;
}

/* ---- Spin Boxes ---- */
QSpinBox, QDoubleSpinBox {
    background-color: #FAF7F2;
    border: 1.5px solid #B8956A;
    border-radius: 4px;
    padding: 6px;
    color: #2C1A0E;
}
QSpinBox:focus, QDoubleSpinBox:focus {
    border: 2px solid #7A1C2E;
}

/* ---- Labels ---- */
QLabel {
    color: #2C1A0E;
}
QLabel#titleLabel {
    color: #7A1C2E;
    font-size: 26px;
    font-weight: bold;
    font-family: "Georgia", serif;
}
QLabel#subtitleLabel {
    color: #8A7560;
    font-size: 14px;
    font-style: italic;
}
QLabel#sectionLabel {
    color: #7A1C2E;
    font-size: 15px;
    font-weight: bold;
    font-family: "Georgia", serif;
}
QLabel#statValue {
    color: #2D5E1E;
    font-size: 28px;
    font-weight: bold;
    font-family: "Georgia", serif;
}
QLabel#statLabel {
    color: #8A7560;
    font-size: 11px;
    font-style: italic;
    letter-spacing: 1px;
}
QLabel#warnLabel {
    color: #C67C00;
    font-weight: bold;
}
QLabel#errorLabel {
    color: #A01020;
    font-weight: bold;
}

/* ---- Table Widget ---- */
QTableWidget {
    background-color: #FAF7F2;
    border: 1px solid #B8956A;
    border-radius: 4px;
    gridline-color: #D4C9B0;
    alternate-background-color: #F0EBE0;
    color: #2C1A0E;
}
QTableWidget::item {
    padding: 6px;
}
QTableWidget::item:selected {
    background-color: #7A1C2E;
    color: #F5F0E8;
}
QHeaderView::section {
    background-color: #5C3D1E;
    color: #F5F0E8;
    padding: 8px;
    font-weight: bold;
    font-family: "Georgia", serif;
    border: none;
    border-right: 1px solid #3E2810;
}
QHeaderView::section:first {
    border-top-left-radius: 4px;
}
QHeaderView::section:last {
    border-top-right-radius: 4px;
    border-right: none;
}

/* ---- Tab Widget ---- */
QTabWidget::pane {
    background-color: #EDE8DC;
    border: 1px solid #B8956A;
    border-radius: 4px;
}
QTabBar::tab {
    background-color: #D4C9B0;
    color: #5C3D1E;
    padding: 9px 20px;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    margin-right: 2px;
    font-weight: bold;
    font-family: "Georgia", serif;
    border: 1px solid #B8956A;
    border-bottom: none;
}
QTabBar::tab:selected {
    background-color: #7A1C2E;
    color: #F5F0E8;
    border-color: #5C1020;
}
QTabBar::tab:hover:!selected {
    background-color: #C4B89A;
    color: #2C1A0E;
}

/* ---- Scroll Bars ---- */
QScrollBar:vertical {
    background-color: #EDE8DC;
    width: 10px;
    border-radius: 5px;
}
QScrollBar::handle:vertical {
    background-color: #B8956A;
    border-radius: 5px;
    min-height: 20px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

QScrollBar:horizontal {
    background-color: #EDE8DC;
    height: 10px;
    border-radius: 5px;
}
QScrollBar::handle:horizontal {
    background-color: #B8956A;
    border-radius: 5px;
    min-width: 20px;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

/* ---- List Widget ---- */
QListWidget {
    background-color: #FAF7F2;
    border: 1px solid #B8956A;
    border-radius: 4px;
    color: #2C1A0E;
}
QListWidget::item {
    padding: 8px;
    border-bottom: 1px solid #D4C9B0;
}
QListWidget::item:selected {
    background-color: #7A1C2E;
    color: #F5F0E8;
}

/* ---- Message Boxes ---- */
QMessageBox {
    background-color: #EDE8DC;
    color: #2C1A0E;
}

/* ---- Separator ---- */
QFrame[frameShape="4"], QFrame[frameShape="5"] {
    color: #B8956A;
}

/* ---- Status Bar ---- */
QStatusBar {
    background-color: #EDE8DC;
    color: #8A7560;
    border-top: 1px solid #B8956A;
    font-style: italic;
}

/* ---- Tool Tips ---- */
QToolTip {
    background-color: #5C3D1E;
    color: #F5F0E8;
    border: 1px solid #B8956A;
    border-radius: 4px;
    padding: 4px;
    font-family: "Georgia", serif;
}

/* ---- Progress Bar ---- */
QProgressBar {
    background-color: #D4C9B0;
    border: 1px solid #B8956A;
    border-radius: 4px;
    text-align: center;
    color: #2C1A0E;
}
QProgressBar::chunk {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                      stop:0 #7A1C2E, stop:1 #B8956A);
    border-radius: 4px;
}
)";

// Card widget stylesheet snippet
static const QString CARD_STYLE = R"(
    background-color: #EDE8DC;
    border: 1.5px solid #B8956A;
    border-radius: 8px;
    padding: 16px;
)";

static const QString STAT_CARD_STYLE_BLUE = R"(
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #7A1C2E,stop:1 #EDE8DC);
    border: 1px solid #B8956A;
    border-radius: 8px; padding: 16px;
)";
static const QString STAT_CARD_STYLE_CYAN = R"(
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #5C3D1E,stop:1 #EDE8DC);
    border: 1px solid #B8956A;
    border-radius: 8px; padding: 16px;
)";
static const QString STAT_CARD_STYLE_GREEN = R"(
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #2D5E1E,stop:1 #EDE8DC);
    border: 1px solid #B8956A;
    border-radius: 8px; padding: 16px;
)";
static const QString STAT_CARD_STYLE_ROSE = R"(
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #A01020,stop:1 #EDE8DC);
    border: 1px solid #B8956A;
    border-radius: 8px; padding: 16px;
)";
