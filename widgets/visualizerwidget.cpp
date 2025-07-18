#include "visualizerwidget.h"
#include "ui_visualizerwidget.h"
#include <spdlog/spdlog.h>
#include <complex>
#include <QPainter>

VisualizerWidget::VisualizerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VisualizerWidget)
{
    ui->setupUi(this);
}

VisualizerWidget::~VisualizerWidget()
{
    delete ui;
}

inline bool isPowerOfTwo(int x)
{
    return (x & (x - 1)) == 0;
}

int highestPowerOfTwo(int n)
{
    int res = 0;
    for (int i = n; i >= 1; i--) {
        // If i is a power of 2
        if (isPowerOfTwo(i)) {
            res = i;
            break;
        }
    }
    return res;
}


using cd = std::complex<double>;
void fft(QList<cd> & a, bool invert = false) {
    int n = a.size();
    if (n == 1)
        return;

    QList<cd> a0(n / 2), a1(n / 2);
    for (int i = 0; 2 * i < n; i++) {
        a0[i] = a[2*i];
        a1[i] = a[2*i+1];
    }
    fft(a0, invert);
    fft(a1, invert);

    double ang = 2 * std::numbers::pi / n * (invert ? -1 : 1);
    cd w(1), wn(cos(ang), sin(ang));
    for (int i = 0; 2 * i < n; i++) {
        a[i] = a0[i] + w * a1[i];
        a[i + n/2] = a0[i] - w * a1[i];
        if (invert) {
            a[i] /= 2;
            a[i + n/2] /= 2;
        }
        w *= wn;
    }
}



void VisualizerWidget::bufferAccept(QAudioBuffer buffer)
{
    // this->buffer = std::move(buffer);
    if (!buffer.isValid()) return;
    QAudioFormat format = buffer.format();
    // if (format.channelCount() != 1) throw std::runtime_error("Fuck");
    if (buffer.byteCount() / format.bytesPerSample() < 100) return;
    size_t size = buffer.byteCount() / format.bytesPerSample();
    if (!isPowerOfTwo(size)) {
        size = highestPowerOfTwo(size);
    }

    QList<std::complex<double>> samples;

    switch (format.sampleFormat()) {
        case QAudioFormat::SampleFormat::Float: {
            const float* raw_samples = buffer.constData<float>();

            for (auto i = 0; i < size; i += format.channelCount()) {
                double t = static_cast<double>(i) / (size - 1);
                double hann = 0.5 - 0.5 * std::cosf(2 * std::numbers::pi * t);

                auto sum = 0.0;
                for (auto j = 0; j < format.channelCount(); ++j) {
                    sum += raw_samples[i + j];
                }
                sum /= format.channelCount();

                samples.emplaceBack(static_cast<double>(sum * hann));
            }

            break;
        }
        case QAudioFormat::SampleFormat::Int16: {
            auto* raw_samples = buffer.constData<int16_t>();

            for (auto i = 0; i < size; ++i) {
                double t = static_cast<double>(i) / (size - 1);
                double hann = 0.5 - 0.5 * std::cosf(2 * std::numbers::pi * t);

                auto sum = 0.0;
                for (auto j = 0; j < format.channelCount(); ++j) {
                    sum += raw_samples[i + j];
                }
                sum /= format.channelCount();

                samples.emplaceBack(static_cast<double>(sum * hann));
            }
            break;
        }
        default: {
            break;
        }
    }

    fft(samples);

    // Calc amplitudes and normalize them
    QList<double> amplitudes(size / 2);
    for (auto i = 0; i < size / 2; ++i) {
        amplitudes[i] = 20.0 * std::log10(std::fabs(samples[i])); // Децибелы
        // amplitudes[i] = std::fabs(samples[i]);
    }

    // Нормализую децибелы
    auto maxAmp = *std::ranges::max_element(amplitudes, {}, [](auto x) {
        return std::fabs(x);
    });
    std::ranges::for_each(amplitudes, [=](auto& value) {
        value /= maxAmp;
    });


    // Split amplitudes into freq bins
    constexpr int binCnt = 80;
    const int binWidth = amplitudes.size() / binCnt;
    QList<double> freqBins(binCnt, 0.0);

    for (auto i = 0; i < amplitudes.size(); ++i) {
        int idx = i / binWidth;
        if (idx < binCnt) {
            freqBins[idx] += amplitudes[i];
        }
    }

    // normalize freq bins
    auto maxFreq = *std::ranges::max_element(freqBins);
    std::ranges::for_each(freqBins, [=](auto& value) {
        value /= maxFreq;
    });
    this->freqBins = std::move(freqBins);


    update();
}

void VisualizerWidget::paintEvent(QPaintEvent *event)
{
    // spdlog::info("here");
    QPainter painter(this);

    painter.setPen(Qt::blue);
    auto posX = 0;
    for (auto amplitude : freqBins) {
        auto finalValue = std::clamp(amplitude * 200.0, 0.0, 200.0); // Масштабирование
        painter.fillRect(posX, 200, 5, -finalValue, Qt::blue);
        posX += 7;
    }
}
