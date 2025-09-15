#include "visualizerwidget.h"
#include "ui_visualizerwidget.h"
#include <spdlog/spdlog.h>
#include <QPainter>
#include <print>
#include <fftw3.h>
#include <QSettings>
#include "settingsvalues.h"

constexpr static const char* DEFAULT_BAR_COLOR = "#ffffff";

constexpr static double MAX_VALUE = 200.0;
constexpr static double MIN_VALUE = 0.0;
// constexpr static int BAR_WIDTH = 10;

constexpr static int Y_PADDING = 200;

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

void VisualizerWidget::clearBuffer()
{
    this->freqBins.clear();
    update();
}

void VisualizerWidget::paintEvent(QPaintEvent *event)
{

    if (freqBins.isEmpty()) {
        return;
    }
    QPainter painter(this);

    QSettings settings;
    auto colorString = settings.value(SettingsValues::BARS_COLOR, DEFAULT_BAR_COLOR).toString();
    QColor color = QColor::fromString(colorString);

    auto posX = 0;
    auto widgetWidth = width();
    auto barWidth = widgetWidth / freqBins.size();
    for (auto magnitude : freqBins) {
        auto finalValue = std::clamp(magnitude * MAX_VALUE, MIN_VALUE, MAX_VALUE); // Масштабирование
        painter.fillRect(posX, Y_PADDING, barWidth, -finalValue, color);
        posX += barWidth * 1.2; // leaving space between bars
    }
}
