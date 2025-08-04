#ifndef VISUALIZERCONTROLLER_H
#define VISUALIZERCONTROLLER_H

#include <QObject>
#include "ringbuffer.h"
#include <QAudioFormat>

class VisualizerController : public QObject
{
    Q_OBJECT
public:
    explicit VisualizerController(QObject *parent = nullptr);

    QList<double> processAudio(const std::array<char, DEFAULT_RINGBUF_SIZE>& buffer, const QAudioFormat& format);
signals:
};

#endif // VISUALIZERCONTROLLER_H
