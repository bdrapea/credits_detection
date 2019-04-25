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

            //cursor
            for(std::size_t i=0; i<m_video_count; i++)
            {
                connect(m_cred_view->m_credits_timelines[i]->m_timeline,
                        &QSlider::valueChanged,[this,i](const int v)
                {
                    m_cred_view->m_credits_timelines[i]->m_cursor->clear();
                    int pos = (m_biff_count/ m_cred_view->m_credits_timelines[0]->
                            m_timeline->maximum())*v;

                    m_cred_view->m_credits_timelines[i]->m_cursor->
                            append(pos,0);
                    m_cred_view->m_credits_timelines[i]->m_cursor->
                            append(pos,256);
                });
            }
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
        QString str(m_core_process->readAll());
        m_console_outputs->setText(str);

        std::string std_str = str.toStdString();

        static std::size_t episod_ind = 0;
        const float scale = 101.0f / static_cast<float>(m_biff_count);
        const char* exten_name = ".jpg";
        const std::size_t zero_count = 8;

        if(std_str.find("LOADING") != std::string::npos)
        {
            std::size_t exten_pos = std_str.find(exten_name);
            const int progress_value =
                    static_cast<int>(
                    scale * std::stof(std_str.substr(exten_pos-zero_count)));

            if(progress_value < m_cred_view->m_credits_timelines[episod_ind]->
                m_progress->value())
            {
                m_cred_view->m_credits_timelines[episod_ind]->
                                m_progress->setValue(100);
                episod_ind++;
            }

            m_cred_view->m_credits_timelines[episod_ind]->m_progress->
                    setValue(progress_value);
        }
        if(std_str.find("GRAPH") != std::string::npos)
        {

            boost::filesystem::path graph_folder =
                    boost::filesystem::path(__FILE__)
                    .parent_path().parent_path().parent_path();
            graph_folder /= "graphs";

            std::vector< boost::filesystem::path > graph_paths;
            std::copy(boost::filesystem::directory_iterator(graph_folder),
                      boost::filesystem::directory_iterator(),
                      std::back_inserter(graph_paths));
            std::sort(graph_paths.begin(), graph_paths.end());

            std::size_t graph_path_count = graph_paths.size();
            for(std::size_t i=0; i<graph_path_count; i++)
            {
                std::ifstream ifile(
                            graph_paths[i].c_str(),
                            std::ios::in);

                QList<QPointF> points;
                const int pas = 10;
                points.reserve(m_biff_count/pas);
                int tmp = 0;
                int off = 0;
                int max_y = 0;

                while(off < m_biff_count)
                {
                    if(off%pas==0)
                    {
                        ifile >> tmp;
                        points.push_back(QPointF(off,tmp));

                    if(tmp > max_y)
                        max_y =tmp;
                    }
                    else
                    {
                        ifile >> tmp;
                    }

                    off++;
                }

                points.pop_back();

                if(m_cred_view->m_credits_timelines[i]->m_series->count() > 0)
                    m_cred_view->m_credits_timelines[i]->m_series->clear();

                m_cred_view->m_credits_timelines[i]->m_series->append(points);
                m_cred_view->m_credits_timelines[i]->m_chart->axes()[1]->
                        setMax(max_y);

                ifile.close();
            }
        }
        if(std::size_t ind = std_str.find("RESSEMBLANCE:") != std::string::npos)
        {
            while(std_str[ind] != ':')ind++;
            std::string num;
            while(std_str[ind] != ' ')
            {
                num.push_back(std_str[ind]);
            }

            std::cout << "num="<< num << std::endl;
        }
        if(std_str.find("TIMECODES") != std::string::npos)
        {
            const char* keyword = "Frames:";
            const std::size_t key_size = std::strlen(keyword);
            std::size_t ind = 0;
            std::size_t episode = 0;

            //Extract frame
            while((ind = std_str.find(keyword,ind+1)) !=  std::string::npos)
            {
                ind += key_size;
                while(std_str[ind] == ' ')
                {
                    ind++;
                }
                std::string start,end;
                while(std_str[ind] != '-')
                {
                    start.push_back(std_str[ind]);
                    ind++;
                }
                while(std_str[ind] < '0' || std_str[ind] > '9')ind++;
                while(std_str[ind] != '\n')
                {
                    end.push_back(std_str[ind]);
                    ind++;
                }

                qreal start_ind = std::stoi(start);
                qreal end_ind = std::stoi(end);

                QList<QPointF> credits_starts =
                {
                    {start_ind,0},{start_ind,256}
                };
                QList<QPointF> credits_ends =
                {
                    {end_ind,0},{end_ind,256}
                };

                m_cred_view->m_credits_timelines[episode]->
                        m_chart->legend()->markers()[2]->setVisible(true);
                m_cred_view->m_credits_timelines[episode]->
                        m_chart->legend()->markers()[3]->setVisible(true);

                m_cred_view->m_credits_timelines[episode]->
                        m_credits_start->append(credits_starts);
                m_cred_view->m_credits_timelines[episode]->
                        m_credits_end->append(credits_ends);

                m_cred_view->m_credits_timelines[episode]->
                        m_credits_start->setName(
                            frame_to_time(
                            static_cast<std::size_t>(start_ind),25.0)
                            + " = " + QString::number(start_ind));
                m_cred_view->m_credits_timelines[episode]->
                        m_credits_end->setName(
                            frame_to_time(static_cast<std::size_t>(end_ind),
                                          25.0)
                            + " = " + QString::number(end_ind));

                m_cred_view->m_credits_timelines[episode]->
                        m_chart->setTitle("Ressemblance = ");
                episode ++;
            }
        }
    });
}

QString main_window::frame_to_time(std::size_t frames, const float fps)
{
    std::size_t nb_frames = frames % static_cast<std::size_t>(fps);
    frames /= static_cast<std::size_t>(fps);
    std::size_t nb_seconds = frames % 60;
    frames /= 60;
    std::size_t nb_minutes = frames % 60;
    frames /= 60;
    std::size_t nb_hours = frames % 60;
    std::stringstream times;

    auto two_number_display = [](const std::size_t num)->std::string
    {
        std::string num_str;
        if(num < 10)
            num_str = std::string("0") + std::to_string(num);
        else
            num_str = std::to_string(num);

        return num_str;
    };

    times << two_number_display(nb_hours);
    times << ':';
    times << two_number_display(nb_minutes);
    times << ':';
    times << two_number_display(nb_seconds);
    times << '.';
    times << two_number_display(nb_frames);

    return times.str().c_str();
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

    m_cred_view->m_thumbnail_pixmaps.reserve(names.size());
    for(const boost::filesystem::path& name : names)
    {
        m_cred_view->load_pixmaps(name,static_cast<std::size_t>(m_biff_count));
        arguments += name.string();
        arguments += " ";
        m_video_count++;
        m_video_names.push_back(name.filename().string());
    }

    return arguments;
}
}
}
