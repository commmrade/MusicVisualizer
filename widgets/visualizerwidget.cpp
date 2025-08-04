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
    this->freqBins = m_visualizerController.processAudio(buffer, format);
    update();
}

void VisualizerWidget::paintEvent(QPaintEvent *event)
{
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
