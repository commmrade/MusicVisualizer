#include "musiccontroller.h"
#include <QFile>
#include <QDebug>

MusicController::MusicController(QObject *parent)
    : QObject{parent}
{
    m_decoder = new QAudioDecoder{this};
    m_audioFile = new QFile(this);
}

static constexpr double DEFAULT_SOUND_VOLUME = 0.5;

void MusicController::loadMusic(QStringView path)
{
    m_audioSamples.clear();
    m_decoder->stop();
    m_decoder->setSourceDevice(nullptr);

    if (m_audioSink) {
        delete m_audioSink;
        m_audioSink = nullptr; // Can't use QAudioSink::reset, since QAudioSink interface doesn't offer a way of setting a different QAudioFormat
        m_isPlaying = false;
        if (m_audioFile->isOpen()) {
            m_audioFile->close();
            m_audioFile->flush();
        }
        m_audioDevice = nullptr;
        delete m_audioDevice;
    }

    m_audioFile->setFileName(QString{path});
    if (!m_audioFile->open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Such file does not exist");
    }

    m_decoder->setSourceDevice(m_audioFile);
    m_decoder->start();

    connect(m_decoder, &QAudioDecoder::bufferReady, this, [this] mutable {
        QAudioBuffer buffer = m_decoder->read();
        if (buffer.isValid()) {
            if (!m_audioSink) {
                QAudioFormat format = buffer.format();
                m_audioSink = new QAudioSink(format, this);
                m_audioDevice = m_audioSink->start();
                m_audioSink->setVolume(DEFAULT_SOUND_VOLUME);
                m_isPlaying = true;
            }
            m_audioSamples.append(std::move(buffer));
            connect(&m_pushTimer, &QTimer::timeout, this, [this] {
                int bytesFree = m_audioSink->bytesFree();
                if (bytesFree) {
                    // Maybe check for empty
                    auto buffer = m_audioSamples.front();
                    m_audioSamples.pop_front();
                    m_audioSamples.append(buffer);

                    if (buffer.startTime() > m_audioSamples.front().startTime()) { // Not playing old samples if new samples arent loaded yet
                        return; // Also this way audio cycles
                    }
                    m_audioDevice->write(buffer.constData<char>(), buffer.byteCount());
                }
            });
        }
    });

    m_pushTimer.start(20);
}

void MusicController::setVolume(int value)
{
    if (m_audioSink) {
        m_isMuted = false;
        m_audioSink->setVolume(static_cast<float>(value) / 100.f);
    }
}


void MusicController::playOrPause()
{
    if (m_audioSink) {
        if (m_isPlaying) {
            m_audioSink->suspend();
            m_isPlaying = !m_isPlaying;
        } else {
            m_audioSink->resume();
            m_isPlaying = !m_isPlaying;
        }
    }
}

void MusicController::mute()
{

    if (!m_isMuted) {
        m_oldVolume = m_audioSink->volume();
        m_audioSink->setVolume(0.0);

        emit setSliderVolume(0.f);
        m_isMuted = !m_isMuted;
    } else {
        m_audioSink->setVolume(m_oldVolume);

        emit setSliderVolume(m_oldVolume);

        m_isMuted = false;
    }
}
