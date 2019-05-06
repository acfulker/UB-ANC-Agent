#include "UBAgent.h"
#include "UBSerial.h"
#include "UBConfig.h"
#include <QHostAddress>
#include <QObject>

UBSerial::UBSerial(QSerialPort *parent) : QSerialPort(parent), m_id(0) {
    serial = new QSerialPort(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(dataReadyEvent()));
    openSerialPort();
}
void UBSerial::openSerialPort()
{
    serial->setPortName(SERIAL_PORT_LOPY);
    serial->setBaudRate(BAUD_RATE_LOPY);
    if (serial->open(QIODevice::ReadWrite)) {
        showStatusMessage("Serial Port LoPy Connected");
        qInfo()<<"Serial Port LoPy Device has been connected"<<endl;
    } else {
        showStatusMessage(tr("Open error for LoPy"));
        qInfo()<<"Serial Port LoPy Device cannot be connected, error"<<endl;
    }
}

void UBSerial::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    showStatusMessage(tr("Disconnected"));
    qInfo()<<"Device is disconnected"<<endl;
}

/*void UBSerial::writeData(const QByteArray &data)
{
    serial->write(data);
}*/

/*void UBSerial::readData()
{
   QByteArray data = serial->readAll();
   qDebug() << data;
}*/

void UBSerial::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        closeSerialPort();
    }
}

void UBSerial::showStatusMessage(const QString &message)
{
    qDebug() << message;
}

void UBSerial::sendData(QByteArray data) {
    UBPacket packet;
    packet.setSrcID(m_id);
    packet.setDesID(0); //Extra junk ignore
    packet.setPayload(data);
    serial->write(packet.packetize().append(PACKET_END));
}

void UBSerial::dataReadyEvent() {
    m_data += readAll();

    while (true) {
        int bytes = m_data.indexOf(PACKET_END);
        if (bytes == -1) {
            break;
        }

        UBPacket packet;
        packet.processPacket(m_data.left(bytes));
        //packet.depacketize(m_data.left(bytes));
        m_data.remove(0, bytes + qstrlen(PACKET_END));

        if (packet.getDesID() == m_id) {
            emit dataReadySerial(packet);

            qInfo() << "Packet Received | From " << packet.getSrcID() << " to " << packet.getDesID() << " | Size: " << packet.getPayload().size();
        }
    }
}
