#include "crde_credits_timeline.h"

namespace crde
{
namespace gui
{
credits_timeline::credits_timeline(
        const QString& title, QWidget* parent) : QGroupBox(title, parent)
{
    setMinimumWidth(
                static_cast<int>(
                    static_cast<float>(
                        qApp->primaryScreen()->availableSize().width()*0.9f)));
    init_widgets();
    organize_widgets();
}

void credits_timeline::init_widgets()
{
    m_main_layout = new QGridLayout(this);
    m_timeline = new QSlider(Qt::Horizontal,this);
    m_progress = new QProgressBar(this);

    m_chart = new QChart;
    m_series = new QLineSeries(this);
    m_cursor = new QLineSeries(this);
        m_cursor->setPen(QPen(Qt::black));
    m_credits_start = new QLineSeries(this);
    m_credits_end = new QLineSeries(this);
        m_credits_start->setPen(QPen(Qt::green,1));
        m_credits_end->setPen(QPen(Qt::red,1));

    m_infos = new QLabel("processing",this);

    m_chart->addSeries(m_series);
    m_chart->addSeries(m_cursor);
    m_chart->addSeries(m_credits_start);
    m_chart->addSeries(m_credits_end);

    m_chart->createDefaultAxes();
    m_chart_view = new QChartView(m_chart);
    m_chart_view->setMinimumHeight(height());
    m_chart_view->setRenderHint(QPainter::Antialiasing);
}

void credits_timeline::organize_widgets()
{
    const int widget_width = 10;
    m_main_layout->addWidget(m_timeline,0,0,1,widget_width);
    m_main_layout->addWidget(m_chart_view,1,0);
    m_main_layout->addWidget(m_progress,2,0);
    m_main_layout->addWidget(m_infos,3,0);
}

void credits_timeline::print_timecodes(const std::string& tc1,
                                       const std::string& tc2)
{
}

}
}
