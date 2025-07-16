#include "controllerwidget.h"
#include "ui_controllerwidget.h"

ControllerWidget::ControllerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControllerWidget)
{
    ui->setupUi(this);
    connect(this, &ControllerWidget::volumeChanged, &m_musicController, &MusicController::setVolume);
    connect(this, &ControllerWidget::playPressed, &m_musicController, &MusicController::playOrPause);
    connect(this, &ControllerWidget::mutePressed, &m_musicController, &MusicController::mute);

    connect(&m_musicController, &MusicController::setSliderVolume, this, [this](float val) {
        ui->volumeSlider->setValue(val * 100);
    });
}

ControllerWidget::~ControllerWidget()
{
    delete ui;
}

void ControllerWidget::loadMusic(const QString& path)
{
    m_musicController.loadMusic(path);
}

void ControllerWidget::on_volumeSlider_valueChanged(int value)
{
    emit volumeChanged(value);
}


void ControllerWidget::on_playButton_clicked()
{
    emit playPressed();
}

void ControllerWidget::on_muteButton_clicked()
{
    emit mutePressed();
}

