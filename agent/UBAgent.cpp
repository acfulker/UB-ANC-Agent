#include "UBAgent.h"
#include "UBSerial.h"
#include "UBPacket.h"
#include "UBConfig.h"
#include "UBNetwork.h"
#include <QTimer>
#include <QCommandLineParser>

#include "Vehicle.h"
#include "TCPLink.h"
#include "QGCApplication.h"

UBAgent::UBAgent(QObject *parent) : QObject(parent),
    m_mav(nullptr)
{
    m_net = new UBNetwork;
    connect(m_net, SIGNAL(dataReady(quint8, QByteArray)), this, SLOT(dataReadyEvent(quint8, QByteArray)));
    m_serial = new UBSerial;
    connect(m_serial, SIGNAL(dataReadySerial(UBPacket)), this, SLOT(dataReadyEventSerial(UBPacket)));
    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(missionTracker()));

    startAgent();
}

void UBAgent::startAgent() {
    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    //m_serial<<sendData()
    parser.addOptions({
        {{"I", "instance"}, "Set instance (ID) of the agent", "id"},
    });

//    parser.process(*QCoreApplication::instance());
    parser.parse(QCoreApplication::arguments());

    quint8 id = parser.value("I").toUInt();
    LinkConfiguration* link = nullptr;
    if (id) {
        quint32 port = 10 * id + STL_PORT + 3;
        TCPConfiguration* tcp = new TCPConfiguration(tr("TCP Port %1").arg(port));
        tcp->setAddress(QHostAddress::LocalHost);
        tcp->setPort(port);

        link = tcp;
    } else {
        SerialConfiguration* serial = new SerialConfiguration("Serial Port");
        serial->setBaud(BAUD_RATE);
        serial->setPortName(SERIAL_PORT);

        link = serial;
    }

    link->setDynamic();
    link->setAutoConnect();

    LinkManager* linkManager = qgcApp()->toolbox()->linkManager();
    linkManager->addConfiguration(link);
    linkManager->linkConfigurationsChanged();

    connect(qgcApp()->toolbox()->multiVehicleManager(), SIGNAL(vehicleAdded(Vehicle*)), this, SLOT(vehicleAddedEvent(Vehicle*)));
    connect(qgcApp()->toolbox()->multiVehicleManager(), SIGNAL(vehicleRemoved(Vehicle*)), this, SLOT(vehicleRemovedEvent(Vehicle*)));

    m_net->connectToHost(QHostAddress::LocalHost, 10 * id + NET_PORT);
    m_timer->start(MISSION_TRACK_RATE);
}

void UBAgent::setMAV(Vehicle* mav) {
    if (m_mav) {
        disconnect(m_mav, SIGNAL(armedChanged(bool)), this, SLOT(armedChangedEvent(bool)));
        disconnect(m_mav, SIGNAL(flightModeChanged(QString)), this, SLOT(flightModeChangedEvent(QString)));
    }

    m_mav = mav;

    if (m_mav) {
        connect(m_mav, SIGNAL(armedChanged(bool)), this, SLOT(armedChangedEvent(bool)));
        connect(m_mav, SIGNAL(flightModeChanged(QString)), this, SLOT(flightModeChangedEvent(QString)));
    }
}

void UBAgent::vehicleAddedEvent(Vehicle* mav) {
    if (!mav || m_mav == mav) {
        return;
    }

    setMAV(mav);
    m_net->setID(mav->id());

    qInfo() << "New MAV connected with ID: " << m_mav->id();
}

void UBAgent::vehicleRemovedEvent(Vehicle* mav) {
    if (!mav || m_mav != mav) {
        return;
    }

    setMAV(nullptr);
    m_net->setID(0);

    qInfo() << "MAV disconnected with ID: " << mav->id();
}

void UBAgent::armedChangedEvent(bool armed) {

    if (!armed) {
      m_mission_stage = STAGE_IDLE;
      return;

    }
    if(m_NoFlyZone){ //If you are armed and in the no fly zone disarm the drone
        m_mission_stage = STAGE_IDLE;
        m_mav -> setArmed(false);
        qInfo()<< "You are in no fly zone, drone is disarming";
        return;
    }
    if (m_mav->altitudeRelative()->rawValue().toDouble() > POINT_ZONE) {
        qWarning() << "The mission can not start while the drone is airborne!";
        return;
    }

//    m_mav->setGuidedMode(true);
    if (!m_mav->guidedMode()) {
        qWarning() << "The mission can not start while the drone is not in Guided mode!";
        return;
    }

    m_mission_data.reset();
    m_mission_stage = STAGE_TAKEOFF;
    qInfo() << "Mission starts...";
    m_currentpos = m_mav->coordinate(); //initializing current and previous position
    m_previouspos = m_currentpos;

//    m_mav->guidedModeTakeoff();
    m_mav->sendMavCommand(m_mav->defaultComponentId(),
                            MAV_CMD_NAV_TAKEOFF,
                            true, // show error
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            TAKEOFF_ALT);
}

void UBAgent::flightModeChangedEvent(QString mode) {
    qInfo() << mode;
}

void UBAgent::dataReadyEvent(quint8 srcID, QByteArray data) {
    Q_UNUSED(data)
    if(srcID == m_mav->id() - 1 && !m_mav->armed()) {
        m_mav->setArmed(true);
    }
}
void UBAgent::dataReadyEventSerial(UBPacket packet){
   m_type = packet.getType();
   if(m_type ==1) { // you are in the fly zone
     static QGeoCoordinate dest(packet.getLat(), packet.getLon());
     m_mav->guidedModeGotoLocation(dest);
   }
    if(m_type == 0) // you are in the no fly zone
    m_NoFlyZone = packet.getNoFly();
      if(m_NoFlyZone){
        qInfo()<<"You are in the no fly zone and cannot fly"<<endl;
      }
   }

void UBAgent::missionTracker() {
    m_previouspos = m_currentpos;
    m_currentpos = m_mav->coordinate();
    /*NoFly = 1;
    qInfo()<<"currentpos lat: "<< currentpos.latitude()<<"currentpos lon: "<< currentpos.longitude()<<endl;
    double bearing = previewpos.azimuthTo(currentpos); // calculates drone bearing as integer
    qInfo()<<"bearing = " << bearing << endl ; */ // displays value of bearing in degrees
    UBPacket txPkt;
//    UBPacket rxPkt;
/*    UBPacket txPkt2;
    UBPacket rxPkt2; */
    QByteArray position_update_data = txPkt.packetizePos(m_currentpos, m_previouspos);
    m_serial->sendData(position_update_data);
  //  QByteArray instruction2 = txPkt2.packetizeNoFly(NoFly);
  //  rxPkt.processPacket(instruction);
  //  rxPkt2.processPacket(instruction2);
    //qInfo()<<"m_lat: "<<rxPkt.getLat()<<"m_lon: "<<rxPkt.getLon()<< endl;
  //  qInfo()<<"m_NoFlyZone"<<rxPkt2.getNoFly()<<endl;
    switch (m_mission_stage) {
    case STAGE_IDLE:
        stageIdle();
        break;
    case STAGE_TAKEOFF:
        stageTakeoff();
        break;
    case STAGE_MISSION:
        stageMission();
        break;
    case STAGE_LAND:
        stageLand();
        break;
    default:
        break;
    }
}

void UBAgent::stageIdle() {
}

void UBAgent::stageTakeoff() {
    if (m_mav->altitudeRelative()->rawValue().toDouble() > TAKEOFF_ALT - POINT_ZONE) {
        m_mission_data.stage = 0;
        m_mission_stage = STAGE_MISSION;
    }
}

void UBAgent::stageLand() {
    if (m_mav->altitudeRelative()->rawValue().toDouble() < POINT_ZONE) {
        m_mission_stage = STAGE_IDLE;
        qInfo() << "Mission ends";
    }
}

void UBAgent::stageMission() {
    static QGeoCoordinate dest;

    if (m_mission_data.stage == 0) {
        m_mission_data.stage++;

        dest = m_mav->coordinate().atDistanceAndAzimuth(100, 90); // 0 -> North, 90 (M_PI / 2) -> East
        m_mav->guidedModeGotoLocation(dest);

        return;
    }

    if (m_mission_data.stage == 1) {
        if (m_mav->coordinate().distanceTo(dest) < POINT_ZONE) {
            m_mission_data.stage++;
        }

        return;
    }

    if (m_mission_data.tick < (20 * 1000 / MISSION_TRACK_RATE)) {
        m_mission_data.tick++;
        m_net->sendData(m_mav->id() + 1, QByteArray(1, MAV_CMD_NAV_TAKEOFF));
    } else {
        m_mav->guidedModeLand();
        m_mission_stage = STAGE_LAND;
    }
}
