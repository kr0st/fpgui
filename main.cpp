#include "mainwindow.h"

#include <QApplication>
#include <QTableWidget>
#include <mongocxx/exception/exception.hpp>

#include <globals.h>
#include <settings.h>
#include <table_view.h>
#include <table_controller.h>
#include <utils.h>
#include <scripting.h>
#include <var_injector.h>
#include <mongo_data_source.h>
#include <fpgui_exceptions.h>


class Closer: public QObject
{
    Q_OBJECT

    public:

        Closer();
        Closer(QApplication* app, fpgui::ui::Table_View* view): view_(view), app_(app) { connect(app, SIGNAL(lastWindowClosed()), this, SLOT(going_down())); }


    public slots:

        void going_down()
        {
            disconnect(app_, SIGNAL(lastWindowClosed()), this, SLOT(going_down()));
            view_->close_view();
        }


    private:

        fpgui::ui::Table_View* view_;
        QApplication* app_;
};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName(fpgui::settings::author);
    QCoreApplication::setOrganizationDomain(fpgui::settings::domain);
    QCoreApplication::setApplicationName(fpgui::settings::application_name);

    fpgui::settings::make_config_path();

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    settings.setFallbacksEnabled(false);

    std::string section(fpgui::settings::db_section_name);
    section += '/';

    if (!settings.contains(QString((section + fpgui::settings::db_host_setting).c_str())))
    {
        settings.clear();
        fpgui::settings::write_default_settigs(settings);
    }

#ifdef _UNIT_TEST
    auto source(std::make_shared<fpgui::data_source::Random_Data_Source<std::queue<std::string>>>());

    source->set_batch_size(app_config.view_batch_size / 4, app_config.view_batch_size);
    source->set_single_string_size(10, 50);
#else
    auto source(std::make_shared<fpgui::data_source::Mongo_Data_Source<>>());

    try
    {
        source->connect(fpgui::settings::read_db_config(settings));
    }
    catch (fpgui::exceptions::Generic_Exception& e)
    {
        generic_utils::ui::message_box(e.what());
        return -1;
    }
    catch (mongocxx::exception& e)
    {
        generic_utils::ui::message_box(e.what());
        return -1;
    }
#endif

    fpgui::settings::create_default_script_file();

    fpgui::lua::load_from_file(fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name);
    fpgui::lua::inject_tab_sorting_config();

    auto app_config(fpgui::settings::read_app_config(settings));

    MainWindow w;
    fpgui::ui::Table_View table(app_config);
    fpgui::ui::Table_Controller table_controller(table);

    QTableWidget* widget(w.centralWidget()->findChild<QTableWidget*>("tableWidget"));

    if (!widget)
    {
        generic_utils::ui::message_box("Fatal error: user interface is broken, exiting..");
        return -1;
    }

    w.show();
    table.setup_view(fpgui::settings::read_tab_config(settings), *widget);
    w.inject_table_view(&table);

    table_controller.set_data_source(source);
    table_controller.start_refreshing_view();

    Closer closer(&a, &table);

    int res = a.exec();
    fpgui::lua::free_resources();

    return res;
}

#include "main.moc"
