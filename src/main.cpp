/*
 *  Copyright (C) 2015, Mike Walters <mike@flomp.net>
 *
 *  This file is part of spectr.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QStyleFactory>
#include <QFont>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("spectr");
    a.setOrganizationName("spectr");

    // Use Fusion style as a clean base for custom theming
    a.setStyle(QStyleFactory::create("Fusion"));

    // Set a clean, modern font
    QFont appFont("Segoe UI", 9);
    appFont.setHintingPreference(QFont::PreferFullHinting);
    a.setFont(appFont);

    // ── Dark theme stylesheet ──────────────────────────────────────────
    a.setStyleSheet(R"(
        /* ── Global ─────────────────────────────────────────────── */
        QMainWindow, QWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
        }

        /* ── Menu Bar ───────────────────────────────────────────── */
        QMenuBar {
            background-color: #181825;
            color: #cdd6f4;
            border-bottom: 1px solid #313244;
            padding: 2px;
        }
        QMenuBar::item {
            padding: 4px 10px;
            border-radius: 4px;
        }
        QMenuBar::item:selected {
            background-color: #313244;
        }
        QMenu {
            background-color: #1e1e2e;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 4px;
        }
        QMenu::item {
            padding: 6px 28px 6px 12px;
            border-radius: 4px;
        }
        QMenu::item:selected {
            background-color: rgba(86, 212, 200, 0.15);
            color: #56d4c8;
        }
        QMenu::separator {
            height: 1px;
            background-color: #313244;
            margin: 4px 8px;
        }

        /* ── Toolbar ────────────────────────────────────────────── */
        QToolBar {
            background-color: #181825;
            border-bottom: 1px solid #313244;
            padding: 3px 6px;
            spacing: 4px;
        }
        QToolBar QToolButton {
            background-color: transparent;
            color: #cdd6f4;
            border: 1px solid transparent;
            border-radius: 5px;
            padding: 5px 12px;
            font-weight: 500;
        }
        QToolBar QToolButton:hover {
            background-color: rgba(86, 212, 200, 0.12);
            border: 1px solid rgba(86, 212, 200, 0.3);
            color: #56d4c8;
        }
        QToolBar QToolButton:pressed {
            background-color: rgba(86, 212, 200, 0.22);
        }

        /* ── Status Bar ─────────────────────────────────────────── */
        QStatusBar {
            background-color: #181825;
            color: #a6adc8;
            border-top: 1px solid #313244;
            padding: 2px 8px;
            font-size: 8pt;
        }
        QStatusBar QLabel {
            color: #a6adc8;
            padding: 0 4px;
        }

        /* ── Dock Widget ────────────────────────────────────────── */
        QDockWidget {
            color: #cdd6f4;
            titlebar-close-icon: none;
            titlebar-normal-icon: none;
        }
        QDockWidget::title {
            background-color: #181825;
            color: #56d4c8;
            padding: 8px 12px;
            border-bottom: 2px solid #56d4c8;
            font-weight: bold;
            font-size: 10pt;
        }
        QDockWidget::close-button, QDockWidget::float-button {
            background-color: transparent;
            border: none;
            padding: 2px;
        }
        QDockWidget::close-button:hover, QDockWidget::float-button:hover {
            background-color: #313244;
            border-radius: 3px;
        }
        QDockWidget > QWidget {
            background-color: #1e1e2e;
        }

        /* ── Group Box ──────────────────────────────────────────── */
        QGroupBox {
            background-color: #232336;
            border: 1px solid #313244;
            border-radius: 8px;
            margin-top: 16px;
            padding: 12px 8px 8px 8px;
            font-weight: bold;
            font-size: 9pt;
            color: #89b4fa;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 2px 10px;
            left: 12px;
            background-color: #232336;
            border-radius: 4px;
            color: #89b4fa;
        }

        /* ── Push Button ────────────────────────────────────────── */
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3b3b5c, stop:1 #2e2e4a);
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 7px 18px;
            font-weight: 500;
            min-height: 18px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #45457a, stop:1 #383860);
            border: 1px solid #56d4c8;
            color: #56d4c8;
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2a2a44, stop:1 #232336);
            border: 1px solid #56d4c8;
        }

        /* ── Line Edit ──────────────────────────────────────────── */
        QLineEdit {
            background-color: #181825;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 5px;
            padding: 5px 8px;
            selection-background-color: rgba(86, 212, 200, 0.3);
            selection-color: #ffffff;
        }
        QLineEdit:focus {
            border: 1px solid #56d4c8;
        }

        /* ── Spin Box ───────────────────────────────────────────── */
        QSpinBox {
            background-color: #181825;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 5px;
            padding: 4px 6px;
        }
        QSpinBox:focus {
            border: 1px solid #56d4c8;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #313244;
            border: none;
            width: 16px;
            border-radius: 3px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background-color: #45475a;
        }
        QSpinBox::up-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-bottom: 5px solid #cdd6f4;
            width: 0; height: 0;
        }
        QSpinBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 5px solid #cdd6f4;
            width: 0; height: 0;
        }

        /* ── Check Box ──────────────────────────────────────────── */
        QCheckBox {
            color: #cdd6f4;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 2px solid #45475a;
            border-radius: 4px;
            background-color: #181825;
        }
        QCheckBox::indicator:hover {
            border-color: #56d4c8;
        }
        QCheckBox::indicator:checked {
            background-color: #56d4c8;
            border-color: #56d4c8;
        }

        /* ── Slider ─────────────────────────────────────────────── */
        QSlider::groove:horizontal {
            height: 6px;
            background-color: #313244;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: qradialgradient(cx:0.5, cy:0.5, radius:0.5,
                fx:0.5, fy:0.4,
                stop:0 #7af5e8, stop:0.7 #56d4c8, stop:1 #3aafa4);
            width: 16px;
            height: 16px;
            margin: -5px 0;
            border-radius: 8px;
            border: 1px solid #3aafa4;
        }
        QSlider::handle:horizontal:hover {
            background: qradialgradient(cx:0.5, cy:0.5, radius:0.5,
                fx:0.5, fy:0.4,
                stop:0 #a0fff5, stop:0.7 #7af5e8, stop:1 #56d4c8);
            border: 1px solid #56d4c8;
        }
        QSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #56d4c8, stop:1 #89b4fa);
            border-radius: 3px;
        }

        /* ── Scrollbar (Horizontal) ─────────────────────────────── */
        QScrollBar:horizontal {
            background-color: #181825;
            height: 10px;
            border: none;
        }
        QScrollBar::handle:horizontal {
            background-color: #45475a;
            border-radius: 4px;
            min-width: 30px;
            margin: 1px;
        }
        QScrollBar::handle:horizontal:hover {
            background-color: #585b70;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: none;
        }

        /* ── Scrollbar (Vertical) ───────────────────────────────── */
        QScrollBar:vertical {
            background-color: #181825;
            width: 10px;
            border: none;
        }
        QScrollBar::handle:vertical {
            background-color: #45475a;
            border-radius: 4px;
            min-height: 30px;
            margin: 1px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #585b70;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
        }

        /* ── Tooltip ────────────────────────────────────────────── */
        QToolTip {
            background-color: #1e1e2e;
            color: #cdd6f4;
            border: 1px solid #56d4c8;
            border-radius: 4px;
            padding: 6px 10px;
            font-size: 8pt;
        }

        /* ── Label (value readout styling) ──────────────────────── */
        QLabel[objectName="valueLabel"] {
            color: #56d4c8;
            font-weight: bold;
            font-size: 9pt;
            padding: 0 2px;
            min-width: 50px;
        }
        QLabel[objectName="sectionHeader"] {
            color: #89b4fa;
            font-weight: bold;
            font-size: 10pt;
            padding: 4px 0;
        }

        /* ── File Dialog / Message Box ──────────────────────────── */
        QDialog {
            background-color: #1e1e2e;
            color: #cdd6f4;
        }
        QMessageBox {
            background-color: #1e1e2e;
        }
        QMessageBox QLabel {
            color: #cdd6f4;
        }

        /* ── Progress Bar ───────────────────────────────────────── */
        QProgressBar {
            background-color: #313244;
            border: 1px solid #45475a;
            border-radius: 5px;
            text-align: center;
            color: #cdd6f4;
            height: 18px;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #56d4c8, stop:1 #89b4fa);
            border-radius: 4px;
        }

        /* ── Tab Widget (if used) ───────────────────────────────── */
        QTabBar::tab {
            background-color: #232336;
            color: #a6adc8;
            border: 1px solid #313244;
            padding: 6px 14px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background-color: #1e1e2e;
            color: #56d4c8;
            border-bottom: 2px solid #56d4c8;
        }

        /* ── Graphics View (plot area) ──────────────────────────── */
        QGraphicsView {
            border: none;
            background-color: #000000;
        }
    )");

    MainWindow mainWin;

    QCommandLineParser parser;
    parser.setApplicationDescription("spectrum viewer");
    parser.addHelpOption();
    parser.addPositionalArgument("file", QCoreApplication::translate("main", "File to view."));

    // Add options
    QCommandLineOption rateOption(QStringList() << "r" << "rate",
                                  QCoreApplication::translate("main", "Set sample rate."),
                                  QCoreApplication::translate("main", "Hz"));
    parser.addOption(rateOption);
    QCommandLineOption formatOption(QStringList() << "f" << "format",
                                  QCoreApplication::translate("main", "Set file format, options: cfile/cf32/fc32, cf64/fc64, cs32/sc32/c32, cs16/sc16/c16, cs8/sc8/c8, cu8/uc8, f32, f64, s16, s8, u8, sigmf-meta/sigmf-data."),
                                  QCoreApplication::translate("main", "fmt"));
    parser.addOption(formatOption);

    // Process the actual command line
    parser.process(a);
 
    // Check for file format override   
    if(parser.isSet(formatOption)){
        mainWin.setFormat(parser.value(formatOption));
    }

    const QStringList args = parser.positionalArguments();
    if (args.size()>=1)
        mainWin.openFile(args.at(0));

    if (parser.isSet(rateOption)) {
        bool ok;
        auto rate = parser.value(rateOption).toDouble(&ok);
        if(!ok) {
            fputs("ERROR: could not parse rate\n", stderr);
            return 1;
        }
        mainWin.setSampleRate(rate);
    }

    mainWin.show();
    return a.exec();
}
