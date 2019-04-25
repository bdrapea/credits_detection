#include "crde_credits_view.h"

namespace crde
{
namespace gui
{
credits_view::credits_view(QWidget* parent) : QGraphicsView (parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
    m_thumbnail = new QGraphicsPixmapItem(QPixmap(100,90));
    m_thumbnail->setZValue(2);
    m_scene->addItem(m_thumbnail);
    m_thumbnail->setVisible(false);

    for(std::size_t i=0; i<nb_timelines; i++)
    {
        m_credits_timelines.push_back(
                    new credits_timeline(names[i].c_str()));
        int cred_y = m_credits_timelines[0]->height()*static_cast<int>(i);
        m_scene->addWidget(m_credits_timelines[i])->setY(cred_y);
        m_credits_timelines[i]->m_timeline->setMaximum(NB_BIFF_SLIDER);

        m_credits_timelines[i]->m_chart->axes()[0]->setMax(7500);

        connect(m_credits_timelines[i]->m_timeline, &QSlider::valueChanged,
                [i,this](const std::size_t slider_val)
        {
            if(m_credits_timelines[i]->m_timeline->isSliderDown())
            {
                m_thumbnail->setVisible(true);
                show_thumbnail(i,slider_val);
            }
        });
        connect(m_credits_timelines[i]->m_timeline, &QSlider::sliderReleased,
                [this]()
        {
            m_thumbnail->setVisible(false);
        });

    }
}

void credits_view::organize_widgets()
{
    QWidget* timelines = new QWidget;
    timelines->setLayout(m_timelines_layout);
    m_scene->addWidget(timelines);
}

void credits_view::load_pixmaps(const boost::filesystem::path &folder_path,
                                const std::size_t pix_count)
{
    boost::filesystem::directory_iterator it(folder_path);
    boost::filesystem::directory_iterator end_it;
    boost::system::error_code err_code;

    std::vector<boost::filesystem::path> paths;
    paths.reserve(pix_count);

    std::copy(it,end_it,std::back_inserter(paths));
    std::sort(paths.begin(), paths.end());

    std::size_t inter = static_cast<std::size_t>(static_cast<float>(pix_count)/
            static_cast<float>(NB_BIFF_SLIDER));
    std::vector<QPixmap> pixs;
    pixs.reserve(NB_BIFF_SLIDER);

    for(std::size_t i=0; i<pix_count; i+=inter)
    {
        pixs.emplace_back(paths[i].c_str());
    }
    m_thumbnail_pixmaps.emplace_back(pixs);
}

void credits_view::show_thumbnail(
        const std::size_t episode, const std::size_t frame)
{
    m_thumbnail->setPixmap(m_thumbnail_pixmaps[episode][frame]);
    m_thumbnail->setY(static_cast<qreal>(m_credits_timelines[0]->height())
            *episode);
    m_thumbnail->setX(
        static_cast<qreal>(
            m_credits_timelines[0]->m_timeline->width()/NB_BIFF_SLIDER)*frame
            + m_credits_timelines[0]->m_timeline->x());
}

void credits_view::load_graphs()
{
    boost::filesystem::path graph_paths =
            boost::filesystem::path(__FILE__).parent_path().
            parent_path().parent_path();
    graph_paths /= "graphs";

    std::vector< boost::filesystem::path > graphs;
    std::copy(boost::filesystem::directory_iterator(graph_paths),
              boost::filesystem::directory_iterator(),
              std::back_inserter(graphs));
    std::sort(graphs.begin(), graphs.end());

    for(std::size_t i=0; i<graphs.size(); i++)
    {
        std::ifstream file(graphs[i].c_str());
        int tmp = 0;
        std::vector<int> series;
        std::size_t count =0;
        series.reserve(7500);
        while(file >> tmp)
        {
            m_credits_timelines[i]->m_series->append(count,tmp);
            count++;
        }
        file.close();

        unlink(graphs[i].c_str());
    }
}


}
}
