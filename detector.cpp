#include "detector.h"
#include <QMessageBox>

void detector::clearTest()
{
    _detectType=0;
    _delegate_id="11111111";
    _test_id="11111111";
    _screw_size=M12;
    _cap_rank=n1;
    _aforce=0;
    _torque=0;
    _testStart=false;
    _saved=false;
    _record.clear();
    _recordCurve.clear();
}

void detector::addRecord(double aforce, double torque, double ratio)
{
    _record.push_back(new measure_record(aforce,torque,ratio));
}

void detector::calRatio()
{
    if(_cap_rank == n1 )
    {
        switch(_screw_size)
        {
        case M12:
            _ratio = _torque / _aforce /60;
            break;
        case M16:
            _ratio = _torque / _aforce /110;
            break;
        case M20:
            _ratio = _torque / _aforce /170;
            break;
        case M22:
            _ratio = _torque / _aforce /210;
            break;
        case M24:
            _ratio = _torque / _aforce /250;
            break;
        case M27:
            _ratio = _torque / _aforce /320;
            break;
        case M30:
            _ratio = _torque / _aforce /390;
            break;
        default:
            //QMessageBox::critical(this, "Error","No such screw size!" );
            break;
        }
    }
    else if(_cap_rank == n2)
    {
        switch(_screw_size)
        {
        case M12:
            _ratio = _torque / _aforce /50;
            break;
        case M16:
            _ratio = _torque / _aforce /90;
            break;
        case M20:
            _ratio = _torque / _aforce /140;
            break;
        case M22:
            _ratio = _torque / _aforce /165.5;
            break;
        case M24:
            _ratio = _torque / _aforce /195.5;
            break;
        case M27:
            _ratio = _torque / _aforce /255.5;
            break;
        case M30:
            _ratio = _torque / _aforce /310;
            break;
        default:
            //QMessageBox::critical(this, "Error","No such screw size!" );
            break;
        }

    }
    else
    {
        //QMessageBox::critical(this, "Error","Cap_rank is set wrong!" );
    }
}
