#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ClientServer.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

class ClientServer : public QMainWindow
{
    Q_OBJECT

public:
    ClientServer(QWidget* parent = nullptr);
    ~ClientServer();

    int sizeMainWidget[2];
    int sizeConfigurationServer[2];

signals:
    void operate(const QString&);

public slots:

    /**********************************************/
    /*                   THREAD                   */
    /**********************************************/

    void dataAvailable(const QByteArray &s) { ui.textBrowserClient->append("Client: " + QString::fromStdString(s.toStdString())); }
    

    /**********************************************/
    /**********************************************/
    /**********************************************/


    /**********************************************/
    /*                CONFIGURATION               */
    /**********************************************/

    void on_pushButtonConfigurationServerClient_clicked();
    void on_pushButtonValideStatusClient_clicked();
    void on_pushButtonValideStatusServer_clicked();

    /**********************************************/
    /**********************************************/
    /**********************************************/


    /**********************************************/
    /*                   SERVER                   */
    /**********************************************/

    void on_pushButtonStartServer_clicked();
    void on_pushButtonSendToClient_clicked();
    void on_pushButtonClearLogServer_clicked();

    /**********************************************/
    /**********************************************/
    /**********************************************/


    /**********************************************/
    /*                   CLIENT                   */
    /**********************************************/

    void on_pushButtonConnectClient_clicked();
    void on_pushButtonSendToServer_clicked();


    /**********************************************/
    /**********************************************/
    /**********************************************/


    /**********************************************/
    /*               LIST CLIENTS                 */
    /**********************************************/

    void on_comboBoxListClients_currentIndexChanged(int index);


    /**********************************************/
    /**********************************************/
    /**********************************************/


    void on_pushButtonReturnMainWidget_clicked();


private:
    Ui::ClientServerClass ui;

    /**********************************************/
    /*                CONFIGURATION               */
    /**********************************************/

    QHostAddress ipClient;
    QHostAddress ipServer;

    quint16 portClient;
    quint16 portServer;

    bool clientConfigure;
    bool serverConfigure;

    /**********************************************/
    /**********************************************/
    /**********************************************/


    /**********************************************/
    /*                   SERVER                   */
    /**********************************************/


    void newConnectionClient();
    //void dataAvailable();
    void disconnectedClient();

    int searchClient(QHostAddress ip, quint16 port);


    bool oneClientConnected;

    QTcpServer* server;
    QTcpSocket* socketServer;

    //int countClient;
    //QVector<QVector<QString>> listIPClient;
    //QVector<QVector<int>> listPortClient;
    QVector<QTcpSocket*> listSocketClients;


    /**********************************************/
    /**********************************************/
    /**********************************************/


    /**********************************************/
    /*                   CLIENT                   */
    /**********************************************/

    QTcpSocket* socketClient;

    void connectToServer();
    void disconnectToServer();
    void readyToReadServer();




    /**********************************************/
    /**********************************************/
    /**********************************************/

};
