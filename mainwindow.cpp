#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QTableWidget>
#include <QStringList>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <direct.h>
#include <memory>
#include <fstream>
#include <string.h>
#include <QTimer>
#include <QDebug>
#include <cstdio>
#include <math.h>

using namespace std;

using namespace std;

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

MainWindow::MainWindow(detector *detr, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _detr = detr;
    _serial = new QSerialPort(this);
    ui->comboBox_3->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->comboBox_3->addItem(list.first(), list);
    }
    connect(_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    _command.clear();
//! [2]
    //connect(console, &Console::getData, this, &MainWindow::writeData);
    _timer=new QTimer(this);
    connect(_timer,SIGNAL(timeout()),this,SLOT(timeOutUpdate()));
    _commandStart=false;
}

void MainWindow::readData()
{
    QByteArray data = _serial->readAll();
    //console->putData(data);
    resolveData(data);
}

void MainWindow::resolveData(QByteArray &data)
{
    int data_size=data.size();
    //bool commandStart=false;

    for(int i=0;i<data_size;i++)
    {

        if(data[i] == '\\')
        {
            _commandStart=true;
            continue;
        }

        else if(_commandStart && data[i] == '/')
        {
            resolveCommand();
            _command.clear();
            _commandStart=false;
        }
        if(_commandStart)
            _command+=data[i];
        //else if(!_command.isEmpty())
            // _command+=data[i];
    }

}

void MainWindow::resolveCommand()
{
    if(_command.isEmpty())
        QMessageBox::critical(this, tr("Error"),"空命令" );
    if(_command == "a1")
        on_measure_clicked();
    else if(_command == "a2")
        on_record_clicked();
    else if(_command == "a3")
        on_adjust_clicked();
    else if(_command =="b1")
        on_pushButton_clicked();
    else if(_command =="b2")
        on_pushButton_2_clicked();
    else if(_command =="b3")
        on_pushButton_3_clicked();
    else if(_command =="b4")
        on_pushButton_4_clicked();
    else if(_command =="b5")
        on_pushButton_5_clicked();
    else if(_command[0]=='c')
    {
        QString id = _command.mid(2);
        if(_command[1]=='1')
        {
            ui->lineEdit->setText(id);
        }
        else if(_command[1]=='2')
        {
            ui->lineEdit_2->setText(id);
        }
        else if(_command[1]=='3')
        {
            ui->comboBox->setCurrentIndex(id.toInt());
        }
        else if(_command[1]=='4')
        {
            ui->comboBox_2->setCurrentIndex(id.toInt());
        }
        else if(_command[1]=='5')
        {
            ui->lineEdit_3->setText(id);
        }
        else if(_command[1]=='6')
            on_pushButton_6_clicked();
        else if(_command[1]=='7')
            on_pushButton_7_clicked();
    }
    else if(_command[0]=='d')
    {
        if(_command[1]=='1')
        {
            double value=_command.mid(2).toDouble();
            _detr->setAForce(value);
            ui->progressBar->setValue(static_cast<int>(value));
            ui->label_13->setText(_command.mid(2));
        }
        else if(_command[1]=='2')
        {
            double value=_command.mid(2).toDouble();
            _detr->setTorque(value);
            ui->progressBar_2->setValue(static_cast<int>(value));
            ui->label_14->setText(_command.mid(2));
        }
        else if(_command=="d3")
        {
            on_pushButton_8_clicked();
        }
        else if(_command=="d4")
        {
            if(_detr->getTestStart())
            {
                _detr->setTestStart(false);
                ui->pushButton_8->show();
            }
        }
        else if(_command=="d5")
        {
            ui->progressBar->setValue(0);
            ui->label_13->setText("0.0");
            ui->progressBar_2->setValue(0);
            ui->label_14->setText("0.0");
        }
        else if(_command == "d6")
        {
            ui->stackedWidget->setCurrentIndex(0);
            _detr->clearTest();
        }
        else if(_command[1] == '7')
        {
            QString sample = _command.mid(2);
            QStringList sampleList = sample.split('|');
            if(sampleList.size()!=2)
            {
                QMessageBox::critical(this, tr("Error"),_command );
                return;
            }
            QString aforce_str = sampleList[0];
            QString torque_str = sampleList[1];
            double aforce=aforce_str.toDouble();
            double torque = torque_str.toDouble();
            ui->label_13->setText(aforce_str);
            ui->label_14->setText(torque_str);
            ui->progressBar->setValue(static_cast<int>(aforce));
            ui->progressBar_2->setValue(static_cast<int>(torque));
            int measureNum = _detr->getMeasureNum();
            _detr->setAForce(aforce);
            _detr->_torque=torque;
            _detr->calRatio();
            _detr->addRecord(aforce,torque,_detr->_ratio);

            double totalRatio=0;
            for(int i=0;i<_detr->_record.size();i++)
            {
                measure_record* tempRecord = _detr->_record[i];
                totalRatio += tempRecord->_RATIO;
            }
            _detr->_everage = totalRatio/_detr->_record.size();

            double totalRatioSqr =0;
            for(int i=0;i<_detr->_record.size();i++)
            {
                measure_record* tempRecord = _detr->_record[i];
                double offset = (tempRecord->_RATIO - _detr->_everage);
                totalRatioSqr += offset * offset;
            }
            _detr->_sDeviation = sqrt(totalRatioSqr/_detr->_record.size());
            //QTableWidgetItem *newItem = new QTableWidgetItem(aforce_str);
            //ui->tableWidget->

            char everage_str[20];
            sprintf(everage_str,"%f",_detr->_everage);
            char sdevi_str[20];
            sprintf(sdevi_str,"%f", _detr->_sDeviation);

            ui->label_21->setText(QString(everage_str));
            ui->label_22->setText(QString(sdevi_str));
            char ratio_str[20];
            sprintf(ratio_str,"%f",_detr->_ratio);
            char number_str[20];
            sprintf(number_str,"%d", _detr->_record.size());
            ui->tableWidget->setItem(measureNum+1,0,new QTableWidgetItem(QString(number_str)));
            ui->tableWidget->setItem(measureNum+1,1,new QTableWidgetItem(aforce_str));
            ui->tableWidget->setItem(measureNum+1,2,new QTableWidgetItem(torque_str));
            ui->tableWidget->setItem(measureNum+1,3,new QTableWidgetItem(QString(ratio_str)));
            _timer->stop();
            int size = _detr->_recordCurve.size();
            if(size>0)
            {
                vector<measure_record* > & tempRecord = _detr->_recordCurve[size-1];
                tempRecord.push_back(new measure_record(aforce,torque));
            }

        }
        else if(_command == "d8")
            on_pushButton_12_clicked();
    }
    else
        QMessageBox::critical(this, tr("Error"),_command );
}

void MainWindow::openSerialPort()
{
    _serial->setPortName(ui->comboBox_3->currentText());
    _serial->setBaudRate(QSerialPort::Baud9600);
    _serial->setDataBits(QSerialPort::Data8);
    _serial->setParity(QSerialPort::NoParity);
    _serial->setStopBits(QSerialPort::OneStop);
    _serial->setFlowControl(QSerialPort::NoFlowControl);
    if (_serial->open(QIODevice::ReadWrite)) {

        _portConnected = true;
        ui->label_12->clear();
        ui->label_11->setText("串口连接成功！");
        //ui->actionConnect->setEnabled(false);
       // ui->actionDisconnect->setEnabled(true);
        //ui->actionConfigure->setEnabled(false);

    } else {
        QMessageBox::critical(this, tr("Error"), _serial->errorString());

        //showStatusMessage(tr("Open error"));
    }

}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (_serial->isOpen())
    {
        _serial->close();
        _portConnected=false;
    }
    //console->setEnabled(false);
    //ui->actionConnect->setEnabled(true);
   // ui->actionDisconnect->setEnabled(false);
    //ui->actionConfigure->setEnabled(true);
    //showStatusMessage(tr("Disconnected"));
    ui->label_11->setText("连接断开");
}
//! [5]


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_measure_clicked()
{
    if(!_portConnected)
    {
        ui->label_12->setText("请先连接串口！");
        return;
    }
    _serial->write("\\a1/",4);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_clicked()
{
    ui->label_15->setText("高强度大六角头螺栓--测试信息");
    _detr->setDetectType(detector::bigSizeEdge);
    ui->stackedWidget->setCurrentIndex(2);
    if(_serial->isOpen())
       _serial->write("\\b1/",4);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->label_15->setText("扭剪型螺栓--测试信息");
    _detr->setDetectType(detector::torsionShear);
    ui->stackedWidget->setCurrentIndex(2);
    if(_serial->isOpen())
       _serial->write("\\b2/",4);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->label_15->setText("特殊螺栓--测试信息");
    _detr->setDetectType(detector::special);
    ui->stackedWidget->setCurrentIndex(2);
    if(_serial->isOpen())
        _serial->write("\\b3/",4);
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    if(_serial->isOpen())
       _serial->write("\\c6/",4);
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    if(_serial->isOpen())
       _serial->write("\\b5/",4);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    QString delId=static_cast<QString>(ui->lineEdit->text());
    QString testId=static_cast<QString>(ui->lineEdit_2->text());
    int screwSize=detector::screwSize(ui->comboBox->currentIndex());
    int capRank=detector::cap_rank(ui->comboBox_2->currentIndex());
    _detr->setDetectProperties(delId,testId,screwSize,capRank);
    _detr->_range = ui->lineEdit_3->text().toInt();
    if(_serial->isOpen())
    {
        _serial->write("\\c7/");
    }
}

void MainWindow::on_pushButton_11_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->label_18->hide();
    ui->pushButton_8->show();
    ui->label_18->clear();
    if(_detr->_testStart)
        _timer->stop();
    _detr->clearTest();
    if(_serial->isOpen())
        _serial->write("\\d6/");


}

void MainWindow::on_serial_port_con_clicked()
{
    openSerialPort();
}

void MainWindow::on_record_clicked()
{
    if(!_portConnected)
    {
        ui->label_12->setText("请先连接串口！");
        return;
    }
    _serial->write("\\a2/",4);
}

void MainWindow::on_adjust_clicked()
{
    if(!_portConnected)
    {
        ui->label_12->setText("请先连接串口！");
        return;
    }
    _serial->write("\\a3/",4);
}

void MainWindow::on_pushButton_4_clicked()
{
    _detr->setDetectType(detector::manual);
    if(_serial->isOpen())
       _serial->write("\\b4/",4);
}

void MainWindow::on_pushButton_8_clicked()
{
    ui->pushButton_8->hide();
    _detr->_testStart=true;
    if(_serial->isOpen())
    {
        _serial->write("\\d3/");
        _timer->start(100);
        vector<measure_record*> temp;
        _detr->_recordCurve.push_back(temp);
    }

}

void MainWindow::on_pushButton_12_clicked()
{
    //ui->pushButton_12->hide();
    if(_detr->getMeasureNum()==0)
    {
        QMessageBox::information(this,"保存提示信息","未产生测量数据，请先开始测量");
        return;
    }
    else
    {
        if(_detr->_saved)
        {
            QMessageBox::warning(this,"保存提示信息","本次测试已保存");
            return;
        }
        int nPos = _detr->_path.rfind('\\');
        string dirPath;
        if(nPos!= -1)
            dirPath=_detr->_path.substr(0,nPos);
        else
        {
            QMessageBox::warning(this,"保存提示信息","文件地址错误");
            return;
        }
        char TempDir[1000];
        sprintf(TempDir,dirPath.c_str());
        strcat(TempDir,"\\TestResult");
        char FilePath[1000];
        sprintf(FilePath,TempDir);
        strcat(FilePath,_detr->_delegate_id.toLatin1().data());
        strcat(FilePath,".txt");
        ofstream fout(FilePath);
        /*
        char Dir[1000];
        GetCurrentDirectory(1000,(LPWSTR)Dir);  //得到当前工作路径
        //FILE *fp = NULL;
        char TempDir[200];
        memset(TempDir,'\0',sizeof(TempDir));

        strcat(Dir,"\\TestResult");
        sprintf(TempDir,(Dir));
        strcat(TempDir,"\\");
        strcat(TempDir,_detr->_delegate_id.toLatin1().data());
        strcat(TempDir,".txt");
        //fp = fopen(TempDir,"w");
        ofstream fout(TempDir);*/
        if (!fout)
        {
            if(_mkdir(TempDir)==0)
            {
                //return 1;//文件夹创建成功
                fout.open(FilePath);
            }
            else
            {
                QMessageBox::warning(this,"ERROR","数据保存失败");
                return;
            }
        }
        fout<<"类型： "<<_detr->detectTypeVec[_detr->_detectType]<<endl;
        fout<<"委托编号： "<<_detr->_delegate_id.toStdString()<<endl;
        fout<<"实验编号： "<<_detr->_test_id.toStdString()<<endl;
        fout<<"规格： "<<_detr->screwSizeVec[_detr->_screw_size]<<endl;
        fout<<"性能等级"<<_detr->capRankVec[_detr->_cap_rank]<<endl;
        for(int i=0;i<_detr->getMeasureNum();i++)
        {
            measure_record *oneSample = _detr->_record[i];
            fout<<oneSample->_AFORCE<<'\t'<<oneSample->_TORQUE<<'\t'<<oneSample->_RATIO<<endl;
        }
        fout<<"均值： "<<_detr->_everage<<'\t'<<"标准差"<<_detr->_sDeviation;
        fout<<endl<<endl<<"测试图像数据："<<endl;
        for(unsigned i=0;i<_detr->getMeasureNum();i++)
        {
            fout<<"测试数据"<<i+1<<endl;
            vector<measure_record*> tempRecord = _detr->_recordCurve[i];
            for(unsigned j = 0;j<tempRecord.size();j++)
            {
                measure_record *oneSample = tempRecord[j];
                fout<<oneSample->_AFORCE<<'\t'<<oneSample->_TORQUE<<'\t'<<oneSample->_RATIO<<endl;
            }
        }
        fout.close();
        _detr->_saved=true;
        ui->label_18->show();
        ui->label_18->setText("保存成功！");
    }
    if(_serial->isOpen())
        _serial->write("\\d8/");
}

void MainWindow::on_pushButton_9_clicked()
{
    ui->pushButton_12->shortcut();
    _detr->_testStart=false;
    if(_serial->isOpen())
        _serial->write("\\d4/");

}

void MainWindow::on_pushButton_10_clicked()
{
    _detr->_aforce=0;
    _detr->_torque=0;
    ui->label_13->setText("0");
    ui->label_14->setText("0");
    ui->progressBar->setValue(0);
    ui->progressBar_2->setValue(0);
    if(_serial->isOpen())
        _serial->write("\\d5/");

}

void MainWindow::on_lineEdit_editingFinished()
{
    _detr->_delegate_id = ui->lineEdit->text();
    if(_serial->isOpen())
    {
        char temp[100];
        sprintf(temp,"\\c1%s/",_detr->_delegate_id.toLatin1().data());
        _serial->write(temp);
    }
}

void MainWindow::on_lineEdit_2_editingFinished()
{
    _detr->_test_id = ui->lineEdit_2->text();
    if(_serial->isOpen())
    {
        char temp[100];
        sprintf(temp,"\\c2%s/",_detr->_test_id.toLatin1().data());
        _serial->write(temp);
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(_serial->isOpen())
    {
        char temp[100];
        sprintf(temp,"\\c3%d/",index);
        _serial->write(temp);
    }
}

void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    if(_serial->isOpen())
    {
        char temp[100];
        sprintf(temp,"\\c4%d/",index);
        _serial->write(temp);
    }
}



void MainWindow::on_lineEdit_3_editingFinished()
{
    _detr->_range = ui->lineEdit_3->text().toInt();
    if(_serial->isOpen())
    {
        char temp[100];
        sprintf(temp,"\\c5%s/",ui->lineEdit_3->text().toLatin1().data());
        _serial->write(temp);
    }
}

void MainWindow::on_pushButton_14_clicked()
{
    if(_serial->isOpen())
        _serial->write("\\d#/");
    ui->tableWidget->clear();
    for(int i=0;i<_detr->_record.size();i++)
    {
        delete _detr->_record[i];
    }
}

void MainWindow::on_pushButton_13_clicked()
{
    if(_serial->isOpen())
        _serial->write("\\d9/");

}

void MainWindow::timeOutUpdate()
{
    qDebug()<<"timeout\n";
    int tempSize = _detr->_recordCurve.size();
    measure_record *oneSample = new measure_record(_detr->_aforce,_detr->_torque);
    (_detr->_recordCurve[tempSize-1]).push_back(oneSample);
    //vector<measure_record*> & tempRecord = _detr->_recordCurve[tempSize-1];
    //tempRecord.push_back(new measure_record(_detr->_aforce,_detr->_torque));
}
