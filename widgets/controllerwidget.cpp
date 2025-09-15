#include "controllerwidget.h"
#include "ui_controllerwidget.h"
#include <QMessageBox>

constexpr static double DEFAULT_SOUND_VOLUME = 0.5;
constexpr static int SECS_IN_HOUR = 3600;
constexpr static int SECS_IN_MINUTE = 60;

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
    // connect(ui->, &MusicController::bufferClear, this, &ControllerWidget::bufferClear);

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
    emit bufferClear();
    setSeekBarValue(0, 0);
    ui->volumeSlider->setValue(DEFAULT_SOUND_VOLUME * 100.0);

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
    auto elapsedHrs = elapsed / SECS_IN_HOUR;
    auto elapsedMins = elapsed % SECS_IN_HOUR / SECS_IN_MINUTE;
    auto elapsedSecs = elapsed % SECS_IN_MINUTE;

    auto totalHrs = total / SECS_IN_HOUR;
    auto totalMins = total % SECS_IN_HOUR / SECS_IN_MINUTE;
    auto totalSecs = total % SECS_IN_MINUTE;

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

