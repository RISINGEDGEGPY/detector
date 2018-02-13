#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QLabel>

#include <QIntValidator>
#include <QLineEdit>
#include "detector.h"
QT_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(detector *detr,QWidget *parent = 0);
    ~MainWindow();

    void openSerialPort();
    void closeSerialPort();
    void readData();
    void resolveData(QByteArray &data);
    void resolveCommand();

private slots:
    void on_measure_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_11_clicked();

    void on_serial_port_con_clicked();

    void on_record_clicked();

    void on_adjust_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_8_clicked();


    void on_pushButton_12_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_lineEdit_editingFinished();

    void on_lineEdit_2_editingFinished();

    void on_comboBox_currentIndexChanged(int index);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_lineEdit_3_editingFinished();

    void on_pushButton_14_clicked();

    void on_pushButton_13_clicked();

    void timeOutUpdate();

private:
    QTimer *_timer;
    Ui::MainWindow *ui;
    detector *_detr;
    QSerialPort *_serial;
    QString _command;
    bool _portConnected;
    bool _commandStart;
};

#endif // MAINWINDOW_H
