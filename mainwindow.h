#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkInterface>

namespace Ui {
        class MainWindow;
    }

class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();
        QList<QStringList> check_supported();
        bool check_supported(QString inter);
        bool check_network();
        void get_vars();
        void start();
        void Hotspotconfig();
        void Dnsmasqconfig();

        void hostapd_status();
        void hostapd_stop();
        void hostapd_start();
        void hostapd_restart();

        QString mac_to_name(QString mac);


    private:
        Ui::MainWindow *ui;
        QString logfile;
        QString pidfile;
        QString hotspotconfig;
        QString hotspotini;
        QString dnsmasqconfig;
        QString user;


        QString GUI_interface;

        QStringList ini_key;
        QStringList ini_value;
        QList<QLine> UIOption;
        QList<QNetworkInterface> all_interfaces;

        QString INTERFACE_WLAN;
        QString INTERFACE_WLAN_HEX;
        QString INTERFACE_ETH;
        QString INTERFACE_ETH_HEX;
        QString driver;
        QString ssid;
        QString hw_mode;
        QString channel;
        QString macaddr_acl;
        QString auth_algs;
        QString ignore_broadcast_ssid;
        QString wpa;
        QString wpa_passphrase;
        QString wpa_key_mgmt;
        QString wpa_pairwise;
        QString rsn_pairwise;
        QString bindinterfaces;
        QString interface;
        QString dhcprange;
    };

#endif // MAINWINDOW_H
