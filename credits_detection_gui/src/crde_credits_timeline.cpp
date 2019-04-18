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
    m_chart_view = new QChartView(this);
    m_chart = new QChart;
}

void credits_timeline::organize_widgets()
{
    const int widget_width = 10;
    m_main_layout->addWidget(m_timeline,0,0,1,widget_width);
    m_main_layout->addWidget(m_chart_view,1,0);
}
}
}
