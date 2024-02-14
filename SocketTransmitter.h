#pragma once

#include <QThread>
#include <QTcpSocket>

class SocketTransmitter : public QThread {
	Q_OBJECT
		void run() override {
			while (socket->isOpen()) {
				while (socket->waitForBytesWritten(100))
					emit resultReady(socket->readAll());
			
			}
		}

signals:
	void resultReady(const QByteArray& s);

public:
	QTcpSocket* socket;

private:
	bool readData = false;

};

