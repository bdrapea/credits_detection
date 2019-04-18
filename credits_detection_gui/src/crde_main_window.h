#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>
#include <QTextEdit>
#include <QGroupBox>
#include <QFileDialog>

#include "crde_credits_view.h"

namespace crde
{
namespace gui
{
class main_window : public QMainWindow
{
    Q_OBJECT

private:
    QGridLayout* m_main_window_layout;
    /**< Folder selection **/
    QPushButton* m_folder_selection_button;
    QLineEdit* m_folder_selection_path;
    /**< Credits show **/
    credits_view* m_cred_view;
    /**< Console output **/
    QTextEdit* m_console_outputs;
public:
    main_window();

    void init_widget();
    void organize_widget();
    void connect_widgets();
};
}
}
