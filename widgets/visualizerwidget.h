#ifndef VISUALIZERWIDGET_H
#define VISUALIZERWIDGET_H

#include <QWidget>

namespace Ui {
class VisualizerWidget;
}

class VisualizerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizerWidget(QWidget *parent = nullptr);
    ~VisualizerWidget();

private:
    Ui::VisualizerWidget *ui;
};

#endif // VISUALIZERWIDGET_H
