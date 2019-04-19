#include "crde_main_window.h"

namespace crde
{
namespace gui
{
main_window::main_window(const boost::filesystem::path& exec_path)
    : QMainWindow(nullptr),
      m_exec_path(exec_path)
{
    init_widget();
    organize_widget();
    connect_widgets();
}

void main_window::init_widget()
{
    m_core_process = new QProcess(this);
    m_main_window_layout = new QGridLayout(nullptr);
    m_folder_selection_button = new QPushButton("select folder",this);
    m_folder_selection_path = new QLineEdit("",this);
        m_folder_selection_path->setReadOnly(true);
    m_cred_view = new credits_view(this);
    m_console_outputs = new QLineEdit(this);
        m_console_outputs->setReadOnly(true);
}

void main_window::organize_widget()
{
    const int full_width = 10;

    //Row 0
    m_main_window_layout->addWidget(m_folder_selection_button,0,0);
    m_main_window_layout->addWidget(
                m_folder_selection_path,0,1,1,full_width-1);

    //Row 1
    m_main_window_layout->addWidget(
                m_cred_view,1,0,1,full_width);

    //Row 2
    m_main_window_layout->addWidget(m_console_outputs,2,0,1,full_width);

    QWidget* proxy_widget = new QWidget;
    proxy_widget->setLayout(m_main_window_layout);
    setCentralWidget(proxy_widget);
}

void main_window::connect_widgets()
{
    //Folder select
    connect(m_folder_selection_button, &QPushButton::clicked, [this]()
    {
        QFileDialog* file_dialog = new QFileDialog(this);
        file_dialog->setFileMode(QFileDialog::DirectoryOnly);

        connect(file_dialog, &QFileDialog::fileSelected,
                m_folder_selection_path, &QLineEdit::setText);
        connect(file_dialog, &QFileDialog::fileSelected,[this]()
        {
            start_detection(m_folder_selection_path->text().toStdString());
            m_cred_view->init_scene(m_video_count,m_video_names);
        });

        file_dialog->exec();
    });
}

void main_window::start_detection(const boost::filesystem::path &path)
{
    QString command = m_exec_path.c_str();
    command += " ";
    command += arguments_from_folder(path).c_str();
    m_core_process->start(command);

    connect(m_core_process,&QProcess::readyReadStandardOutput,
            [this]()
    {
        QString str(m_core_process->readAllStandardOutput());
        m_console_outputs->setText(str);

        std::string std_str = str.toStdString();

        if(std_str.find("LOADING") != std::string::npos)
        {
            std::size_t ind_num = static_cast<std::size_t>(
                            std::stoi(std_str.substr(35,2)));

            double scale = 101.0/static_cast<double>(m_biff_count);

            std::size_t ind = std_str.find(".jpg");
            double value = std::stod(std_str.substr(ind-8,8));

            m_cred_view->m_credits_timelines[ind_num-1]
                    ->m_progress->setValue(static_cast<int>(value*scale));
        }
    });
}


std::string main_window::arguments_from_folder(
        const boost::filesystem::path& folder_path)
{
    std::string arguments = "";
    boost::filesystem::directory_iterator dir_it(folder_path);
    boost::filesystem::directory_iterator end_it;
    boost::system::error_code err_code;

    std::vector<boost::filesystem::path> names;
    std::copy(boost::filesystem::directory_iterator(folder_path),
                            boost::filesystem::directory_iterator(),
                            std::back_inserter(names));
    std::sort(names.begin(),names.end());

    m_biff_count = static_cast<int>(std::count_if(
                boost::filesystem::directory_iterator(names[0]),
                boost::filesystem::directory_iterator(),
                static_cast< bool(*)(const boost::filesystem::path&) >(
                                         boost::filesystem::is_regular_file)));

    for(const boost::filesystem::path& name : names)
    {
        arguments += name.string();
        arguments += " ";
        m_video_count++;
        m_video_names.push_back(name.filename().string());
    }

    return arguments;
}
}
}
