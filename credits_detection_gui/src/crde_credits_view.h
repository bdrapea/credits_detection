#pragma once

#include <vector>
#include <iostream>

#include <QApplication>
#include <QGraphicsView>
#include <QDesktopWidget>
#include <QResizeEvent>

#include "crde_credits_timeline.h"

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
    std::vector< QGraphicsPixmapItem* > m_thumbnails;
    QVBoxLayout* m_timelines_layout;

public:
    credits_view(QWidget* parent = nullptr);

    void init_widgets();
    void init_scene(
        const std::size_t nb_timelines, const std::vector<std::string>& names);
    void organize_widgets();

    friend class main_window;
};
}
}
