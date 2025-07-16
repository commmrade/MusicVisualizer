#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <filesystem>
#include <QMessageBox>
#include <spdlog/spdlog.h>

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
    if (filename.isEmpty() || !std::filesystem::exists(filename.toStdString())) {
        spdlog::error("Such file does not exist: {}", filename.toStdString());
        QMessageBox::warning(this, "Warning", "Audio could not be loaded!");
        return;
    }
    ui->controllerWidget->loadMusic(filename);
    enableGUI(true);
}

void MainWindow::enableGUI(bool val)
{
    spdlog::info("GUI enabled: {}", val);

    ui->controllerWidget->setEnabled(val);
    ui->visualizerWidget->setEnabled(val);
}

