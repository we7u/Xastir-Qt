/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * XASTIR, Amateur Station Tracking and Information Reporting
 * Copyright (C) 1999,2000  Frank Giannandrea
 * Copyright (C) 2000-2018 The Xastir Group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Look at the README for more information on the program.
 */

#include "ui_mainwindow.h"
#include "interfacecontroldialog.h"
#include "xastir.h"
#include "symbols.h"
#include "colors.h"
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    interfaceControlDialog = NULL;
    stationConfigurationDialog = NULL;

    connect(&interfaceManager, SIGNAL(interfaceAdded(PacketInterface*)), this, SLOT(newInterface(PacketInterface*)));
 //   connect(&netInterface,SIGNAL(interfaceChangedState(PacketInterface::Device_Status)), this, SLOT(statusChanged(PacketInterface::Device_Status)));
 //   connect(&netInterface,SIGNAL(packetReceived(PacketInterface *, QString)), this, SLOT(newData(PacketInterface *,QString)));
    total_lines = 0;
    interfaceManager.restoreInterfaces();
    QSettings settings;

    settings.beginGroup("StationSettings");
    stationSettings.restoreFromSettings(settings);
    settings.endGroup();
    initializeColors();
    load_pixmap_symbol_file( (char *)":/xastir/symbols.dat", 0);
}

MainWindow::~MainWindow()
{
    delete ui;
    interfaceManager.saveInterfaces();
    QSettings settings;

    settings.beginGroup("StationSettings");
    settings.remove(""); // Delete previous settings
    stationSettings.saveSettings(settings);
    settings.endGroup();
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::newInterface(PacketInterface *iface)
{
    connect(iface, SIGNAL(packetReceived(PacketInterface*,QString)), this, SLOT(newData(PacketInterface*,QString)));
}

void MainWindow::newData (PacketInterface *device, QString data)
{
    int max_lines = 15;
    QString tmp;


    // Best here would be to look for CR or LF in "data", remove, then add "\n".
    // We're not doing that yet.
    //
    // packetDisplay.append(device->deviceName() + "-> " + data + "\n");
    packetDisplay.append(device->deviceName() + "-> " + data);


    if (total_lines >= max_lines) {
        int ii = packetDisplay.indexOf("\n");
        // Chop first line
        tmp = packetDisplay.right(packetDisplay.size() - (ii + 1));
        packetDisplay = tmp;
    }
    else {
        total_lines++;
    }
    ui->incomingPackets->setText(packetDisplay);
}

void MainWindow::interfaceControlAction()
{
    if( interfaceControlDialog == NULL) {
        interfaceControlDialog = new InterfaceControlDialog(interfaceManager, this);
    }
    interfaceControlDialog->show();
    interfaceControlDialog->raise();

}

void MainWindow::stationSettingsAction()
{
    stationConfigurationDialog = new StationConfigurationDialog(&stationSettings, this);
    stationConfigurationDialog->show();
    stationConfigurationDialog->raise();
}
