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
    connect(&m_musicController, &MusicController::elapsedChanged, this, &ControllerWidget::setSeekBarValue);
    connect(&m_musicController, &MusicController::bufferReady, this, &ControllerWidget::bufferReady);
}

ControllerWidget::~ControllerWidget()
{
    delete ui;
}

void ControllerWidget::loadMusic(TagLib::FileRef fileRef)
{
    m_musicController.loadMusic(fileRef);
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

void ControllerWidget::setSeekBarValue(int elapsed, int total)
{
    ui->seekBarLabel->setText(QString("%1 secs / %2 secs").arg(elapsed).arg(total));
    ui->seekBar->setMaximum(total);
    ui->seekBar->setValue(elapsed);
}

void ControllerWidget::on_seekBar_sliderMoved(int position)
{
    m_musicController.setMusicElapsed(position);
}

