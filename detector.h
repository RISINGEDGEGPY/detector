#ifndef DETECTOR_H
#define DETECTOR_H

#include <QString>
#include <vector>
using namespace std;

struct measure_record{
    measure_record(double aforce,double torque, double ratio=0):
        _AFORCE(aforce),_TORQUE(torque),_RATIO(ratio){}
    double _AFORCE;
    double _TORQUE;
    double _RATIO;
};

class detector
{
public:
    enum detectType{
        bigSizeEdge=0,
        torsionShear=1,
        special=2,
        manual=3,
    };

    enum screwSize{
        M12=0,
        M16=1,
        M20=2,
        M22=3,
        M24=4,
        M27=5,
        M30=6
    };

    enum cap_rank{
        n1=0,
        n2=1
    };

    vector<string > detectTypeVec = {"大六角型","扭剪型","特殊规格","手动"};
    vector<string > screwSizeVec = {"M12","M16","M20","M22","M24","M27","M30"};
    vector<string > capRankVec = {"10.9s","8.8s"};

    detector(string path):_detectType(0),_delegate_id("11111111"),_test_id("11111111")
            ,_screw_size(M12),_cap_rank(n1),_range(100),_aforce(0),_torque(0),_everage(0)
            ,_sDeviation(0),_testStart(false),_path(path),_saved(false),_ratio(0){}
    void setDetectType(int type){_detectType=type;}
    void setDetectProperties(QString delId,QString testId,int screwSize,int capRank)
    {_delegate_id=delId;_test_id=testId;_screw_size=screwSize;_cap_rank=capRank;}

    void setType(int type) {_detectType = type;}
    void setAForce(double aforce){_aforce = aforce;}
    void setTorque(double torque){_torque = torque;}
    void setTestStart(bool testStart){_testStart = testStart;}
    bool getTestStart() {return _testStart;}
    void clearTest() ;
    void addRecord(double aforce, double torque, double ratio) ;
    int getMeasureNum(){return _record.size();}
    //char* getDelegateId(){return static_cast<char*>(_delegate_id.data());}
    int getDetectType() {return _detectType;}
    QString getTestId(){return _test_id;}
    int getScrewSize(){return _screw_size;}
    int getCapRank(){return _cap_rank;}
    void calRatio();
//private:
    int _detectType;
    QString _delegate_id;
    QString _test_id;
    int _screw_size;
    int _cap_rank;
    int _range;
    double _aforce;
    double _torque;
    double _ratio;
    double _everage;
    double _sDeviation;
    bool _testStart;
    vector<measure_record*> _record;
    vector<vector<measure_record*>> _recordCurve;
    string _path;
    bool _saved;
};

#endif // DETECTOR_H
