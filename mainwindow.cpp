#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QTextStream>

#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDebug>
#include <QProcess>
#include <QFile>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
    {
        ui->setupUi(this);

        /* Global variables */
        user = qgetenv("USER");
        if (user.isEmpty())
            user = qgetenv("USERNAME");

        logfile="/tmp/hostapd.log";
        pidfile="/tmp/hotspot.pid";
        //hotspotconfig="/etc/ap-hotspot.conf";
        /**/hotspotconfig="ap-hotspot.conf";
        hotspotini   ="/ap-hotspot.ini";
        //dnsmasqconfig="/etc/dnsmasq.d/ap-hotspot.rules";
        /**/dnsmasqconfig="ap-hotspot.rules";
        all_interfaces=QNetworkInterface().allInterfaces();

        get_vars();
        qDebug()<<check_supported();
    }
/*
    Global Variablen
    G_USER
    G_LOG_FILE
    G_PID_FILE
    G_HOTSPOTCONF
    G_HOTSPOTINI
    G_DNSMASQCONF
    G_ALL_INTERFACES
    G_INI_AP
    G_INI_INET

    INI Aufbau
    [hotspotini]
    AP  = (wlanX)MAC
    INET= (wlanX/ethX)MAC



    ToDo list:
    GUI und CLI Version

    Interface Erkennung an MAC Adresse (ethX/wlanX)
    Test AP Support wlan0 wlan1 wlanX
    Test AP_Interface on und nicht verbunden
    erst mal Start ini anlegen
    Lese/Speicher ini Datei
    START
    STOP
    RESTART
    Lese LOG
*/

MainWindow::~MainWindow()
    {
        delete ui;
    }

QString MainWindow::mac_to_name(QString mac)
    {
        foreach (QNetworkInterface var, all_interfaces)
            {
                if (var.hardwareAddress()==mac)
                    return var.name();
            }
        return "N/A";
    }


QList<QStringList> MainWindow::check_supported()
    {
        QProcess *myProcess =new QProcess(this);
        myProcess->start("iw", QStringList()<<"dev");
        myProcess->waitForFinished();
        QString T= QString(myProcess->readAll()).replace("\n"," ").replace("\r"," ").replace("\t","").replace("  "," ");
        myProcess->close();
        QRegExp rx("phy#(\\d+) Interface (\\w+)");
        int pos = 0;
        int count = 0;
        QList<QStringList> supported;
        while (pos >= 0) {
            pos = rx.indexIn(T, pos);
            if (pos >= 0) {
                    QStringList tt;
                    tt<<"phy"+rx.cap(1)<<rx.cap(2);

                    myProcess->start("iw", QStringList()<<("phy"+rx.cap(1))<<"info");
                    myProcess->waitForFinished();
                    QString ttt=QString(myProcess->readAll().replace("\t","").replace("\n"," ").replace("  "," "));
                    QRegExp rx1("(Supported interface modes:)(?:.*)( AP )");
                    rx1.indexIn(ttt);
                    if (rx1.capturedTexts().at(2)==" AP ")
                        {
                            tt<<"AP";
                        }
                    supported.append(tt);
                ++pos;
                ++count;
            }
        }
        return supported;
    }

bool MainWindow::check_supported(QString inter)
    {

    }

bool MainWindow::check_network()
    {
        QProcess *myProcess =new QProcess(this);
        myProcess->start("iwconfig", QStringList()<<INTERFACE_WLAN);
        myProcess->waitForFinished();
        QByteArray t=myProcess->readAll();
        myProcess->close();
        if (t.indexOf("Tx-Power=off") != -1)
            {
                qDebug()<<"WiFi is disabled, please enable WiFi before running this script";
                return false;
            } else
            {
                if ( t.indexOf("ESSID:off/any")==-1 && t.indexOf("ESSID:")!=-1 )
                    {
                        qDebug()<<"Please disconnect WiFi before proceeding";
                        return false;
                    }
            }
        return true;
    }

void MainWindow::get_vars()
    {
        QSettings settings("ap-hotspot.ini",QSettings::IniFormat);

        settings.beginGroup("hotspotconfig");
        INTERFACE_WLAN        = settings.value("INTERFACE_WLAN"        ,"wlan0"                            ).toString();
        INTERFACE_WLAN_HEX    = settings.value("INTERFACE_WLAN_HEX"    ,""                                 ).toString();
        INTERFACE_ETH         = settings.value("INTERFACE_ETH"         ,"eth0"                             ).toString();
        INTERFACE_ETH_HEX     = settings.value("INTERFACE_ETH_HEX"     ,""                                 ).toString();
        driver                = settings.value("driver"                ,"nl80211"                          ).toString();
        ssid                  = settings.value("ssid"                  ,"myhotspot"                        ).toString();
        hw_mode               = settings.value("hw_mode"               ,"g"                                ).toString();
        channel               = settings.value("channel"               ,"1"                                ).toString();
        macaddr_acl           = settings.value("macaddr_acl"           ,"0"                                ).toString();
        auth_algs             = settings.value("auth_algs"             ,"1"                                ).toString();
        ignore_broadcast_ssid = settings.value("ignore_broadcast_ssid" ,"0"                                ).toString();
        wpa                   = settings.value("wpa"                   ,"2"                                ).toString();
        wpa_passphrase        = settings.value("wpa_passphrase"        ,"qwerty0987"                       ).toString();
        wpa_key_mgmt          = settings.value("wpa_key_mgmt"          ,"WPA-PSK"                          ).toString();
        wpa_pairwise          = settings.value("wpa_pairwise"          ,"TKIP"                             ).toString();
        rsn_pairwise          = settings.value("rsn_pairwise"          ,"CCMP"                             ).toString();
        settings.endGroup();

        settings.beginGroup("dnsmasqconfig");
        bindinterfaces        = settings.value("bind-interfaces"        ,"bind-interfaces"                 ).toString();
        interface             = settings.value("interface"              ,"wlan0"                           ).toString();
        dhcprange             = settings.value("dhcp-range"             ,"192.168.150.2,192.168.150.10,12h").toString();
        settings.endGroup();

        if (INTERFACE_WLAN_HEX=="")
            {
                foreach (QNetworkInterface var, all_interfaces)
                    {
                        if (var.name()==INTERFACE_WLAN)
                            {
                                settings.beginGroup("hotspotconfig");
                                settings.setValue("INTERFACE_WLAN_HEX",var.hardwareAddress());
                                settings.endGroup();
                                INTERFACE_WLAN_HEX=var.hardwareAddress();
                            }
                    }
            } else
            {
                foreach (QNetworkInterface var, all_interfaces)
                    {
                        if (var.hardwareAddress()==INTERFACE_WLAN_HEX)
                            {
                                settings.beginGroup("hotspotconfig");
                                settings.setValue("INTERFACE_WLAN",var.name());
                                settings.endGroup();
                                INTERFACE_WLAN=var.name();
                            }
                    }
            }

        if (INTERFACE_ETH_HEX=="")
            {
                foreach (QNetworkInterface var, all_interfaces)
                    {
                        if (var.name()==INTERFACE_ETH)
                            {
                                settings.beginGroup("hotspotconfig");
                                settings.setValue("INTERFACE_ETH_HEX",var.hardwareAddress());
                                settings.endGroup();
                                INTERFACE_ETH_HEX=var.hardwareAddress();
                            }
                    }
            } else
            {
                foreach (QNetworkInterface var, all_interfaces)
                    {
                        if (var.hardwareAddress()==INTERFACE_ETH_HEX)
                            {
                                settings.beginGroup("hotspotconfig");
                                settings.setValue("INTERFACE_ETH",var.name());
                                settings.endGroup();
                                INTERFACE_ETH=var.name();
                            }
                    }
            }
        settings.beginGroup("hotspotconfig");
        settings.setValue("INTERFACE_WLAN"        ,INTERFACE_WLAN);
        settings.setValue("INTERFACE_WLAN_HEX"    ,INTERFACE_WLAN_HEX);
        settings.setValue("INTERFACE_ETH"         ,INTERFACE_ETH);
        settings.setValue("INTERFACE_ETH_HEX"     ,INTERFACE_ETH_HEX);
        settings.setValue("driver"                ,driver);
        settings.setValue("ssid"                  ,ssid);
        settings.setValue("hw_mode"               ,hw_mode);
        settings.setValue("channel"               ,channel);
        settings.setValue("macaddr_acl"           ,macaddr_acl);
        settings.setValue("auth_algs"             ,auth_algs);
        settings.setValue("ignore_broadcast_ssid" ,ignore_broadcast_ssid);
        settings.setValue("wpa"                   ,wpa);
        settings.setValue("wpa_passphrase"        ,wpa_passphrase);
        settings.setValue("wpa_key_mgmt"          ,wpa_key_mgmt);
        settings.setValue("wpa_pairwise"          ,wpa_pairwise);
        settings.setValue("rsn_pairwise"          ,rsn_pairwise);
        settings.endGroup();

        settings.beginGroup("dnsmasqconfig");
        settings.setValue("bind-interfaces"        ,bindinterfaces);
        settings.setValue("interface"              ,interface);
        settings.setValue("dhcp-range"             ,dhcprange);
        settings.endGroup();
    }

void MainWindow::start()
    {
        QProcess *myProcess =new QProcess(this);

        myProcess->start("service", QStringList()<<"hostapd"<<"stop");
        myProcess->waitForFinished();

        myProcess->start("service", QStringList()<<"dnsmasq"<<"stop");
        myProcess->waitForFinished();

        myProcess->start("update-rc.d", QStringList()<<"hostapd"<<"disable");
        myProcess->waitForFinished();

        myProcess->start("update-rc.d", QStringList()<<"dnsmasq"<<"disable");
        myProcess->waitForFinished();

        myProcess->start("ifconfig", QStringList()<<INTERFACE_WLAN<<"192.168.150.1");
        myProcess->waitForFinished();

        myProcess->start("service", QStringList()<<"dnsmasq"<<"restart");
        myProcess->waitForFinished();

        myProcess->start("sysctl", QStringList()<<"net.ipv4.ip_forward=1");
        myProcess->waitForFinished();

        myProcess->start("iptables", QStringList()<<"-t"<<"nat"<<"-A"<<"POSTROUTING"<<"-o"<<INTERFACE_ETH<<"-j"<<"MASQUERADE");
        myProcess->waitForFinished();

        Hotspotconfig();
        Dnsmasqconfig();

        myProcess->start("rm", QStringList()<<"-f"<<logfile);
        myProcess->waitForFinished();
        myProcess->start("rm", QStringList()<<"-f"<<pidfile);
        myProcess->waitForFinished();

        myProcess->start("hostapd",  QStringList()<<"-B"<<hotspotconfig<<"-f"<<logfile);
        QFile file(pidfile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            return;
        QTextStream out(&file);
        out<<myProcess->pid();
        myProcess->waitForFinished();
    }

void MainWindow::hostapd_status()
    {

    }

void MainWindow::hostapd_stop()
    {

    }

void MainWindow::hostapd_start()
    {

    }

void MainWindow::hostapd_restart()
    {

    }

void MainWindow::Hotspotconfig()
    {
        QFile file(hotspotconfig);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            return;
        QTextStream out(&file);
        out << "# WiFi Hotspot"<<"\n";
        out << "interface="+INTERFACE_WLAN<<"\n";
        out << "driver="+driver<<"\n";
        out << "#Access Point"<<"\n";
        out << "ssid="+ssid<<"\n";
        out << "hw_mode="+hw_mode<<"\n";
        out << "# WiFi Channel:"<<"\n";
        out << "channel="+channel<<"\n";
        out << "macaddr_acl="+macaddr_acl<<"\n";
        out << "auth_algs="+auth_algs<<"\n";
        out << "ignore_broadcast_ssid="+ignore_broadcast_ssid<<"\n";
        out << "wpa="+wpa<<"\n";
        out << "wpa_passphrase="+wpa_passphrase<<"\n";
        out << "wpa_key_mgmt="+wpa_key_mgmt<<"\n";
        out << "wpa_pairwise="+wpa_pairwise<<"\n";
        out << "rsn_pairwise="+rsn_pairwise<<"\n";
        file.close();
    }

void MainWindow::Dnsmasqconfig()
    {
        QFile file(dnsmasqconfig);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            return;
        QTextStream out(&file);
        out << "# WiFi Hotspot"<<"\n";
        out << "# Bind to only one interface\n";
        out << bindinterfaces<<"\n";
        out << "# Choose interface for binding\n";
        out << "interface="+INTERFACE_WLAN<<"\n";
        out << "# Specify range of IP addresses for DHCP leasses"<<"\n";
        out << "dhcp-range="+dhcprange<<"\n";
        out << "#INTERFACE_NET="+INTERFACE_ETH<<"\n";

        file.close();
    }
