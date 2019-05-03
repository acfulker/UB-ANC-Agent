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
    void sendData(QByteArray data);
    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void showStatusMessage(const QString &message);

protected slots:
    void dataReadyEvent();

private:
    quint8 m_id;
    QByteArray m_data;
    QByteArray data;
    QSerialPort *serial;

};

#endif // UBSERIAL_H
