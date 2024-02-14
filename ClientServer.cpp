#include "ClientServer.h"
#include <QMessageBox>
#include "SocketTransmitter.h"

ClientServer::ClientServer(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	sizeMainWidget[0] = 810;
	sizeMainWidget[1] = 580;
	sizeConfigurationServer[0] = 370;
	sizeConfigurationServer[1] = 200;

	ui.stackedWidget->setCurrentIndex(0);
	ui.pushButtonStartServer->setText("Start Server");
	ui.labelStatusIPPortServer->setText("[IP]:[PORT]");
	ui.labelStatusServer->setText("Server status: STOP");

	oneClientConnected = false;
	clientConfigure = false;
	serverConfigure = false;

}

ClientServer::~ClientServer() {}

/**********************************************/
/*                CONFIGURATION               */
/**********************************************/
void ClientServer::on_pushButtonConfigurationServerClient_clicked() {
	ui.stackedWidget->setCurrentIndex(1);
	this->setMinimumSize(sizeConfigurationServer[0], sizeConfigurationServer[1]);
	this->setMaximumSize(sizeConfigurationServer[0], sizeConfigurationServer[1]);
}

void ClientServer::on_pushButtonValideStatusClient_clicked() {
	ipClient = ui.lineEditIPClient->displayText();
	portClient = ui.lineEditPortClient->displayText().toInt();
	if (!clientConfigure) clientConfigure = true;
	QMessageBox::information(this, "Configuration", "Client IP/Port Edited : " + ipClient.toString() + ':' + QString::number(portClient));
}

void ClientServer::on_pushButtonValideStatusServer_clicked() {
	ipServer = ui.lineEditIPServer->displayText();
	portServer = ui.lineEditPortServer->displayText().toInt();
	if (!serverConfigure) serverConfigure = true;
	QMessageBox::information(this, "Configuration", "Server IP/Port Edited : " + ipServer.toString() + ':' + QString::number(portServer));
}
/**********************************************/
/**********************************************/
/**********************************************/




/**********************************************/
/*                   SERVER                   */
/**********************************************/

void ClientServer::on_pushButtonStartServer_clicked() {
	if (serverConfigure) {
		if (ui.pushButtonStartServer->text() == "Start Server") {
			//if (ipServer != NULL)
			server = new QTcpServer(this);

			connect(server, &QTcpServer::newConnection, this, &ClientServer::newConnectionClient);

			

			ui.textBrowserServer->append("*** Ready for a new Connection ...");

			if (!server->listen(ipServer, portServer)) {
				QMessageBox::warning(this, "Start Server", "The server did not Start");
			}
			else {
				ui.labelStatusServer->setText("Server status: START");
				ui.pushButtonStartServer->setText("Stop Server");
				ui.labelStatusIPPortServer->setText(ipServer.toString() + ":" + QString::number(portServer));
				ui.pushButtonSendToClient->setEnabled(true);
			}
		}
		else {
			delete server;
			ui.textBrowserServer->append("*** Shutting down the server");
			ui.labelStatusServer->setText("Server status: STOP");
			ui.pushButtonStartServer->setText("Start Server");
			ui.labelStatusIPPortServer->setText("[IP]:[PORT]");
			ui.pushButtonSendToClient->setEnabled(false);
		}
	}
	else {
		QMessageBox::warning(this, "Configuration", "Server IP/Port no configurated");

	}
}


int ClientServer::searchClient(QHostAddress ip, quint16 port) {
	for (int i = 0; i < listSocketClients.size(); i++) {
		if (listSocketClients.at(i)->peerAddress() == ip && listSocketClients.at(i)->peerPort() == port)
			return i;
	}
	return -1;
}

void ClientServer::newConnectionClient() {
	socketServer = server->nextPendingConnection();


	if (searchClient(socketClient->peerAddress(), socketClient->peerPort()) == -1) {
		listSocketClients.push_back(socketClient);
		
		ui.textBrowserClient->append(QString::number(socketServer->peerAddress().toString().toInt()));
	}

	ui.comboBoxListClients->addItem("Client " + QString::number(listSocketClients.size()));


	

	if (!oneClientConnected) oneClientConnected = true;

	//connect(socketServer, &QTcpSocket::readyRead, this, &ClientServer::dataAvailable);
	connect(socketServer, &QTcpSocket::disconnected, this, &ClientServer::disconnectedClient);


		/**********************************************/
		/*                   THREAD                   */
		/**********************************************/

	SocketTransmitter* socketTransmitter = new SocketTransmitter();
	socketTransmitter->socket = socketServer;
	socketTransmitter->moveToThread(this->thread());
	connect(socketTransmitter, &SocketTransmitter::resultReady, this, &ClientServer::dataAvailable);
	connect(socketTransmitter, &SocketTransmitter::finished, socketTransmitter, &QObject::deleteLater);
	socketTransmitter->start();

		/**********************************************/
		/**********************************************/
		/**********************************************/





	ui.textBrowserServer->append("Client \"" + socketServer->peerAddress().toString() + ':' + QString::number(socketServer->peerPort()) + "\" connected.");
	socketServer->write("*** Connection accepted\r\n\r\n");
	socketServer->flush();
	socketServer->waitForBytesWritten(3000);
}
/*
void ClientServer::dataAvailable() {
	ui.textBrowserServer->append(socketServer->peerAddress().toString() + '/' + QString::number(socketServer->peerPort()) + ": " + socketServer->readAll());

}
*/
void ClientServer::disconnectedClient() {
	if (socketServer->isOpen()) {
		ui.textBrowserServer->append("Client \"" + socketServer->peerAddress().toString() + ':' + QString::number(socketServer->peerPort()) + "\" disconnected.");

	}
}


void ClientServer::on_pushButtonSendToClient_clicked() {
	if (oneClientConnected) {


		socketServer->write("Server: ");
		socketServer->write(ui.plainTextEditSendServer->toPlainText().toStdString().c_str()); // Mettre le curseur à 0
		socketServer->write("\r\n");
		QString tmp = ui.plainTextEditSendServer->toPlainText().toStdString().c_str();
		ui.textBrowserServer->append("Server: " + tmp);

	}
}


void ClientServer::on_pushButtonClearLogServer_clicked() {
	ui.textBrowserServer->clear();
}

/**********************************************/
/**********************************************/
/**********************************************/








/**********************************************/
/*                   CLIENT                   */
/**********************************************/

void ClientServer::on_pushButtonConnectClient_clicked() {
	if (clientConfigure) {
		if (ui.pushButtonConnectClient->text() == "Connect Client") {
			socketClient = new QTcpSocket(this);

			connect(socketClient, &QTcpSocket::connected, this, &ClientServer::connectToServer);
			connect(socketClient, &QTcpSocket::disconnected, this, &ClientServer::disconnectToServer);
			connect(socketClient, &QTcpSocket::readyRead, this, &ClientServer::readyToReadServer);

			socketClient->connectToHost(ipClient, portClient);
			ui.textBrowserClient->append("*** Current connection ...");
			ui.pushButtonConnectClient->setText("Disconnecte Client");
			ui.pushButtonSendToServer->setEnabled(true);
		}
		else {
			disconnectToServer();
		}
	}
	else {
		QMessageBox::warning(this, "Configuration", "Client IP/Port no configurated");
	}
}

void ClientServer::connectToServer() {
	ui.textBrowserClient->append("*** Connected " + ipClient.toString() + ':' + QString::number(portClient) + " ...");
	ui.labelStatusIPClient->setText("IP : " + socketClient->localAddress().toString());
	ui.labelStatusPortClient->setText("Port : " + QString::number(socketClient->localPort()));
}

void ClientServer::disconnectToServer() {
	if (socketClient->isOpen()) {
		socketClient->close();
		ui.textBrowserClient->append("*** Deconnected");
		ui.pushButtonConnectClient->setText("Connect Client");
		ui.pushButtonSendToServer->setEnabled(false);
		ui.labelStatusIPClient->setText("IP : [IP]");
		ui.labelStatusPortClient->setText("Port : [PORT]");
	}
}

void ClientServer::readyToReadServer() {
	ui.textBrowserClient->append(socketClient->readAll());
}


void ClientServer::on_pushButtonSendToServer_clicked() {
	if (socketClient->isOpen()) {
		//socketClient->write("IP/Port[SOON] : ");
		socketClient->write(ui.plainTextEditSendClient->toPlainText().toStdString().c_str()); // Mettre le curseur à 0
		socketClient->write("\r\n");
		QString tmp = ui.plainTextEditSendClient->toPlainText().toStdString().c_str();
		ui.textBrowserClient->append("Client: " + tmp);
	}
}

/**********************************************/
/**********************************************/
/**********************************************/

void ClientServer::on_pushButtonReturnMainWidget_clicked() {
	ui.stackedWidget->setCurrentIndex(0);
	this->setMinimumSize(sizeMainWidget[0], sizeMainWidget[1]);
	this->setMaximumSize(sizeMainWidget[0], sizeMainWidget[1]);
}








/**********************************************/
/*               LIST CLIENTS                 */
/**********************************************/

void ClientServer::on_comboBoxListClients_currentIndexChanged(int index) {
	if (listSocketClients.size() != 0) {
		if (listSocketClients.at(index)->isOpen())
			ui.labelIsConnected->setText("Status : CONNECTED");
		else
			ui.labelIsConnected->setText("Status : DISCONNECTED");

		ui.labelListIP->setText("IP:" + listSocketClients.at(index)->peerAddress().toString());
		ui.labelListPort->setText("Port:" + QString::number(listSocketClients.at(index)->peerPort()));
	}
	
}


/**********************************************/
/**********************************************/
/**********************************************/