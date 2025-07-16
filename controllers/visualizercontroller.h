#ifndef VISUALIZERCONTROLLER_H
#define VISUALIZERCONTROLLER_H

#include <QObject>

class VisualizerController : public QObject
{
    Q_OBJECT
public:
    explicit VisualizerController(QObject *parent = nullptr);

signals:
};

#endif // VISUALIZERCONTROLLER_H
