#pragma once

#include <iostream>

#include <QWidget>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QGroupBox>
#include <QtCharts>
#include <QProgressBar>

#include "crde_qt_utils.h"

using namespace QtCharts;

namespace crde
{
namespace gui
{
class credits_timeline : public QGroupBox
{
    Q_OBJECT

private:
    QGridLayout* m_main_layout;
    QSlider* m_timeline;
    QChartView* m_chart_view;
    QChart* m_chart;
    QProgressBar* m_progress;
    QLineSeries* m_series;
    QLineSeries* m_cursor;
    QLineSeries* m_credits_start;
    QLineSeries* m_credits_end;
    QLabel* m_infos;

public:
    credits_timeline(const QString& title,QWidget* parent = nullptr);

    void init_widgets();
    void organize_widgets();
    friend class main_window;
    friend class credits_view;
};
}
}
