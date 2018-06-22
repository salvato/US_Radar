#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include "polarwidget.h"
#include "cartesianwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

signals:
    void arduinoFound();

public slots:
    void onSerialDataAvailable();
    void onArduinoConnectionTimerTimeout();
    void onPointReplaced();
    void onArduinoFound();
    void onArduinoDisconnected(QSerialPort::SerialPortError error);
    void onTimerConnection();

protected:
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void ConnectToArduino();
    void stopArduino();
    int  writeSerialRequest(QByteArray requestData);
    QByteArray decodeResponse(QByteArray response);
    bool executeCommand(QByteArray command);

protected:
    QSerialPort            serialPort;
    QSerialPortInfo        serialPortinfo;
    QList<QSerialPortInfo> serialPorts;
    QSerialPort::BaudRate  baudRate;
    int                    currentPort;
    int                    waitTimeout;
    QByteArray             requestData;
    QByteArray             responseData;
    QTimer                 arduinoConnectionTimer;

    const quint8           StartMarker  = quint8(0xFF);
    const quint8           EndMarker    = quint8(0xFE);
    const quint8           AngleMarker  = quint8(0xFD);
    const quint8           DistMarker   = quint8(0xFC);
    const quint8           ValuesMarker = quint8(0xFB);
    const quint8           StepsMarker  = quint8(0xFA);
    const quint8           SpecialByte  = quint8(0x80);
    const quint8           StartSending = quint8(0x81);
    const quint8           StopSending  = quint8(0x82);
    const quint8           AreYouThere  = quint8(0xAA);

private:
    QChart* pCurrentChart;
    PolarWidget polarRadar;
    CartesianWidget cartesianRadar;
    QLineSeries polarSeries;
    QLineSeries cartesianSeries;
    double angle;
    double angleStep;
    double distance;
    int stepsPerTurn;
    bool starting;
    int nElem, currElem, iDirection;
    QTimer connectionTimer;
};

#endif // MAINWINDOW_H
