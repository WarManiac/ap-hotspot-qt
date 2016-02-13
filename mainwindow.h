#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
        class MainWindow;
    }

class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();
        void save();
        void read();
        void start();
        void check_root();
        void info();

    private slots:
        void on_INTERFACE_NET_textChanged(const QString &arg1);
        void on_INTERFACE_NET_mac_textChanged(const QString &arg1);

    private:
        Ui::MainWindow *ui;
        QString logfile;
        QString pidfile;
        QString hotspotconfig;
        QString hotspotconfigini;
        QString dnsmasqconfig;
        QString user;
        QString WMPID;
        QString DBUS;

        QString GUI_interface;

        QStringList ini_key;
        QStringList ini_value;
        QList<QLine> UIOption;
    };

#endif // MAINWINDOW_H
