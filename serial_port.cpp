#include "serial_port.h"
#include "ui_serial_port.h"

serial_port::serial_port(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::serial_port)
{
    ui->setupUi(this);
}

serial_port::~serial_port()
{
    delete ui;
}
