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
QByteArray UBPacket::packetizePos(QGeoCoordinate currentpos, QGeoCoordinate previewpos) // Designed packets (In progress)
{
  QByteArray lat = QByteArray::number(currentpos.latitude(), 'g', 11); // (10 total digits ( 2 integers and 8 decimal points after) equals 33 bytes)
  lat = lat.rightJustified(11, '0', true);


  QByteArray lon = QByteArray::number(currentpos.longitude(), 'g', 11);
  lon = lon.rightJustified(11, '0', true);

  double bearing = previewpos.azimuthTo(currentpos); // calculates drone bearing as integer
  QByteArray bearingarray = QByteArray::number(bearing, 'g', 10);
  bearingarray = bearingarray.rightJustified(3, '0', true); // bearing relative north
  qInfo()<<lat + lon + bearingarray << endl;
  return lat + lon + bearingarray;
}

 void UBPacket::depacketize(const QByteArray& packet) {
    m_srcID = *((quint8*)(packet.mid(0, sizeof(quint8)).data()));
    m_desID = *((quint8*)(packet.mid(0 + sizeof(quint8), sizeof(quint8)).data()));

    m_payload = packet.mid(0 + sizeof(quint8) + sizeof(quint8));
}

void UBPacket::depacketizePos(){ //
  m_lat = *((double*)(m_payload.mid(0+4, 10).data()));
  m_lon =  *((double*)(m_payload.mid(0+4+10, 10).data()));
  m_bearing = *((double*)(m_payload.mid(0+4+10+3, 10).data()));

}

void UBPacket::depacketizeNoFly(){

}

UBPacket UBPacket::processPacket(const QByteArray &instruction)
{
  UBPacket pkt;
  QByteArray opt1("0000"); // first 4 numbers of received packets
  QByteArray opt2("0001");

  QByteArray header = instruction.mid(0,4);
  pkt.setPayload(instruction);
  if (header == opt1){
    pkt.depacketizeNoFly();

  }
  else if (header == opt2) {
    pkt.depacketizePos();

  }
  else {
    qInfo() << "Header not supported";

  }
  return pkt;
}
