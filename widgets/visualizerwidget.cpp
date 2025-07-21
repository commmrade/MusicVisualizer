#include "visualizerwidget.h"
#include "ui_visualizerwidget.h"
#include <spdlog/spdlog.h>
#include <complex>
#include <QPainter>
#include <print>
#include <fftw3.h>

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

void VisualizerWidget::bufferAccept(std::array<char, DEFAULT_RINGBUF_SIZE> buffer, QAudioFormat format)
{
    size_t size = buffer.size() / format.bytesPerSample();
    QList<double> samples;

    switch (format.sampleFormat()) {
        case QAudioFormat::SampleFormat::Float: {
            const float* raw_samples = reinterpret_cast<const float*>(buffer.data());
            // qDebug() << "float";
            for (auto i = 0; i < size; i += format.channelCount()) {
                double t = static_cast<double>(i) / (size - 1);
                double hann = 0.5 - 0.5 * std::cos(2 * std::numbers::pi * t);
                // double hann = 1.0;
                samples.emplaceBack(static_cast<double>(raw_samples[i] * hann));
            }

            break;
        }
        case QAudioFormat::SampleFormat::Int16: {
            auto* raw_samples = reinterpret_cast<const int16_t*>(buffer.data());

            for (auto i = 0; i < size; i += format.channelCount()) {
                double t = static_cast<double>(i) / (size - 1);
                double hann = 0.5 - 0.5 * std::cos(2 * std::numbers::pi * t);
                // double hann = 1.0;
                double normalized = static_cast<double>(raw_samples[i] / 32768.0);
                samples.emplaceBack(normalized * hann); // take onlty 1 channel
            }

            break;
        }
        default: {
            break;
        }
    }

    size = samples.size();
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
    fftw_plan plan = fftw_plan_dft_r2c_1d(size, samples.data(), out, FFTW_ESTIMATE);
    fftw_execute(plan);

    // // Calc amplitudes and normalize them
    QList<double> magnitudes(size / 2);
    for (auto i = 0; i < size / 2; ++i) {
        auto real = out[i][0];
        auto img = out[i][1];
        auto value = std::sqrt(real * real + img * img); // == std::abs(complex)

        magnitudes[i] = value;
    }
    auto maxAmp = *std::ranges::max_element(magnitudes, {}, [](auto x) {
        return std::fabs(x);
    });

    if (maxAmp != 0) {
        std::ranges::for_each(magnitudes, [=](auto& value) {
            value /= maxAmp;
        });
    }


    constexpr int binCnt = 40;
    const int binWidth = magnitudes.size() / binCnt;

    QList<double> freqBins(binCnt, 0.0);

    for (auto i = 0; i < magnitudes.size(); ++i) {
        int idx = i / binWidth;
        if (idx < binCnt) {
            freqBins[idx] += magnitudes[i];
        }
    }

    // normalize freq bins
    auto maxFreq = *std::ranges::max_element(freqBins);

    if (maxFreq > 0) {
        std::ranges::for_each(freqBins, [=](auto& value) {
            value /= maxFreq;
        });
    }

    this->freqBins = std::move(freqBins);

    update();
    fftw_free(out);
    fftw_destroy_plan(plan);
}

void VisualizerWidget::paintEvent(QPaintEvent *event)
{
    // spdlog::info("here");
    QPainter painter(this);

    painter.setPen(Qt::blue);
    auto posX = 0;
    for (auto amplitude : freqBins) {
        // qDebug() << amplitude;
        auto finalValue = std::clamp(amplitude * 200.0, 0.0, 200.0); // Масштабирование
        painter.fillRect(posX, 200, 10, -finalValue, Qt::blue);
        posX += 13;
    }
}
