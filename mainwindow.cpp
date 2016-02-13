#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QTextStream>

#include <QNetworkInterface>
#include <QDebug>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {
        ui->setupUi(this);

        ui->INTERFACE_NET_mac->setInputMask(QString("HH:HH:HH:HH:HH:HH;0"));
        ui->INTERFACE_mac->setInputMask(QString("HH:HH:HH:HH:HH:HH;0"));

        hotspotconfigini="/etc/ap-hotspot.ini";
        hotspotconfigini="ap-hotspot.ini";
        read();


        logfile="/tmp/hostapd.log";
        pidfile="/tmp/hotspot.pid";
        hotspotconfig="/etc/ap-hotspot.conf";
        hotspotconfigini="/etc/ap-hotspot.ini";
        dnsmasqconfig="/etc/dnsmasq.d/ap-hotspot.rules";
        /*
        $user=$(who | awk '{print $1}' | sed '/^root$/d' | uniq)
        $WMPID=$(ps -u $user | tail -n 1 | awk '{print $1}')
        $DBUS=$(egrep -z 'DBUS_SESSION_BUS_ADDRESS|DISPLAY' /proc/${WMPID}/environ | sed -r -e 's/(.)DBUS_/\1 $
        */
        read();
        check_root();

        start();

        QString name = qgetenv("USER");
        if (name.isEmpty())
            name = qgetenv("USERNAME");
        qDebug() << name;

    }

void MainWindow::info()
    {

    }

MainWindow::~MainWindow()
    {
        delete ui;
    }

void MainWindow::save()
    {

    }

void MainWindow::read()
    {
        QSettings settings(hotspotconfigini,QSettings::NativeFormat);
        //ap-hotspot.conf
        //  WiFi Hotspot
            ui->INTERFACE_mac->setText(settings.value("ap-hotspot/interface_MAC"                , "").toString());
            ui->INTERFACE_mac->setInputMask(QString("HH:HH:HH:HH:HH:HH;0"));
            ui->INTERFACE->setText(settings.value("ap-hotspot/interface"                        , "wlan0").toString());

            ui->INTERFACE_NET_mac->setText(settings.value("ap-hotspot/interface_MAC"            , "").toString());
            ui->INTERFACE_NET_mac->setInputMask(QString("HH:HH:HH:HH:HH:HH;0"));
            ui->INTERFACE_NET->setText(settings.value("hotspot/INTERFACE_NET"                   , "eth0").toString());
            ui->driver->setText(settings.value("ap-hotspot/driver"                              , "nl80211").toString());

        //  Access Point
            ui->ssid->setText(settings.value("ap-hotspot/ssid"                                  , "myhotspot").toString());
            ui->hw_mode->setText(settings.value("ap-hotspot/hw_mode"                            , "g").toString());
        //  WiFi Channel
            ui->channel->setText(settings.value("ap-hotspot/channel"                            , "1").toString());
            ui->macaddr_acl->setText(settings.value("ap-hotspot/macaddr_acl"                    , "0").toString());
            ui->auth_algs->setText(settings.value("ap-hotspot/auth_algs"                        , "1").toString());
            ui->ignore_broadcast_ssid->setText(settings.value("ap-hotspot/ignore_broadcast_ssid", "0").toString());
            ui->wpa->setText(settings.value("ap-hotspot/wpa"                                    , "2").toString());
            ui->wpa_passphrase->setText(settings.value("ap-hotspot/wpa_passphrase"              , "qwerty0987").toString());
            ui->wpa_key_mgmt->setText(settings.value("ap-hotspot/wpa_key_mgmt"                  , "WPA-PSK").toString());
            ui->wpa_pairwise->setText(settings.value("ap-hotspot/wpa_pairwise"                  , "TKIP").toString());
            ui->rsn_pairwise->setText(settings.value("ap-hotspot/rsn_pairwise"                  , "CCMP").toString());
        //  dnsmasq.d
            ui->bind_interfaces->setText(settings.value("hotspot/bind-interfaces"               , "bind-interfaces").toString());
            ui->dhcp_range->setText(settings.value("hotspot/dhcp-range"                         , "192.168.150.2,192.168.150.10,12h").toString());
            ui->INTERFACE_NET->setText(settings.value("hotspot/INTERFACE_NET"                   , "eth0").toString());
    }



void MainWindow::on_INTERFACE_NET_textChanged(const QString &arg1)
    {
        if (ui->INTERFACE_NET_mac->text()==":::::")
            {
                foreach (QNetworkInterface Interfaces, QNetworkInterface().allInterfaces())
                    {
                        if (Interfaces.name()==arg1)
                            {
                                ui->INTERFACE_NET_mac->setText( Interfaces.hardwareAddress().toLatin1() );
                            }
                    }
            } else
            {
                foreach (QNetworkInterface Interfaces, QNetworkInterface().allInterfaces())
                    {
                        if (Interfaces.hardwareAddress()==ui->INTERFACE_NET_mac->text())
                            {
                                ui->INTERFACE_NET->setText( Interfaces.name().toLatin1() );
                            }
                    }
            }

    }

void MainWindow::on_INTERFACE_NET_mac_textChanged(const QString &arg1)
    {
        foreach (QNetworkInterface Interfaces, QNetworkInterface().allInterfaces())
            {
                if (Interfaces.hardwareAddress()==ui->INTERFACE_NET_mac->text())
                    {
                        ui->INTERFACE_NET->setText( Interfaces.name().toLatin1() );
                    } else
                    {
                        ui->INTERFACE_NET->setText("N/A");
                    }
            }
    }

void MainWindow::check_root()
    {
        QString name = qgetenv("USER");
        if (name.isEmpty())
            name = qgetenv("USERNAME");
        qDebug() << name;
        if (name=="root")
            {

            }else
            {
                qApp->exit(1);
            }
    }


void MainWindow::start()
    {
        QProcess *myProcess = new QProcess(this);
        //check_root();
        //check_supported();
        //get_vars();
        //check_network();


        //myProcess->start("service", QStringList()<<"hostapd"<<"stop");
        //myProcess->waitForFinished();
        //qDebug()<<myProcess->readAllStandardOutput();
        //qDebug()<<myProcess->readAllStandardError();
        //myProcess->close();

        /*
        service hostapd stop 2>&1 | show_debug
        service dnsmasq stop 2>&1 | show_debug
        update-rc.d hostapd disable 2>&1 | show_debug
        update-rc.d dnsmasq disable 2>&1 | show_debug
        # Configure IP address for WLAN
        ifconfig "$INTERFACE_WLAN" 192.168.150.1 2>&1 | show_debug
        # Start DHCP/DNS server
        service dnsmasq restart 2>&1 | show_debug
        # Enable routing
        sysctl net.ipv4.ip_forward=1 2>&1 | show_debug
        # Enable NAT
        iptables -t nat -A POSTROUTING -o "$INTERFACE_NET" -j MASQUERADE 2>&1 | show_debug
        # Run access point daemon
        if [[ $(hostapd --help 2>&1 | grep "\-f") ]]; then
                rm -f "$logfile"
                touch "$logfile"
                hostapd -B "$hotspotconfig" -f "$logfile"
                while :
                do
                        [[ $(grep "Using interface" "$logfile") ]] && show_info "Wireless Hotspot active" && break
                        sleep 5
                done
                check_connected 2>&1 &
                disown
        else
                hostapd -B "$hotspotconfig" 2>&1 | show_debug
                show_info "Wireless Hotspot active"
        fi
        */
    }
