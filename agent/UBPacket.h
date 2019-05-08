#ifndef UBPACKET_H
#define UBPACKET_H

#include <QObject>
#include <QGeoCoordinate>

class UBPacket
{
public:
    explicit UBPacket();
    enum typeEnum{
      NOFLYZONE = 0, POS = 1};

public slots:
    void setSrcID(quint8 srcID) {m_srcID = srcID;}
    void setDesID(quint8 desID) {m_desID = desID;}
    void setPayload(const QByteArray& payload) {m_payload = payload;}

    quint8 getSrcID(void) {return m_srcID;}
    quint8 getDesID(void) {return m_desID;}
    double getLat(void)   {return m_lat;}
    double getLon(void)   {return m_lon;}
    quint8 getNoFly(void) {return m_NoFlyZone;}
    quint8 getType(void) {return m_type;}
    QByteArray getPayload(void) {return m_payload;}

    QByteArray packetize(void);
    void depacketize(const QByteArray &packet);
    void depacketizeNoFly();
    QByteArray packetizePos(QGeoCoordinate currentpos, QGeoCoordinate previewpos);
    QByteArray packetizeNoFly(quint8 NoFly);
    void depacketizePos();
    void processPacket(const QByteArray &instruction);

protected:
    quint8 m_srcID;
    quint8 m_desID;
    double m_lat;
    double m_lon;
    double m_bearing;
    quint8 m_NoFlyZone;
    QByteArray m_payload;
    int m_type;
};

#endif // UBPACKET_H
