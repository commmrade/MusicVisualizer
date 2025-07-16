#include "visualizerwidget.h"
#include "ui_visualizerwidget.h"

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
