#pragma once

#include <sstream>
#include <string>
#include <algorithm>

#include <boost/filesystem.hpp>

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>
#include <QTextEdit>
#include <QGroupBox>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

#include "crde_credits_view.h"

namespace crde
{
namespace gui
{
class main_window : public QMainWindow
{
    Q_OBJECT

private:
    std::vector<std::string> m_video_names;
    boost::filesystem::path m_exec_path;
    QProcess* m_core_process;
    std::size_t m_video_count = 0;
    int m_biff_count = 0;
    QGridLayout* m_main_window_layout;
    /**< Folder selection **/
    QPushButton* m_folder_selection_button;
    QLineEdit* m_folder_selection_path;
    /**< Credits show **/
    credits_view* m_cred_view;
    /**< Console output **/
    QLineEdit* m_console_outputs;

public:
    main_window(const boost::filesystem::path& exec_path);

    void init_widget();
    void organize_widget();
    void connect_widgets();
    std::string arguments_from_folder(
            const boost::filesystem::path& folder_path);

    QString frame_to_time(const std::size_t frame, const float fps);

public slots:
    void start_detection(const boost::filesystem::path& path);
};
}
}
