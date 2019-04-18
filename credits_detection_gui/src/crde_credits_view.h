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
    QVBoxLayout* m_timelines_layout;

public:
    credits_view(QWidget* parent = nullptr);

    void init_widgets();
    void organize_widgets();
};
}
}
