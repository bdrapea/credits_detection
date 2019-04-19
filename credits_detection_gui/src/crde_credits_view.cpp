#include "crde_credits_view.h"

namespace crde
{
namespace gui
{
credits_view::credits_view(QWidget* parent) : QGraphicsView (parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    init_widgets();
    organize_widgets();
}

void credits_view::init_widgets()
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    m_timelines_layout = new QVBoxLayout;
}

void credits_view::init_scene(
        const std::size_t nb_timelines, const std::vector<std::string>& names)
{
    m_credits_timelines.reserve(nb_timelines);
    m_thumbnails.reserve(nb_timelines);

    for(std::size_t i=0; i<nb_timelines; i++)
    {
        m_credits_timelines.push_back(
                    new credits_timeline(names[i].c_str()));
        int cred_y = m_credits_timelines[0]->height()*static_cast<int>(i);
        m_scene->addWidget(m_credits_timelines[i])->setY(cred_y);
    }
}

void credits_view::organize_widgets()
{
    QWidget* timelines = new QWidget;
    timelines->setLayout(m_timelines_layout);
    m_scene->addWidget(timelines);
}
}
}
