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
    QPen start_pen = QPen(Qt::green,1);
    QPen end_pen = QPen(Qt::red,1);
        m_credits_start->setPen(start_pen);

        m_credits_end->setPen(end_pen);
        m_credits_start->setName("searching start tc");
        m_credits_end->setName("searching end tc");

    m_chart->addSeries(m_series);
    m_chart->addSeries(m_cursor);
    m_chart->addSeries(m_credits_start);
    m_chart->addSeries(m_credits_end);

    QFont marker_font = QFont("Arial");
    marker_font.setBold(true);

    for(QLegendMarker* marker : m_chart->legend()->markers())
    {
        marker->setVisible(false);
        marker->setFont(marker_font);
    }

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
}

}
}
