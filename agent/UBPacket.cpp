#include "UBPacket.h"
UBPacket::UBPacket() : m_srcID(0),
    m_desID(0)
{

}

QByteArray UBPacket::packetize(void) {
    QByteArray src(((char*)(&m_srcID)), sizeof(quint8));
    QByteArray des(((char*)(&m_desID)), sizeof(quint8));

    return src + des + m_payload;
}
QByteArray UBPacket::packetizePos(QGeoCoordinate pos) // Designed packets (In progress)
{
  QByteArray header = QByteArray::number(0000, 4)
  QByteArray lat = QByteArray::number(pos.latitude(), 'g', 10); // (10 total digits ( 2 integers and 8 decimal points after) equals 33 bytes)
  lat = lat.rightJustified(10, '0', true);


  QByteArray lon = QByteArray::number(pos.longitude(), 'g', 10)
  lon = lon.rightJustified(10, '0', true);
  QGeoCoordinate newpos = QGeoCoordinate(lat, lon) // new coordinates

  QByteArray bearing = azimuthTo(QGeoCoordinate newpos & QGeoCoordinate pos) // calculates drone bearing
  bearing = bearing.rightJustified(3, '0', true); // bearing relative north
  return header + lat + lon + bearing;
}
void UBPacket::depacketize(const QByteArray& packet) {
    m_srcID = *((quint8*)(packet.mid(0, sizeof(quint8)).data()));
    m_desID = *((quint8*)(packet.mid(0 + sizeof(quint8), sizeof(quint8)).data()));

    m_payload = packet.mid(0 + sizeof(quint8) + sizeof(quint8));
QByteArray UBPacket::processpacketsPos(const QByteArray & packet)
{
  header = packet.mid(0,4)
  switch (header)

  case "0000" // read first 4 numbers of our received packet
    m_mission_state = STATE_IDLE;
    break;
  case "0001"
    break;
  case "0010"
    break;
  case "0100"
    break;
  case "1000"
    break;
  default:
    break;
}
}
