#include "UBAgent.h"
#include "UBSerial.h"
#include "UBConfig.h"
#include <QHostAddress>

UBSerial::UBSerial(QSerialPort *parent) : QSerialPort(parent), m_id(0) {
    connect(this, SIGNAL(readyRead()), this, SLOT(dataReadyEvent()));
}

void UBSerial::sendData(quint8 desID, QByteArray data) {
    UBPacket packet;
    packet.setSrcID(m_id);
    packet.setDesID(desID);
    packet.setPayload(data);

    write(packet.packetize().append(PACKET_END));
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

        if (packet.getDesID() == m_id || packet.getDesID() == BROADCAST_ID) {
            emit dataReadySerial(packet);

            qInfo() << "Packet Received | From " << packet.getSrcID() << " to " << packet.getDesID() << " | Size: " << packet.getPayload().size();
        }
    }
}
