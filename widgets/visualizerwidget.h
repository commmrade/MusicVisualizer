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

    void paintEvent(QPaintEvent* event) override;
public:
    explicit VisualizerWidget(QWidget *parent = nullptr);
    ~VisualizerWidget();

public slots:
    void acceptBuffer(QAudioBuffer buffer);

private:
    Ui::VisualizerWidget *ui;

    QAudioBuffer m_buffer;

    QPainter* m_painter;
};

#endif // VISUALIZERWIDGET_H
