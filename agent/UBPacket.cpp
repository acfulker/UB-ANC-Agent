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
  QByteArray lat = QByteArray::number(currentpos.latitude(), 'g', 10); // (10 total digits ( 2 integers and 8 decimal points after) equals 33 bytes)
  lat = lat.rightJustified(10, '0', true);


  QByteArray lon = QByteArray::number(currentpos.longitude(), 'g', 10);
  lon = lon.rightJustified(10, '0', true);

  double bearing = previewpos.azimuthTo(currentpos); // calculates drone bearing as integer
  QByteArray bearingarray = QByteArray::number(bearing, 'g', 10);
  bearingarray = bearingarray.rightJustified(3, '0', true); // bearing relative north
//  qInfo()<<lat + lon + bearingarray << endl;
  return lat + lon + bearingarray;
}
void UBPacket::depacketize(const QByteArray& packet) {
    m_srcID = *((quint8*)(packet.mid(0, sizeof(quint8)).data()));
    m_desID = *((quint8*)(packet.mid(0 + sizeof(quint8), sizeof(quint8)).data()));

    m_payload = packet.mid(0 + sizeof(quint8) + sizeof(quint8));
}
/*void depacketizepos(const QByteArray &packet){
  QByteArray header = packet.mid(0,4);
  depacketize_data.currentpos = data.mid(0+4, 10).toDouble(), data.mid(0+4+10, 10).toDouble(), data.mid(0+4+10+3, 10).toDouble();
}

QByteArray UBPacket::processpacket(const QByteArray &packet)
{
  QByteArray opt1("0000"); // first 4 numbers of received packets
  QByteArray opt2("0001");
  QByteArray opt3("0010");
  QByteArray opt4("0100");
  QByteArray opt5("1000");

  QByteArray header = packet.mid(0,4);
  if (header == opt1){


  }
  else if (header == opt2) {


  }
  else if (header == opt3) {


  }
  else if (header == opt4) {


  }
  else if (header == opt5) {


  }
  else {
    qInfo() << "Header not supported";

  }
}*/
