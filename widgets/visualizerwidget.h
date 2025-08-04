#ifndef VISUALIZERWIDGET_H
#define VISUALIZERWIDGET_H

#include <QWidget>
#include <QAudioBuffer>
#include <array>
#include "ringbuffer.h"
#include "visualizercontroller.h"

namespace Ui {
class VisualizerWidget;
}

class VisualizerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizerWidget(QWidget *parent = nullptr);
    ~VisualizerWidget();

public slots:
    void bufferAccept(std::array<char, DEFAULT_RINGBUF_SIZE> buffer, QAudioFormat format);

private:
    void paintEvent(QPaintEvent* event) override;
    QList<double> freqBins;

    Ui::VisualizerWidget *ui;
    VisualizerController m_visualizerController;
};

#endif // VISUALIZERWIDGET_H
