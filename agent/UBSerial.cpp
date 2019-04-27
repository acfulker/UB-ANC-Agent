#include "UBAgent.h"
#include "UBNetwork.h"
#include "UBPacket.h"
#include "UBConfig.h"
#include <QTimer>
#include <QCommandLineParser>
#include "Vehicle.h"
#include "TCPLink.h"
#include "QGCApplication.h"
#include  <QSerialPort>

 
SerialConfiguration* serial = new SerialConfiguration("Serial Port");
serial->setBaud(BAUD_RATE);
serial->setPortName(SERIAL_PORT);
