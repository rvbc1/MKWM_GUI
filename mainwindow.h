#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSerialPort *serial;

    QTimer *updateing_angle_timer;
    QTimer *tracking_timer;
    QTimer *recieve_timer;

    void addAvaibleSerials();
    void prepareSerial();

    void prepareData();
    void updateData();

    void enableDataWidgets();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendData();

    void serialReceived();

    void tracking();

    void on_pushButton_pressed();

    void on_pushButton_2_pressed();

    void on_pushButton_3_pressed();

    void on_spinBox_valueChanged(int arg1);

    void on_horizontalSlider_valueChanged(int value);

    void on_spinBox_2_valueChanged(int arg1);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_spinBox_3_valueChanged(int arg1);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_checkBox_pressed();

    void on_checkBox_stateChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_horizontalSlider_4_valueChanged(int value);

    void on_pushButton_4_clicked();

    void on_checkBox_2_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
