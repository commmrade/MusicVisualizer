#ifndef MUSICCONTROLLER_H
#define MUSICCONTROLLER_H

#include <QObject>
#include <QAudioSink>
#include <QIODevice>
#include <QAudioDecoder>
#include <QList>
#include <QTimer>
#include <QFile>
#include "ringbuffer.h"
#include <fileref.h>

class MusicController : public QObject
{
    Q_OBJECT


public:
    explicit MusicController(QObject *parent = nullptr);

    void loadMusic(TagLib::FileRef fileRef);
    void setMusicElapsed(int value);
signals:
    void setSliderVolume(float value);

    void bufferReady(std::array<char, DEFAULT_RINGBUF_SIZE> samples, QAudioFormat format);
    void elapsedChanged(int elapsedSecs, int totalSecs);

    void errorDecoding(QAudioDecoder::Error error);
    void okDecoding();
public slots:

    void setVolume(int value);
    void playOrPause();
    void mute();
    void bufferDecoded();

    void audioLoop();
private:
    QAudioDecoder* m_decoder{nullptr};
    QAudioSink* m_audioSink{nullptr};

    QIODevice* m_audioDevice{nullptr};
    QFile* m_audioFile{nullptr};

    QList<QAudioBuffer> m_audioSamples;
    QTimer m_pushTimer;
    QList<float> m_circularBuffer;

    bool m_isPlaying{false}; //  used for pause unpause
    bool m_isMuted{false};
    float m_oldVolume;

    std::optional<int> m_totalLengthSecs{std::nullopt};

    RingBuffer<char, DEFAULT_RINGBUF_SIZE> m_ringBuffer;
};

#endif // MUSICCONTROLLER_H
