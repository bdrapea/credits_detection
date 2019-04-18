#include "crde_credits_view.h"

namespace crde
{
namespace gui
{
credits_view::credits_view(QWidget* parent) : QGraphicsView (parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    init_widgets();
    for(std::size_t i=0; i<100; i++)
    {
        m_credits_timelines.push_back(new credits_timeline("yo",this));
    }
    organize_widgets();
}

void credits_view::init_widgets()
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    m_timelines_layout = new QVBoxLayout;
}

void credits_view::organize_widgets()
{
    std::size_t timeline_count = m_credits_timelines.size();
    for(std::size_t i=0; i<timeline_count; i++)
        m_timelines_layout->addWidget(m_credits_timelines[i]);

    QWidget* timelines = new QWidget;
    timelines->setLayout(m_timelines_layout);
    m_scene->addWidget(timelines);
}
}
}
