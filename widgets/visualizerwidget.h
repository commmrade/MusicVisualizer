#ifndef VISUALIZERWIDGET_H
#define VISUALIZERWIDGET_H

#include <QWidget>
#include <QAudioBuffer>
#include <array>
#include "ringbuffer.h"

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
};

#endif // VISUALIZERWIDGET_H
