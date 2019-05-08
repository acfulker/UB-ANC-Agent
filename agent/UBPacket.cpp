#include "UBPacket.h"
#include <QDebug>
UBPacket::UBPacket() : m_srcID(0),
    m_desID(0)
{

}

QByteArray UBPacket::packetize(void) {
    QByteArray src(((char*)(&m_srcID)), sizeof(quint8));
    QByteArray des(((char*)(&m_desID)), sizeof(quint8));

    return src + des + m_payload;
}

QByteArray UBPacket::packetizePos(QGeoCoordinate currentpos, QGeoCoordinate previewpos){ // Designed packets
  QByteArray lat = QByteArray::number(currentpos.latitude(), 'g', 11); // (10 total digits ( 2 integers and 8 decimal points after) equals 20 total bytes)
  lat = lat.rightJustified(11, '0', true);
  QByteArray lon = QByteArray::number(currentpos.longitude(), 'g', 11);
  lon = lon.rightJustified(11, '0', true);
  double bearing = previewpos.azimuthTo(currentpos); // calculates drone bearing as integer
  QByteArray bearingarray = QByteArray::number(bearing, 'g', 3);
  bearingarray = bearingarray.rightJustified(3, '0', true); // bearing relative north
  //qInfo()<<lat + lon + bearingarray << endl;
  return lat + lon + bearingarray;
}

void UBPacket::depacketize(const QByteArray& packet) {
    m_srcID = *((quint8*)(packet.mid(0, sizeof(quint8)).data()));
    m_desID = *((quint8*)(packet.mid(0 + sizeof(quint8), sizeof(quint8)).data()));

    m_payload = packet.mid(0 + sizeof(quint8) + sizeof(quint8));
}

void UBPacket::depacketizePos(){
  m_lat = m_payload.mid(0+1, 11).toDouble();
  m_lon =  m_payload.mid(0+1+11, 11).toDouble();
  qInfo()<<"m_lat:"<<m_lat<<"m_lon:"<<m_lon<<endl;
  m_type = POS;
}

void UBPacket::depacketizeNoFly(){
  m_NoFlyZone = *((quint8*)(m_payload.mid(0+1, sizeof(quint8)).data()));
  m_type = NOFLYZONE;
}

void UBPacket::processPacket(const QByteArray &instruction)
{
  QByteArray opt1("0");
  QByteArray opt2("1");

  QByteArray header = instruction.mid(0,1);
  this->setPayload(instruction);
  if (header == "0"){
    this->depacketizeNoFly();
    qInfo()<<"Decoding No Fly"<<endl;
  }
  else if (header == "1") {
    this->depacketizePos();
    qInfo()<<"Decoding Position"<<endl;
  }
  else {
    qInfo() << "Header not supported";

  }
}

