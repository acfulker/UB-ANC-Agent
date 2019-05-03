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
  QByteArray header ("1");
  QByteArray lat = QByteArray::number(currentpos.latitude(), 'g', 8); // (10 total digits ( 2 integers and 8 decimal points after) equals 20 total bytes)
  lat = lat.rightJustified(8, '0', true);


  QByteArray lon = QByteArray::number(currentpos.longitude(), 'g', 8);
  lon = lon.rightJustified(8, '0', true);

  double bearing = previewpos.azimuthTo(currentpos); // calculates drone bearing as integer
  QByteArray bearingarray = QByteArray::number(bearing, 'g', 3);
  bearingarray = bearingarray.rightJustified(3, '0', true); // bearing relative north
  //qInfo()<<header + lat + lon + bearingarray << endl;
  return header + lat + lon + bearingarray;
}

QByteArray UBPacket::packetizeNoFly(quint8 NoFly) { //in progress..
  QByteArray header ("0");
  QByteArray NoFlyZone(((char*)(&NoFly)), sizeof(quint8));
  return header + NoFlyZone; 
}

void UBPacket::depacketize(const QByteArray& packet) {
    m_srcID = *((quint8*)(packet.mid(0, sizeof(quint8)).data()));
    m_desID = *((quint8*)(packet.mid(0 + sizeof(quint8), sizeof(quint8)).data()));

    m_payload = packet.mid(0 + sizeof(quint8) + sizeof(quint8));
}

void UBPacket::depacketizePos(){ //
  m_lat = m_payload.mid(0+1, 25).toDouble();
  m_lon =  m_payload.mid(0+1+25, 25).toDouble();
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

