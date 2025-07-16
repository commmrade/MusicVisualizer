#ifndef CONTROLLERWIDGET_H
#define CONTROLLERWIDGET_H

#include <QWidget>
#include "musiccontroller.h"

namespace Ui {
class ControllerWidget;
}

class ControllerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControllerWidget(QWidget *parent = nullptr);
    ~ControllerWidget();

    void loadMusic(QStringView path);
private slots:
    void on_volumeSlider_valueChanged(int value);
    void on_playButton_clicked();
    void on_muteButton_clicked();
signals:
    void volumeChanged(int value);
    void playPressed();
    void mutePressed();
private:
    Ui::ControllerWidget *ui;
    MusicController m_musicController;

    float m_oldVolume;
    bool m_isMuted;
};

#endif // CONTROLLERWIDGET_H
