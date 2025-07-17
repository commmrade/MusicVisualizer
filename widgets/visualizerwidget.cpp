#include "visualizerwidget.h"
#include "ui_visualizerwidget.h"
#include <QPainter>
#include <spdlog/spdlog.h>
#include <QRandomGenerator>
#include <vector>
#include <complex>
#include <cmath>

VisualizerWidget::VisualizerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VisualizerWidget)
{
    ui->setupUi(this);
    m_painter = new QPainter(this);

}

VisualizerWidget::~VisualizerWidget()
{

    delete ui;
}

void VisualizerWidget::acceptBuffer(QAudioBuffer buffer)
{
    m_buffer = std::move(buffer);
    update();
}

using cd = std::complex<double>;
void fft(std::vector<cd> & a, bool invert) {
    int n = a.size();
    if (n == 1)
        return;

    std::vector<cd> a0(n / 2), a1(n / 2);
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


void VisualizerWidget::paintEvent(QPaintEvent *event)
{
    if (!m_buffer.isValid()) return;

    m_painter->begin(this);

    if (m_buffer.format().sampleFormat() == QAudioFormat::SampleFormat::Int16) {
        auto data = m_buffer.constData<int16_t>();
        auto size = m_buffer.byteCount() / m_buffer.format().bytesPerSample();
        int16_t max_el = 1; // avoid div by zero
        for (auto i = 0; i < size; ++i) {
            max_el = std::max(max_el, static_cast<int16_t>(std::abs(data[i])));
        }

        std::vector<std::complex<double>> samples(size);
        for (auto i = 0; i < size; ++i) {
            samples[i] = std::complex<double>{(double)data[i] / max_el};
        }
        fft(samples, false);

        auto pos_x = 0;
        for (auto i = 0; i < size / 50; ++i) {
            double sum = 0;
            for (auto j = 0; j < 50; ++j) {
                sum += std::abs(samples[i * 50 + j]); // use magnitude of complex number
            }

            float avg = static_cast<float>(sum / 1000.0);

            float final_value = std::clamp(avg * 200.0f, 0.0f, 200.0f);

            m_painter->fillRect(pos_x, 200, 30, -final_value, QColor{}.blue());
            pos_x += 35;
        }
    } else if (m_buffer.format().sampleFormat() == QAudioFormat::SampleFormat::Float) {
        auto data = m_buffer.constData<float>();
        auto size = m_buffer.byteCount() / m_buffer.format().bytesPerSample();

        auto pos_x = 0;

        for (auto i = 0; i < size / 50; ++i) {
            float sum = 0;
            for (auto j = 0; j < 50; ++j) {
                sum += std::fabs(data[i * 50 + j]);
            }

            float avg = sum / 50.0f;
            // float norm = avg / 32768.0f;

            float final_value = std::clamp(avg * 200.0f, 0.0f, 200.0f);

            m_painter->fillRect(pos_x, 200, 30, -final_value, QColor{}.blue());
            pos_x += 35;
        }
    }




    m_painter->end();

}
