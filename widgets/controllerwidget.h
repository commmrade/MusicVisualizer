#ifndef CONTROLLERWIDGET_H
#define CONTROLLERWIDGET_H

#include <QWidget>
#include "musiccontroller.h"
#include <taglib/fileref.h>

namespace Ui {
class ControllerWidget;
}

class ControllerWidget : public QWidget
{
    Q_OBJECT


public:


    explicit ControllerWidget(QWidget *parent = nullptr);
    ~ControllerWidget();

    void loadMusic(TagLib::FileRef fileRef);
private slots:
    void on_volumeSlider_valueChanged(int value);
    void on_playButton_clicked();
    void on_muteButton_clicked();

    void on_seekBar_sliderMoved(int position);

public slots:
    void setSeekBarValue(int elapsed, int total);
signals:
    void volumeChanged(int value);
    void playPressed();
    void mutePressed();
    void bufferReady(std::array<char, DEFAULT_RINGBUF_SIZE> samples, QAudioFormat format);
private:
    MusicController m_musicController;
    Ui::ControllerWidget *ui;
    float m_oldVolume;
    bool m_isMuted;
};

#endif // CONTROLLERWIDGET_H
