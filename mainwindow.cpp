#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <filesystem>
#include <QMessageBox>
#include <spdlog/spdlog.h>
#include <QColorDialog>
#include <QSettings>
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/taglib.h>
#include "settingsvalues.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGUIEnabled(false);

    connect(ui->controllerWidget, &ControllerWidget::bufferReady, ui->visualizerWidget, &VisualizerWidget::bufferAccept);
    connect(ui->controllerWidget, &ControllerWidget::bufferClear, ui->visualizerWidget, &VisualizerWidget::clearBuffer);

    connect(ui->controllerWidget, &ControllerWidget::error, this, [this]{
        setGUIEnabled(false);
    });
    connect(ui->controllerWidget, &ControllerWidget::ok, this, [this] {
        setGUIEnabled(true);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_Music_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open audio file"), QDir::homePath(), tr("Audio files (*.mp3 *.wav);;All files (*.*)"));
    if (filename.isEmpty() || !std::filesystem::exists(filename.toStdString())) {
        spdlog::error("Such file does not exist: {}", filename.toStdString());
        QMessageBox::warning(this, tr("Warning"), tr("Audio could not be loaded!"));
        return;
    }


    TagLib::FileRef f{filename.toStdString().c_str()};
    if (!f.isNull() && f.tag()) {
        auto title = f.tag()->title().to8Bit();
        ui->label->setText(title.empty() ? QString{tr("Unknown song")} : QString{title.c_str()});
        ui->controllerWidget->loadMusic(f);
    }
}

void MainWindow::setGUIEnabled(bool val)
{
    ui->controllerWidget->setEnabled(val);
    ui->visualizerWidget->setEnabled(val);
}


void MainWindow::on_actionPick_color_triggered()
{
    QColor color = QColorDialog::getColor();
    auto var = QVariant::fromValue(color);

    QSettings settings;
    settings.setValue(SettingsValues::BARS_COLOR, var);
}

