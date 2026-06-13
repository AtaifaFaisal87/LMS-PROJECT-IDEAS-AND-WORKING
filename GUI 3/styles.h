#pragma once
#include <QString>

// ============================================================
//  COLOUR PALETTE
//  Primary:   Deep navy   #1A1F3C
//  Accent:    Royal blue  #3D5AF1
//  Highlight: Cyan-teal   #22D3EE
//  Success:   Emerald     #10B981
//  Warning:   Amber       #F59E0B
//  Danger:    Rose        #F43F5E
//  Surface:   Dark card   #232946
//  Text:      Off-white   #E0E7FF
// ============================================================

static const QString APP_STYLE = R"(
/* ---- Global ---- */
QWidget {
    background-color: #1A1F3C;
    color: #E0E7FF;
    font-family: "Segoe UI", "Arial", sans-serif;
    font-size: 13px;
}

/* ---- Main Window / Dialogs ---- */
QMainWindow, QDialog {
    background-color: #1A1F3C;
}

/* ---- Group Boxes ---- */
QGroupBox {
    background-color: #232946;
    border: 1.5px solid #3D5AF1;
    border-radius: 8px;
    margin-top: 18px;
    padding: 12px;
    font-weight: bold;
    color: #22D3EE;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 2px 10px;
    color: #22D3EE;
    font-size: 14px;
}

/* ---- Push Buttons ---- */
QPushButton {
    background-color: #3D5AF1;
    color: #FFFFFF;
    border: none;
    border-radius: 6px;
    padding: 8px 18px;
    font-weight: bold;
    font-size: 13px;
    min-width: 80px;
}
QPushButton:hover {
    background-color: #5B73F5;
}
QPushButton:pressed {
    background-color: #2940C8;
}
QPushButton:disabled {
    background-color: #3A3F5C;
    color: #6B7280;
}
QPushButton#dangerBtn {
    background-color: #F43F5E;
}
QPushButton#dangerBtn:hover {
    background-color: #FB6F87;
}
QPushButton#successBtn {
    background-color: #10B981;
}
QPushButton#successBtn:hover {
    background-color: #34D399;
}
QPushButton#warningBtn {
    background-color: #F59E0B;
    color: #1A1F3C;
}
QPushButton#warningBtn:hover {
    background-color: #FCD34D;
}
QPushButton#cyanBtn {
    background-color: #22D3EE;
    color: #1A1F3C;
}
QPushButton#cyanBtn:hover {
    background-color: #67E8F9;
}

/* ---- Line Edits ---- */
QLineEdit {
    background-color: #2D3561;
    border: 1.5px solid #3D5AF1;
    border-radius: 5px;
    padding: 7px 10px;
    color: #E0E7FF;
    selection-background-color: #3D5AF1;
}
QLineEdit:focus {
    border: 2px solid #22D3EE;
}

/* ---- Combo Boxes ---- */
QComboBox {
    background-color: #2D3561;
    border: 1.5px solid #3D5AF1;
    border-radius: 5px;
    padding: 6px 10px;
    color: #E0E7FF;
    min-width: 120px;
}
QComboBox:focus {
    border: 2px solid #22D3EE;
}
QComboBox QAbstractItemView {
    background-color: #232946;
    border: 1px solid #3D5AF1;
    selection-background-color: #3D5AF1;
    color: #E0E7FF;
}
QComboBox::drop-down {
    border: none;
    width: 24px;
}

/* ---- Spin Boxes ---- */
QSpinBox, QDoubleSpinBox {
    background-color: #2D3561;
    border: 1.5px solid #3D5AF1;
    border-radius: 5px;
    padding: 6px;
    color: #E0E7FF;
}
QSpinBox:focus, QDoubleSpinBox:focus {
    border: 2px solid #22D3EE;
}

/* ---- Labels ---- */
QLabel {
    color: #E0E7FF;
}
QLabel#titleLabel {
    color: #22D3EE;
    font-size: 26px;
    font-weight: bold;
}
QLabel#subtitleLabel {
    color: #A5B4FC;
    font-size: 14px;
}
QLabel#sectionLabel {
    color: #22D3EE;
    font-size: 15px;
    font-weight: bold;
}
QLabel#statValue {
    color: #10B981;
    font-size: 28px;
    font-weight: bold;
}
QLabel#statLabel {
    color: #A5B4FC;
    font-size: 11px;
}
QLabel#warnLabel {
    color: #F59E0B;
    font-weight: bold;
}
QLabel#errorLabel {
    color: #F43F5E;
    font-weight: bold;
}

/* ---- Table Widget ---- */
QTableWidget {
    background-color: #232946;
    border: 1px solid #3D5AF1;
    border-radius: 6px;
    gridline-color: #2D3561;
    alternate-background-color: #1E2440;
    color: #E0E7FF;
}
QTableWidget::item {
    padding: 6px;
}
QTableWidget::item:selected {
    background-color: #3D5AF1;
    color: #FFFFFF;
}
QHeaderView::section {
    background-color: #3D5AF1;
    color: #FFFFFF;
    padding: 8px;
    font-weight: bold;
    border: none;
    border-right: 1px solid #2940C8;
}
QHeaderView::section:first {
    border-top-left-radius: 5px;
}
QHeaderView::section:last {
    border-top-right-radius: 5px;
    border-right: none;
}

/* ---- Tab Widget ---- */
QTabWidget::pane {
    background-color: #232946;
    border: 1px solid #3D5AF1;
    border-radius: 6px;
}
QTabBar::tab {
    background-color: #2D3561;
    color: #A5B4FC;
    padding: 9px 20px;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    margin-right: 2px;
    font-weight: bold;
}
QTabBar::tab:selected {
    background-color: #3D5AF1;
    color: #FFFFFF;
}
QTabBar::tab:hover:!selected {
    background-color: #3A3F5C;
    color: #E0E7FF;
}

/* ---- Scroll Bars ---- */
QScrollBar:vertical {
    background-color: #1A1F3C;
    width: 10px;
    border-radius: 5px;
}
QScrollBar::handle:vertical {
    background-color: #3D5AF1;
    border-radius: 5px;
    min-height: 20px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

QScrollBar:horizontal {
    background-color: #1A1F3C;
    height: 10px;
    border-radius: 5px;
}
QScrollBar::handle:horizontal {
    background-color: #3D5AF1;
    border-radius: 5px;
    min-width: 20px;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

/* ---- List Widget ---- */
QListWidget {
    background-color: #232946;
    border: 1px solid #3D5AF1;
    border-radius: 6px;
    color: #E0E7FF;
}
QListWidget::item {
    padding: 8px;
    border-bottom: 1px solid #2D3561;
}
QListWidget::item:selected {
    background-color: #3D5AF1;
    color: #FFFFFF;
}

/* ---- Message Boxes ---- */
QMessageBox {
    background-color: #232946;
    color: #E0E7FF;
}

/* ---- Separator ---- */
QFrame[frameShape="4"], QFrame[frameShape="5"] {
    color: #3D5AF1;
}

/* ---- Status Bar ---- */
QStatusBar {
    background-color: #1A1F3C;
    color: #A5B4FC;
    border-top: 1px solid #3D5AF1;
}

/* ---- Tool Tips ---- */
QToolTip {
    background-color: #232946;
    color: #22D3EE;
    border: 1px solid #3D5AF1;
    border-radius: 4px;
    padding: 4px;
}

/* ---- Progress Bar ---- */
QProgressBar {
    background-color: #2D3561;
    border: 1px solid #3D5AF1;
    border-radius: 5px;
    text-align: center;
    color: #FFFFFF;
}
QProgressBar::chunk {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                      stop:0 #3D5AF1, stop:1 #22D3EE);
    border-radius: 5px;
}
)";

// Card widget stylesheet snippet
static const QString CARD_STYLE = R"(
    background-color: #232946;
    border: 1.5px solid #3D5AF1;
    border-radius: 10px;
    padding: 16px;
)";

static const QString STAT_CARD_STYLE_BLUE = R"(
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #3D5AF1,stop:1 #232946);
    border-radius: 10px; padding: 16px;
)";
static const QString STAT_CARD_STYLE_CYAN = R"(
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #0E7490,stop:1 #232946);
    border-radius: 10px; padding: 16px;
)";
static const QString STAT_CARD_STYLE_GREEN = R"(
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #065F46,stop:1 #232946);
    border-radius: 10px; padding: 16px;
)";
static const QString STAT_CARD_STYLE_ROSE = R"(
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #9F1239,stop:1 #232946);
    border-radius: 10px; padding: 16px;
)";
