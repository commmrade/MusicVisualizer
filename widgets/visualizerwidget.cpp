#include "visualizerwidget.h"
#include "ui_visualizerwidget.h"
#include <spdlog/spdlog.h>
#include <QPainter>
#include <print>
#include <fftw3.h>
#include <QSettings>

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

void VisualizerWidget::bufferAccept(std::array<char, DEFAULT_RINGBUF_SIZE> buffer, QAudioFormat format)
{
    size_t size = buffer.size() / format.bytesPerSample();
    QList<double> samples;
    switch (format.sampleFormat()) {
        case QAudioFormat::SampleFormat::Float: {
            const float* raw_samples = reinterpret_cast<const float*>(buffer.data());
            for (auto i = 1; i < size; i += format.channelCount()) {
                double t = static_cast<double>(i) / (size - 1);
                double hann = 0.5 - 0.5 * std::cos(2 * std::numbers::pi * t);
                samples.emplaceBack(static_cast<double>(raw_samples[i] * hann));
            }
            break;
        }
        case QAudioFormat::SampleFormat::Int16: {
            const int16_t* raw_samples = reinterpret_cast<const int16_t*>(buffer.data());
            for (auto i = 1; i < size; i += format.channelCount()) {
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
    auto samplesSize = samples.size();
    int fftSize = samplesSize / 2 + 1;

    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
    fftw_plan plan = fftw_plan_dft_r2c_1d(samplesSize, samples.data(), out, FFTW_ESTIMATE);
    fftw_execute(plan);

    // Calc amplitudes and normalize them
    QList<double> magnitudes(fftSize);
    for (auto i = 0; i < fftSize; ++i) {
        auto real = out[i][0];
        auto img = out[i][1];
        auto value = std::sqrt(real * real + img * img); // == std::abs(complex)

        magnitudes[i] = value;
    }
    auto maxAmp = *std::ranges::max_element(magnitudes);

    if (maxAmp != 0) {
        std::ranges::for_each(magnitudes, [=](auto& value) {
            value /= maxAmp;
        });
    }


    constexpr int binCnt = 80;
    QList<double> freqBins(binCnt, 0.0);
    QList<int> binCounts(binCnt, 0);

    // Приводим частоту к лог шкале
    for (int i = 1; i < fftSize; ++i) {
        double norm = static_cast<double>(i) / (fftSize - 1); // от 0 до 1
        int logIndex = static_cast<int>(std::log10(1 + 9 * norm) * binCnt); // log10(1..10)
        if (logIndex >= binCnt)
            logIndex = binCnt - 1;

        freqBins[logIndex] += magnitudes[i];
        binCounts[logIndex]++;
    }

    for (int i = 0; i < binCnt; ++i) {
        if (binCounts[i] > 0)
            freqBins[i] /= binCounts[i];
    }


    double maxVal = *std::ranges::max_element(freqBins);
    if (maxVal > 1e-6) {
        std::ranges::for_each(freqBins, [=](auto& value) {
            value /= maxVal;
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

    QSettings settings;
    auto colorString = settings.value("Bars/Color", QString("#ffffff")).toString();
    QColor color = QColor::fromString(colorString);

    auto posX = 0;
    for (auto magnitude : freqBins) {
        // qDebug() << amplitude;
        auto finalValue = std::clamp(magnitude * 200.0, 0.0, 200.0); // Масштабирование
        painter.fillRect(posX, 200, 10, -finalValue, color);
        posX += 13;
    }
}
