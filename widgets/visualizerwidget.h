#ifndef VISUALIZERWIDGET_H
#define VISUALIZERWIDGET_H

#include <QWidget>
#include <QAudioBuffer>

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
    void bufferAccept(QAudioBuffer buffer);

private:
    void paintEvent(QPaintEvent* event) override;

    QAudioBuffer buffer;
    QList<double> freqBins;

    Ui::VisualizerWidget *ui;
};

#endif // VISUALIZERWIDGET_H
