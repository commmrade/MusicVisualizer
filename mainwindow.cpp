#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    enableGUI(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_Music_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open audio file"), QDir::homePath(), tr("Audio files (*.mp3, *.wav);;All files (*.*)"));
    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Path to audio file is empty!");
        return;
    }
    ui->controllerWidget->loadMusic(filename);

    enableGUI(true);
}

void MainWindow::enableGUI(bool val)
{
    ui->controllerWidget->setEnabled(val);
    ui->visualizerWidget->setEnabled(val);
}

