#include "musiccontroller.h"
#include <QFile>
#include <QDebug>
#include <spdlog/spdlog.h>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <algorithm>

static constexpr double DEFAULT_SOUND_VOLUME = 0.5;

MusicController::MusicController(QObject *parent)
    : QObject{parent}
{
    m_decoder = new QAudioDecoder{this};
    m_audioFile = new QFile(this);

    connect(m_decoder, &QAudioDecoder::bufferReady, this, &MusicController::bufferDecoded);
    connect(&m_pushTimer, &QTimer::timeout, this, &MusicController::audioLoop);
    connect(m_decoder, &QAudioDecoder::finished, this, [this] {
        spdlog::info("Finished processing audio");
    });
    connect(m_decoder, QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), [this](QAudioDecoder::Error err) {
        m_pushTimer.stop(); // Stop timer of decoding failed, so we dont waste resources
        spdlog::error("Error decoding audio: {}", static_cast<int>(err));
        emit errorDecoding(err);
    });

}

void MusicController::loadMusic(TagLib::FileRef fileRef)
{
    m_audioSamples.clear();
    m_decoder->stop();

    if (m_audioSink) {
        m_totalLengthSecs.reset();
        delete m_audioSink;
        m_audioSink = nullptr; // Can't use QAudioSink::reset, since QAudioSink interface doesn't offer a way of setting a different QAudioFormat

        m_isPlaying = false;
        if (m_audioFile->isOpen()) {
            m_audioFile->close();
            m_audioFile->flush();
        }
        m_audioDevice = nullptr;
    }

    auto filepath = fileRef.file()->name();
    m_audioFile->setFileName(QString{filepath});
    m_audioFile->open(QIODevice::ReadOnly); // Not likely to not open, since we already tried to open it in mainwindow when taglib read it.

    m_totalLengthSecs = fileRef.file()->audioProperties()->lengthInSeconds();
    m_decoder->setSourceDevice(m_audioFile);
    m_decoder->start();
}

void MusicController::setMusicElapsed(int value)
{
    auto sampleIter = std::find_if(m_audioSamples.begin(), m_audioSamples.end(), [value](auto&& sample) {
        return static_cast<int>(sample.startTime() / 1e+6) == value;
    });
    if (sampleIter == m_audioSamples.end()) {
        return;
    }
    std::rotate(m_audioSamples.begin(), sampleIter, m_audioSamples.end());
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

void MusicController::bufferDecoded()
{
    QAudioBuffer buffer = m_decoder->read();
    if (buffer.isValid()) {
        if (!m_audioSink) {
            QAudioFormat format = buffer.format();

            m_audioSink = new QAudioSink(format, this);
            m_audioDevice = m_audioSink->start();
            m_audioSink->setVolume(DEFAULT_SOUND_VOLUME);
            m_isPlaying = true;
            emit okDecoding();
        }
        m_pushTimer.start();
        m_audioSamples.append(std::move(buffer));
    }
}

void MusicController::audioLoop()
{
    if (m_audioSink && m_audioSink->bytesFree()) {
        // Maybe check for empty
        auto buffer = m_audioSamples.front();
        m_audioSamples.append(buffer);
        m_audioSamples.pop_front();

        const auto* bufData = buffer.constData<char>();
        const auto bufDataSize = buffer.byteCount();

        m_audioDevice->write(bufData, bufDataSize);
        m_ringBuffer.push_range(std::span<const char>{bufData, static_cast<std::size_t>(bufDataSize)});

        emit bufferReady(m_ringBuffer.get_data(), buffer.format());
        emit elapsedChanged(static_cast<int>(buffer.startTime() / 1e+6), m_totalLengthSecs.value_or(1));

        double durationMs = buffer.duration() / 1000;
        m_pushTimer.start(static_cast<int>(std::round(durationMs)));
    }
}
