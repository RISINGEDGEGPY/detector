#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <QDialog>

namespace Ui {
class serial_port;
}

class serial_port : public QDialog
{
    Q_OBJECT

public:
    explicit serial_port(QWidget *parent = 0);
    ~serial_port();

private:
    Ui::serial_port *ui;
};

#endif // SERIAL_PORT_H
