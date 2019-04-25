#pragma once

#include <vector>
#include <iostream>
#include <fstream>

#include <QApplication>
#include <QGraphicsView>
#include <QDesktopWidget>
#include <QResizeEvent>
#include <QMessageBox>

#include <boost/filesystem.hpp>

#include "crde_credits_timeline.h"

#define NB_BIFF_SLIDER 300

namespace crde
{
namespace gui
{
class credits_view : public QGraphicsView
{
    Q_OBJECT

private:
    QGraphicsScene* m_scene;
    std::vector< credits_timeline* > m_credits_timelines;
    QVBoxLayout* m_timelines_layout;
    QGraphicsPixmapItem* m_thumbnail;
    std::vector< std::vector<QPixmap>> m_thumbnail_pixmaps;

public:
    credits_view(QWidget* parent = nullptr);

    void init_widgets();
    void init_scene(
        const std::size_t nb_timelines, const std::vector<std::string>& names);
    void organize_widgets();
    void load_pixmaps(const boost::filesystem::path& folder_path,
                      const std::size_t pix_count);

    void show_thumbnail(const std::size_t episode, const std::size_t frame);
    void load_graphs();
    friend class main_window;
};
}
}
