#include "crde_main_window.h"

namespace crde
{
namespace gui
{
main_window::main_window() : QMainWindow(nullptr)
{
    init_widget();
    organize_widget();
    connect_widgets();
}

void main_window::init_widget()
{
    m_main_window_layout = new QGridLayout(nullptr);
    m_folder_selection_button = new QPushButton("select folder",this);
    m_folder_selection_path = new QLineEdit("",this);
        m_folder_selection_path->setDisabled(true);
        m_folder_selection_path->setReadOnly(true);
    m_cred_view = new credits_view(this);
        m_cred_view->setDisabled(true);
    m_console_outputs = new QTextEdit(this);
        m_console_outputs->setReadOnly(true);
}

void main_window::organize_widget()
{
    const int full_width = 10;

    // Wrapping in groupbox
    QGroupBox* consol_out_gb = new QGroupBox(nullptr);
    consol_out_gb->setTitle("Console outputs");
    consol_out_gb->setLayout(m_console_outputs->layout());

    //Row 0
    m_main_window_layout->addWidget(m_folder_selection_button,0,0);
    m_main_window_layout->addWidget(
                m_folder_selection_path,0,1,1,full_width-1);

    //Row 1
    m_main_window_layout->addWidget(
                m_cred_view,1,0,1,full_width);

    //Row 2
    m_main_window_layout->addWidget(m_console_outputs,2,0,1,full_width);

    QWidget* proxy_widget = new QWidget();
    proxy_widget->setLayout(m_main_window_layout);
    setCentralWidget(proxy_widget);
}

void main_window::connect_widgets()
{
    connect(m_folder_selection_button, &QPushButton::clicked, [this]()
    {
        QFileDialog* file_dialog = new QFileDialog(this);
        file_dialog->setFileMode(QFileDialog::DirectoryOnly);

        connect(file_dialog, &QFileDialog::fileSelected, [this](QString path)
        {
            m_folder_selection_path->setText(path);
        });

        m_folder_selection_path->setEnabled(true);
        m_cred_view->setEnabled(true);

        file_dialog->exec();
    });
}
}
}
