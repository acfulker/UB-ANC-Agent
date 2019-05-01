
#ifndef UBSERIAL_H
#define UBSERIAL_H

#include <QSerialPort>
#include "UBPacket.h"

class UBSerial : public QSerialPort
{
    Q_OBJECT
public:
    explicit UBSerial(QSerialPort *parent = 0);

signals:
    void dataReadySerial(UBPacket packet);

public slots:
    void setID(quint8 id) {m_id = id;}
    void sendData(quint8 desID, QByteArray data);

protected slots:
    void dataReadyEvent();

private:
    quint8 m_id;
    QByteArray m_data;
};

#endif // UBSERIAL_H
