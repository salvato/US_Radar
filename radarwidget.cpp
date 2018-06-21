//#define LOG_VERBOSE

#include "radarwidget.h"
#include <QSerialPortInfo>

#ifdef LOG_VERBOSE
#include <QDebug>
#endif




RadarWidget::RadarWidget(QGraphicsItem *parent)
    : QPolarChart(parent)
{
    setTitle("Ultrasonic Radar");

    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
    angularAxis->setLabelFormat("%.1f");
    angularAxis->setShadesVisible(false);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(5);
    radialAxis->setMinorTickCount(1);
    radialAxis->setLabelFormat("%d");
    radialAxis->setShadesVisible(false);
    addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    radialAxis->setRange(0.0, 300.0); // Distance in cm
    angularAxis->setRange(0.0, 360.0);// Angles in degrees

    series1 = new QLineSeries();
    series1->setName("Distance [cm]");
    addSeries(series1);
    series1->attachAxis(radialAxis);
    series1->attachAxis(angularAxis);
    angle = 0;
    connect(series1, SIGNAL(pointReplaced(int)),
            this, SLOT(onPointReplaced()));

    // Arduino Serial Port
    baudRate = QSerialPort::Baud115200;
    waitTimeout = 300;
    responseData.clear();
    ConnectToArduino();
}


void
RadarWidget::stopArduino() {
    if(serialPort.isOpen()) {
        requestData.clear();
        requestData.append(quint8(StartMarker));
        requestData.append(quint8(4));
        requestData.append(quint8(StopSending));
        requestData.append(quint8(EndMarker));
        writeSerialRequest(requestData);
        serialPort.waitForBytesWritten();
        QThread::sleep(1);
        serialPort.clear();
        serialPort.close();
    }
}


void
RadarWidget::ConnectToArduino() {
    // Get a list of available serial ports
    serialPorts = QSerialPortInfo::availablePorts();
    // Remove from the list NON tty and already opened devices
    for(int i=0; i<serialPorts.count(); i++) {
        serialPortinfo = serialPorts.at(i);
        if(!serialPortinfo.portName().contains("tty"))
            serialPorts.removeAt(i);
        serialPort.setPortName(serialPortinfo.portName());
        if(serialPort.isOpen())
            serialPorts.removeAt(i);
    }
    // Do we have still serial ports ?
    if(serialPorts.isEmpty()) {
        #ifdef LOG_VERBOSE
            qDebug() << QString("No serial port available");
        #endif
        return;
    }
    connect(this, SIGNAL(arduinoFound()),
            this, SLOT(onArduinoFound()));
    // Yes we have serial ports available:
    // Search for the one connected to Arduino
    baudRate = QSerialPort::Baud115200;
    waitTimeout = 2000;
    connect(&arduinoConnectionTimer, SIGNAL(timeout()),
            this, SLOT(onArduinoConnectionTimerTimeout()));

    requestData.clear();
    requestData.append(quint8(StartMarker));
    requestData.append(quint8(4));
    requestData.append(quint8(AreYouThere));
    requestData.append(quint8(EndMarker));

    for(currentPort=0; currentPort<serialPorts.count(); currentPort++) {
        serialPortinfo = serialPorts.at(currentPort);
        serialPort.setPortName(serialPortinfo.portName());
        serialPort.setBaudRate(baudRate);
        serialPort.setDataBits(QSerialPort::Data8);
        if(serialPort.open(QIODevice::ReadWrite)) {
            #ifdef LOG_VERBOSE
                    qDebug() << QString("Trying connection to %1")
                                .arg(serialPortinfo.portName());
            #endif
            // Arduino will be reset upon a serial connectiom
            // so give time to set it up before communicating.
            QThread::sleep(3);
            serialPort.clear();
            connect(&serialPort, SIGNAL(readyRead()),
                    this, SLOT(onSerialDataAvailable()));
            writeSerialRequest(requestData);
            arduinoConnectionTimer.start(waitTimeout);
            return;
        }
        #ifdef LOG_VERBOSE
        else {
            qDebug() << QString("Unable to open %1 because %2")
                       .arg(serialPortinfo.portName())
                       .arg(serialPort.errorString());
        }
        #endif
    }
    #ifdef LOG_VERBOSE
        qDebug() << QString("No Arduino ready to use !");
    #endif
}


void
RadarWidget::onArduinoFound() {
    starting = true;
    currElem = 0;
    requestData.clear();
    requestData.append(quint8(StartMarker));
    requestData.append(quint8(4));
    requestData.append(quint8(StartSending));
    requestData.append(quint8(EndMarker));
    writeSerialRequest(requestData);
}


void
RadarWidget::onArduinoConnectionTimerTimeout() {
    arduinoConnectionTimer.stop();
    serialPort.disconnect();
    serialPort.close();
    for(++currentPort; currentPort<serialPorts.count(); currentPort++) {
        serialPortinfo = serialPorts.at(currentPort);
        serialPort.setPortName(serialPortinfo.portName());
        serialPort.setBaudRate(baudRate);
        serialPort.setDataBits(QSerialPort::Data8);
        if(serialPort.open(QIODevice::ReadWrite)) {
            #ifdef LOG_VERBOSE
                qDebug() << QString("Trying connection to %1")
                          .arg(serialPortinfo.portName());
            #endif
            // Arduino will be reset upon a serial connectiom
            // so give time to set it up before communicating.
            QThread::sleep(3);
            serialPort.clear();
            connect(&serialPort, SIGNAL(readyRead()),
                    this, SLOT(onSerialDataAvailable()));
            writeSerialRequest(requestData);
            arduinoConnectionTimer.start(waitTimeout);
            return;
        }
        #ifdef LOG_VERBOSE
        else {
            qDebug() << QString("Unable to open %1 because %2")
                       .arg(serialPortinfo.portName())
                       .arg(serialPort.errorString());
        }
        #endif
    }
    #ifdef LOG_VERBOSE
        qDebug() << QString("Error: No Arduino ready to use !");
    #endif
}


int
RadarWidget::writeSerialRequest(QByteArray requestData) {
    if(!serialPort.isOpen()) {
        #ifdef LOG_VERBOSE
            qDebug() <<  QString("Serial port %1 has been closed")
                         .arg(serialPort.portName());
        #endif
        return -1;
    }
    responseData.clear();
    serialPort.write(requestData);
    return 0;
}


void
RadarWidget::onSerialDataAvailable() {
    QSerialPort *testPort = static_cast<QSerialPort *>(sender());
    responseData.append(testPort->readAll());
    while(!testPort->atEnd()) {
        responseData.append(testPort->readAll());
    }
    while(responseData.count() > 0) {
        // Do we have a complete command ?
        int iStart = responseData.indexOf(quint8(StartMarker));
        if(iStart == -1) {
            responseData.clear();
            return;
        }
        if(iStart > 0)
            responseData.remove(0, iStart);
        int iEnd   = responseData.indexOf(quint8(EndMarker));
        if(iEnd == -1) return;
        executeCommand(decodeResponse(responseData.left(responseData[1])));
        responseData.remove(0, responseData[1]);
    }
}


QByteArray
RadarWidget::decodeResponse(QByteArray response) {
    QByteArray decodedResponse;
    int iStart;
    // Discard all data up to (and including) StartMarker
    for(iStart=0; iStart<response.count(); iStart++) {
        if(quint8(response[iStart]) == quint8(StartMarker)) break;
    }
    for(int i=iStart+1; i<response.count(); i++) {
        if(quint8(response[i]) == EndMarker) {
            break;
        }
        if((quint8(response[i]) == quint8(SpecialByte))) {
            i++;
            if(i<response.count()) {
                decodedResponse.append(quint8(SpecialByte)+quint8(response[i]));
            }
            else { // Not enough character received
                decodedResponse.clear();
                return decodedResponse;
           }
        }
        else
            decodedResponse.append(quint8(response[i]));
    }
    if(decodedResponse.count() != 0)
        decodedResponse[0] = decodedResponse[0]-2;
    return decodedResponse;
}


bool
RadarWidget::executeCommand(QByteArray command) {
    QString sFunctionName = " TimedScorePanel::executeCommand ";
    Q_UNUSED(sFunctionName)
    if(quint8(command[1]) == quint8(AreYouThere)) {
        arduinoConnectionTimer.stop();
        #ifdef LOG_VERBOSE
            qDebug() << QString("Arduino found at: %1")
                       .arg(serialPort.portName());
        #endif
        emit arduinoFound();
        return true;
    }

    if(quint8(command[1]) == quint8(AngleMarker)) {
        quint16 iAngle = (quint8(command[3]) << 8) + quint8(command[2]);
        if(starting)
            if(angle > iAngle) {
                starting = false;
                nElem = series1->count();
                currElem = nElem-2;
                iDir = -1;
                #ifdef LOG_VERBOSE
                    qDebug() << "Backward";
                #endif
            }
        angle = iAngle;
        return true;
    }

    if(quint8(command[1]) == quint8(DistMarker)) {
        quint16 iDist = (quint8(command[3]) << 8) + quint8(command[2]);
        distance = iDist;
        #ifdef LOG_VERBOSE
            qDebug() << "Angle= "
                     << angle
                     << "Distance= "
                     << distance
                     << "Elem "
                     << currElem;
        #endif
        if(!starting) {
            series1->replace(currElem, angle, distance);
            currElem += iDir;
            if(currElem < 0) {
                iDir = -iDir;
                currElem = 1;
            }
            else if(currElem >= nElem) {
                iDir = - iDir;
                currElem = nElem-2;
            }
        }
        else {
            series1->append(angle, distance);
            currElem++;
            update();
        }
        return true;
    }
    #ifdef LOG_VERBOSE
        qDebug() << "Unknown Command length="
                 << command.length();
    #endif
    return false;
}


void
RadarWidget::onPointReplaced() {
    update();
}


void
RadarWidget::onTimerUpdate() {
    double dist = QRandomGenerator::global()->bounded(100.0);
    angle += 10.0;
    if(angle > 360.0) series1->removePoints(0, 1);
    series1->append(fmod(angle, 360.0), dist);
    scroll(0.0, 0.0);
}
