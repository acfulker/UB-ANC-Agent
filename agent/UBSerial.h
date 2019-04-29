
#ifndef UBSERIAL_H
#define UBSERIAL_H

#include <QSerialPort>

class UBSerial : public QSerialPort
{
    Q_OBJECT
public:
    explicit UBSerial(QSerialPort *parent = 0);

    signals:
            void dataReady(UBPacket data);

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