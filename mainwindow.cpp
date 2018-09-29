//#define LOG_VERBOSE
//#define LOG_VALUES

#ifdef LOG_VERBOSE
    #include <QDebug>
#else
    #ifdef LOG_VALUES
        #include <QDebug>
    #endif
#endif

#include "mainwindow.h"
#include "chartview.h"
#include "math.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , stepsPerTurn(128)
{
    angleStep = 360.0/double(stepsPerTurn);

    polarRadar.setTheme(QChart::ChartThemeBlueCerulean);
    polarRadar.setAnimationOptions(QChart::SeriesAnimations);
    polarRadar.legend()->hide();

    polarSeries.setName("Distance [cm]");
    polarRadar.addSeries(&polarSeries);
    polarSeries.attachAxis(polarRadar.axisX());
    polarSeries.attachAxis(polarRadar.axisY());

    cartesianRadar.setTheme(QChart::ChartThemeBlueCerulean);
    cartesianRadar.setAnimationOptions(QChart::SeriesAnimations);
    cartesianRadar.legend()->hide();

    cartesianSeries.setName("Distance [cm]");
    cartesianRadar.addSeries(&cartesianSeries);
    cartesianSeries.attachAxis(cartesianRadar.axisX());
    cartesianSeries.attachAxis(cartesianRadar.axisY());

    ChartView* pChartView;
    pChartView = new ChartView();

    pCurrentChart = &polarRadar;
    pChartView->setChart(pCurrentChart);
    pChartView->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(pChartView);
    connect(&polarSeries, SIGNAL(pointReplaced(int)),
            this, SLOT(onPointReplaced()));

    // Arduino Serial Port
    baudRate = QSerialPort::Baud115200;
    waitTimeout = 300;
    responseData.clear();
    connect(&connectionTimer, SIGNAL(timeout()),
            this, SLOT(onTimerConnection()));
    ConnectToArduino();
}


MainWindow::~MainWindow() {
}


void
MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    connectionTimer.stop();
    stopArduino();
}


void
MainWindow::ConnectToArduino() {
    connectionTimer.stop();
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
            qDebug() << QString("No Arduino ready to use !");
            qDebug() << QString("Retrying in 3 seconds");
        #endif
        connectionTimer.start(3000);
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
    requestData.append(qint8(StartMarker));
    requestData.append(qint8(4));
    requestData.append(qint8(AreYouThere));
    requestData.append(qint8(EndMarker));

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
    connectionTimer.start(3000);
    #ifdef LOG_VERBOSE
        qDebug() << QString("No Arduino ready to use !");
        qDebug() << QString("Retrying in 3 seconds");
    #endif
}


void
MainWindow::stopArduino() {
    if(serialPort.isOpen()) {
        requestData.clear();
        requestData.append(qint8(StartMarker));
        requestData.append(qint8(4));
        requestData.append(qint8(StopSending));
        requestData.append(qint8(EndMarker));
        writeSerialRequest(requestData);
        serialPort.waitForBytesWritten();
        QThread::sleep(1);
        serialPort.clear();
        serialPort.close();
    }
}


void
MainWindow::onArduinoFound() {
    connect(&serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
            this, SLOT(onArduinoDisconnected(QSerialPort::SerialPortError)));
    starting = true;
    currElem = 0;
    angle    = 0;
    requestData.clear();
    requestData.append(qint8(StartMarker));
    requestData.append(qint8(4));
    requestData.append(qint8(StartSending));
    requestData.append(qint8(EndMarker));
    writeSerialRequest(requestData);
}


void
MainWindow::onArduinoDisconnected(QSerialPort::SerialPortError error) {
    Q_UNUSED(error)
    #ifdef LOG_VERBOSE
        qDebug() << error << serialPort.errorString();
        serialPort.clearError();
    #endif
    ConnectToArduino();
}


void
MainWindow::onArduinoConnectionTimerTimeout() {
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
        connectionTimer.start(3000);
    #ifdef LOG_VERBOSE
        qDebug() << QString("No Arduino ready to use !");
        qDebug() << QString("Retrying in 3 seconds");
    #endif
}


void
MainWindow::onTimerConnection() {
    connectionTimer.stop();
    serialPort.close();
    ConnectToArduino();
}


void
MainWindow::onSerialDataAvailable() {
    QSerialPort *testPort = static_cast<QSerialPort *>(sender());
    responseData.append(testPort->readAll());
    while(!testPort->atEnd()) {
        responseData.append(testPort->readAll());
    }
    while(responseData.count() > 0) {
        // Do we have a complete command ?
        int iStart = responseData.indexOf(qint8(StartMarker));
        if(iStart == -1) {
            responseData.clear();
            return;
        }
        if(iStart > 0)
            responseData.remove(0, iStart);
        int iEnd   = responseData.indexOf(qint8(EndMarker));
        if(iEnd == -1) return;
        executeCommand(decodeResponse(responseData.left(responseData[1])));
        responseData.remove(0, responseData[1]);
    }
}


QByteArray
MainWindow::decodeResponse(QByteArray response) {
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
                decodedResponse.append(qint8(SpecialByte)+quint8(response[i]));
            }
            else { // Not enough character received
                decodedResponse.clear();
                return decodedResponse;
           }
        }
        else
            decodedResponse.append(qint8(response[i]));
    }
    if(decodedResponse.count() != 0)
        decodedResponse[0] = decodedResponse[0]-2;
    return decodedResponse;
}


bool
MainWindow::executeCommand(QByteArray command) {
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

    if(quint8(command[1]) == quint8(StepsMarker)) {
        stepsPerTurn = (quint8(command[3]) << 8) + quint8(command[2]);
        angleStep = 360.0/double(stepsPerTurn);
        #ifdef LOG_VERBOSE
            qDebug() << QString("Steps per turn: %1")
                       .arg(stepsPerTurn);
        #endif
        return true;
    }

    if(quint8(command[1]) == quint8(AngleMarker)) {
        int iAngle = (quint8(command[3]) << 8) + quint8(command[2]);
        if(starting)
            if(angle > iAngle) {
                starting = false;
                nElem = polarSeries.count();
                currElem = nElem-2;
                iDirection = -1;
                #ifdef LOG_VERBOSE
                    qDebug() << "Backward";
                #endif
            }
        angle = iAngle;
        return true;
    }

    if(quint8(command[1]) == quint8(DistMarker)) {
        int iDist = (quint8(command[3]) << 8) + quint8(command[2]);
        distance = double(iDist);
        double radiants = M_PI*angle/180.0;
        double x = distance*cos(radiants);
        double y = distance*sin(radiants);
        if(!starting) {
            polarSeries.replace(currElem, angle, distance);
            cartesianSeries.replace(currElem, x, y);
            currElem += iDirection;
            if(currElem < 0) {
                iDirection = -iDirection;
                currElem = 1;
            }
            else if(currElem >= nElem) {
                iDirection = - iDirection;
                currElem = nElem-2;
            }
        }
        else {
            polarSeries.append(angle, distance);
            cartesianSeries.append(x, y);
            currElem++;
        }
        #ifdef LOG_VERBOSE
            qDebug() << "Angle= " << angle
                     << "Rads = " << radiants
                     << "Dist = " << distance
                     << "X    = " << x
                     << "Y    = " << y;
        #endif
        return true;
    }

    // ValuesMarker
    if(quint8(command[1]) == quint8(ValuesMarker)) {
        int iStep = (quint8(command[3]) << 8) + quint8(command[2]);
        int iDist = (quint8(command[5]) << 8) + quint8(command[4]);
        angle = double(iStep)*angleStep;
        distance = double(iDist);
        double radiants = M_PI*angle/180.0;
        double x = distance*cos(radiants);
        double y = distance*sin(radiants);
        if(iStep >= polarSeries.count()) {
            polarSeries.append(angle, distance);
            cartesianSeries.append(x, y);
        }
        else {
            polarSeries.replace(iStep, angle, distance);
            cartesianSeries.replace(iStep, x, y);
        }
        #ifdef LOG_VALUES
            qDebug() << "Step#= " << iStep
                     << "Angle= " << angle
                     << "Rads = " << radiants
                     << "Dist = " << distance
                     << "X    = " << x
                     << "Y    = " << y;
        #endif
        return true;
    }
    #ifdef LOG_VERBOSE
        qDebug() << "Unknown Command "
                 << quint8(command[1])
                  << " length="
                 << command.length();
    #endif
    return false;
}


int
MainWindow::writeSerialRequest(QByteArray requestData) {
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
MainWindow::onPointReplaced() {
    update();
}


void
MainWindow::wheelEvent(QWheelEvent *event) {
    int degrees = event->delta() / 16;
    if(pCurrentChart == &polarRadar) {
        QValueAxis* pAxis = static_cast<QValueAxis*>(polarRadar.axisY());
        if((degrees > 0) &&  (pAxis->max() > 10.0))
            pAxis->setMax(pAxis->max() - 5.0);
        else if(degrees < 0)
            pAxis->setMax(pAxis->max() + 5.0);
    }
    else {
        QValueAxis* pAxisX = static_cast<QValueAxis*>(cartesianRadar.axisX());
        QValueAxis* pAxisY = static_cast<QValueAxis*>(cartesianRadar.axisY());
        if((degrees > 0) &&  (pAxisX->max() > 10.0)) {
            pAxisX->setMax(pAxisX->max() - 5.0);
            pAxisX->setMin(pAxisX->min() + 5.0);
            pAxisY->setMax(pAxisY->max() - 5.0);
            pAxisY->setMin(pAxisY->min() + 5.0);
        }
        else if(degrees < 0) {
            pAxisX->setMax(pAxisX->max() + 5.0);
            pAxisX->setMin(pAxisX->min() - 5.0);
            pAxisY->setMax(pAxisY->max() + 5.0);
            pAxisY->setMin(pAxisY->min() - 5.0);
        }
    }
}


void
MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_C) {
        qDebug() << "C";
        if(pCurrentChart == &polarRadar) {
            disconnect(&polarSeries, SIGNAL(pointReplaced(int)),
                    this, SLOT(onPointReplaced()));
            pCurrentChart = &cartesianRadar;
            static_cast<ChartView *>(centralWidget())->setChart(pCurrentChart);
            connect(&cartesianSeries, SIGNAL(pointReplaced(int)),
                    this, SLOT(onPointReplaced()));
        }
    }
    else if(event->key() == Qt::Key_P) {
        qDebug() << "P";
        if(pCurrentChart == &cartesianRadar) {
            disconnect(&cartesianSeries, SIGNAL(pointReplaced(int)),
                    this, SLOT(onPointReplaced()));
            pCurrentChart = &polarRadar;
            static_cast<ChartView *>(centralWidget())->setChart(pCurrentChart);
            connect(&polarSeries, SIGNAL(pointReplaced(int)),
                    this, SLOT(onPointReplaced()));
        }
    }

}

