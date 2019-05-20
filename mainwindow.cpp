#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qbytearray.h>
#include <qdebug.h>
#include <qvector.h>
#include <qlist.h>

#define DATA_FRAME_TX_SIZE 10
#define DATA_FRAME_RX_SIZE 10

#define START_CODE 0x40
#define END_CODE 0x80

#define TRACKING_INTERVAL 50

struct dataFrameTX{
    uint8_t start_code;
    uint16_t servo1;
    uint16_t servo2;
    uint16_t servo3;
    uint16_t servo4;
    uint8_t end_code;
} __attribute__ ((__packed__));

struct dataFrameRX{
    uint8_t start_code;
    uint16_t servo1;
    uint16_t servo2;
    uint16_t servo3;
    uint16_t servo4;
    uint8_t end_code;
} __attribute__ ((__packed__));

union{
    dataFrameTX data;
    char bytes[DATA_FRAME_TX_SIZE];
}frameTX;


union{
    dataFrameRX data;
    char bytes[DATA_FRAME_RX_SIZE];
}frameRX;

QVector<dataFrameTX> datas;
int counter;

void MainWindow::addAvaibleSerials(){
    QList<QSerialPortInfo> lista = QSerialPortInfo::availablePorts();

    for (int i = 0; i < lista.size(); i++){
        ui->comboBox->addItem(lista[i].portName());
    }
}

void MainWindow::prepareSerial(){
    addAvaibleSerials();
    serial = new QSerialPort();
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
}

void MainWindow::prepareData(){
    frameTX.data.start_code = START_CODE;

    frameTX.data.end_code = END_CODE;
}

void MainWindow::updateData(){
    frameTX.data.servo1 = static_cast <uint16_t>(ui->spinBox->value());
    frameTX.data.servo2 = static_cast <uint16_t>(ui->spinBox_2->value());
    frameTX.data.servo3 = static_cast <uint16_t>(ui->spinBox_3->value());
    frameTX.data.servo4 = static_cast <uint16_t>(ui->spinBox_4->value());
}

void MainWindow::enableDataWidgets(){
    ui->checkBox->setEnabled(true);
    ui->checkBox_2->setEnabled(true);

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);

    ui->horizontalSlider->setEnabled(true);
    ui->horizontalSlider_2->setEnabled(true);
    ui->horizontalSlider_3->setEnabled(true);
    ui->horizontalSlider_4->setEnabled(true);

    ui->spinBox->setEnabled(true);
    ui->spinBox_2->setEnabled(true);
    ui->spinBox_3->setEnabled(true);
    ui->spinBox_4->setEnabled(true);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    prepareSerial();

    prepareData();

    updateing_angle_timer = new QTimer(this);
    updateing_angle_timer->setInterval(TRACKING_INTERVAL);
    connect(updateing_angle_timer, SIGNAL(timeout()), this, SLOT(sendData()));

    tracking_timer = new QTimer(this);
    tracking_timer->setInterval(TRACKING_INTERVAL);
    connect(tracking_timer, SIGNAL(timeout()), this, SLOT(tracking()));

    recieve_timer = new QTimer(this);
    recieve_timer->setInterval(TRACKING_INTERVAL);
    connect(recieve_timer, SIGNAL(timeout()), this, SLOT(serialReceived()));
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::serialReceived(){
    QByteArray ba = serial->readAll();
    for(int i =0; i < ba.size() - DATA_FRAME_RX_SIZE; i++){
        if((uint8_t(ba[i]) == 0x40) && ((uint8_t(ba[i + DATA_FRAME_RX_SIZE - 1])) == 0x80)){
            for(int j = 0; j < DATA_FRAME_RX_SIZE; j++){
                frameRX.bytes[j] = ba[i + j];
            }
            //ui->spinBox->setValue(frameRX.data.servo1);
            ui->label_5->setText(QString("Current_Value: ").append(QString::number(frameRX.data.servo1)));
            ui->label_6->setText(QString("Current_Value: ").append(QString::number(frameRX.data.servo2)));
            ui->label_7->setText(QString("Current_Value: ").append(QString::number(frameRX.data.servo3)));
            ui->label_8->setText(QString("Current_Value: ").append(QString::number(frameRX.data.servo4)));
            break;
        }
        //qDebug() << i << ": " << QString::number(uint8_t(ba[i]), 16);
    }
    //        receivedData.append(serial->read(RX_FRAME_BYTES));

    //        if(((uint8_t)(receivedData[receivedData.size()-1]) == 0x80) && (receivedData.size() >= 4)){
    //            if((receivedData.size() == 4) && ((uint8_t)(receivedData[0]) == 0x40)){

    //                for (int i = 0; i < receivedData.size(); i++) {
    //                    frameRX.bytesRX[i] = (uint8_t) receivedData[i];
    //                    //qDebug() << QString::number((uint8_t)datax[i], 10);
    //                }

    //            } else qDebug() << "Error!!!";

    //            receivedData.clear();
    //        }
}

void MainWindow::tracking(){
    ui->spinBox->setValue(frameRX.data.servo1);
    ui->spinBox_2->setValue(frameRX.data.servo2);
    ui->spinBox_3->setValue(frameRX.data.servo3);
    ui->spinBox_4->setValue(frameRX.data.servo4);
}

void MainWindow::sendData(){
    updateData();
    if(serial->isOpen())
        serial->write(frameTX.bytes, DATA_FRAME_TX_SIZE);

    //    if(counter < datas.size()){
    //        frameTX.data = datas[counter];
    //        serial->write(frameTX.bytes, DATA_FRAME_TX_SIZE);
    //        counter++;
    //    }
}


void MainWindow::on_pushButton_pressed(){
    sendData();
}

void MainWindow::on_pushButton_2_pressed(){
    tracking();
}

void MainWindow::on_pushButton_3_pressed(){
    //counter = 0;
    //serial->write("1");
}

void MainWindow::on_pushButton_4_clicked()
{
    serial->close();
    if(ui->comboBox->count() > 0){
        serial->setPortName(ui->comboBox->currentText());
        serial->open(QIODevice::ReadWrite);
        enableDataWidgets();
        ui->progressBar->setTextVisible(true);
        ui->progressBar->setValue(100);
        recieve_timer->start();
    } else {

    }
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    ui->horizontalSlider->setValue(ui->spinBox->value());
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->spinBox->setValue(ui->horizontalSlider->value());
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    ui->horizontalSlider_2->setValue(ui->spinBox_2->value());
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    ui->spinBox_2->setValue(ui->horizontalSlider_2->value());
}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    ui->horizontalSlider_3->setValue(ui->spinBox_3->value());
}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    ui->spinBox_3->setValue(ui->horizontalSlider_3->value());
}

void MainWindow::on_spinBox_4_valueChanged(int arg1)
{
    ui->horizontalSlider_4->setValue(ui->spinBox_4->value());
}

void MainWindow::on_horizontalSlider_4_valueChanged(int value)
{
    ui->spinBox_4->setValue(ui->horizontalSlider_4->value());
}

void MainWindow::on_checkBox_pressed()
{

}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(ui->checkBox->checkState()){
        updateing_angle_timer->start();
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->checkBox_2->setEnabled(false);
    }
    else {
        updateing_angle_timer->stop();
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->checkBox_2->setEnabled(true);
    }
}

void MainWindow::on_checkBox_2_stateChanged(int arg1){
    if(ui->checkBox_2->checkState()){
        tracking_timer->start();
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->checkBox->setEnabled(false);
    }
    else {
        tracking_timer->stop();
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->checkBox->setEnabled(true);
    }
}




