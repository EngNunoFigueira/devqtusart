#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <stdio.h>
#include <iostream>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "This programming will testing the USAR RS232 using the QT Object";
    int portIndex=0;
    bool rxTimeout = false;
    QSerialPort uartPort;
    qDebug() << "Number of serial ports detected on system:" << QSerialPortInfo::availablePorts().count();
    foreach(QSerialPortInfo port, QSerialPortInfo::availablePorts())
    {
        if (portIndex == 0)
        {
            qDebug() << "Setting Serial Port configurations";
            uartPort.setPortName("ttyUSB0");
            uartPort.error(QSerialPort::OpenError);
            uartPort.setDataBits(QSerialPort::Data8);
            uartPort.setParity(QSerialPort::NoParity);
            uartPort.setBaudRate(QSerialPort::Baud19200, QSerialPort::Directions());
            uartPort.setStopBits(QSerialPort::OneStop);
            uartPort.setFlowControl(QSerialPort::HardwareControl);
        }
        portIndex ++;
        qDebug() << "Serial Port name : " << port.portName();
        qDebug() << "Serial Port Manufacture name : " << port.manufacturer();
        qDebug() << "Serial Port Baud Rate : " << port.standardBaudRates();
    }

    if( uartPort.open(QIODevice::ReadWrite))
    {
        //const char uartDataToSend[3]={0x41,0x54,0x0D};
        const QByteArray uartDataToSend = "AT+GMI\r";
        qDebug() << "The serial Port is open";
        uartPort.flush();
        uartPort.write(uartDataToSend, uartDataToSend.size());
        uartPort.waitForBytesWritten(1000);
        while( uartPort.QSerialPort::bytesToWrite() != 0 );
        if (uartPort.error() == QSerialPort::NoError )
            qDebug() << "TX Data Succesfully Sent";
        else
            qDebug() << "TX Failed to Sent Data";
        a.exit(-1);
    }
    else
    {
        qDebug() << "Failed to open the Serial Port analyze the error below:" << uartPort.error();
        qDebug() << uartPort.error();
        a.exit();
    }

    while( uartPort.bytesAvailable() == 0 and (rxTimeout == false) )
    {
        qDebug() << "Starting reading RX received data";
        uartPort.waitForReadyRead(9000);
        uartPort.setReadBufferSize(300);
        QByteArray responseData = uartPort.readAll();
        qDebug() << "RX received : " << responseData.size() << "bytes";
        qDebug() << "This is the response: " << responseData.toHex();
        uartPort.close();
        rxTimeout=true;
    }

    qDebug() << "Program finish";
    return a.exec();
}
