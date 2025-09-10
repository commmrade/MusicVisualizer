#include "controllerwidget.h"
#include "ui_controllerwidget.h"
#include <QMessageBox>

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

    connect(&m_musicController, &MusicController::errorDecoding, this, [this](QAudioDecoder::Error err) {
        QMessageBox::critical(this, tr("Error"), tr("Error when loading audio: ") + QString::number(err));
        emit error();
    });
    connect(&m_musicController, &MusicController::okDecoding, this, [this]{
        emit ok();
    });
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
    auto elapsedHrs = elapsed / 3600;
    auto elapsedMins = elapsed % 3600 / 60;
    auto elapsedSecs = elapsed % 60;

    auto totalHrs = total / 3600;
    auto totalMins = total % 3600 / 60;
    auto totalSecs = total % 60;

    auto elapsedStr = QString{"%1:%2:%3/%4:%5:%6"}
        .arg(elapsedHrs, 2, 10, '0')
        .arg(elapsedMins, 2, 10, '0')
        .arg(elapsedSecs, 2, 10, '0')
        .arg(totalHrs, 2, 10, '0')
        .arg(totalMins, 2, 10, '0')
        .arg(totalSecs, 2, 10, '0');
    ui->seekBarLabel->setText(elapsedStr);
    ui->seekBar->setMaximum(total);
    ui->seekBar->setValue(elapsed);
}

void ControllerWidget::on_seekBar_sliderMoved(int position)
{
    m_musicController.setMusicElapsed(position);
}

