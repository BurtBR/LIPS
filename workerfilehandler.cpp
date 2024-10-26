#include "workerfilehandler.h"
#include <QFileInfo>
#include <QDataStream>
#include <QTextStream>

volatile uint8_t* WorkerFileHandler::_defaultfile_map = nullptr;
QFile* WorkerFileHandler::_defaultfile = nullptr;

#define _MAP_SCALEWIDTH_VALUE (_defaultfile_map[0] << 24U | _defaultfile_map[1] << 16U | _defaultfile_map[2] << 8U | _defaultfile_map[3])
#define _MAP_LASERMIN_VALUE (_defaultfile_map[4] << 24U | _defaultfile_map[5] << 16U | _defaultfile_map[6] << 8U | _defaultfile_map[7])
#define _MAP_LASERMAX_VALUE (_defaultfile_map[8] << 24U | _defaultfile_map[9] << 16U | _defaultfile_map[10] << 8U | _defaultfile_map[11])
#define _MAP_CLOCKFREQ_VALUE (_defaultfile_map[12] << 24U | _defaultfile_map[13] << 16U | _defaultfile_map[14] << 8U | _defaultfile_map[15])
#define _MAP_SATURATION_VALUE _defaultfile_map[16]
#define _MAP_HASDEFAULTANCHORS_VALUE _defaultfile_map[17]

WorkerFileHandler::WorkerFileHandler(QObject *parent) : QObject{parent}{

}

WorkerFileHandler::~WorkerFileHandler(){
    UnmapDefaultFile();
}

bool WorkerFileHandler::MapDefaultFile(){
    if(!_defaultfile_map){
        try{
            _defaultfile = new QFile(_defaultfilename);
        }catch(...){
            emit Message("WorkerFileHandler: Unable to allocate memory for default values file");
            return false;
        }

        if(!_defaultfile->open(QIODevice::ReadWrite)){
            emit Message("WorkerFileHandler: Unable to open default values file");
            delete _defaultfile;
            _defaultfile = nullptr;
            return false;
        }

        _defaultfile_map = _defaultfile->map(0,18);

        _defaultfile->close();

        if(!_defaultfile_map){
            delete _defaultfile;
            _defaultfile = nullptr;
            return false;
        }
        return true;
    }
    return false;
}

bool WorkerFileHandler::UnmapDefaultFile(){
    if(_defaultfile_map){
        _defaultfile->unmap((uchar*)_defaultfile_map);
        delete _defaultfile;
        _defaultfile = nullptr;
        _defaultfile_map = nullptr;
        return true;
    }
    return false;
}

void WorkerFileHandler::GetDefaultValues(){
    if(_defaultfile_map)
        return;

    emit Message("WorkerFileHandler: Getting default values");

    if(!QFileInfo::exists(_defaultfilename)){
        if(!CreateDefaultFile()){
            emit Message("WorkerFileHandler: Unable to create default file");
            return;
        }
    }

    if(!MapDefaultFile()){
        emit Message("WorkerFileHandler: Unable to map default values file");
        return;
    }

    float doubleaux;
    uint32_t intaux = _MAP_CLOCKFREQ_VALUE;
    memcpy(&doubleaux, &intaux, 4);

    emit SetDefaultValues(_MAP_SATURATION_VALUE, _MAP_SCALEWIDTH_VALUE, _MAP_LASERMIN_VALUE, _MAP_LASERMAX_VALUE, doubleaux);

    if(_MAP_HASDEFAULTANCHORS_VALUE)
        OpenAnchorFile(GetDefaultAnchorFilename());
}

bool WorkerFileHandler::CreateDefaultFile(){
    QFile fp(_defaultfilename);

    if(!fp.open(QIODevice::WriteOnly))
        return false;

    QDataStream out(&fp);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    out << 700 << 5 << 20 << 5.0 << (uint8_t)230 << (uint8_t)0;

    fp.close();
    return true;
}

bool WorkerFileHandler::SetDefaultAnchorFilename(QString filename){
    QFile fp(_defaultfilename);

    if(!UnmapDefaultFile())
        return false;

    if(!fp.open(QIODevice::ReadWrite | QIODevice::Truncate))
        return false;

    if(!fp.resize(18))
        return false;

    if(!MapDefaultFile())
        return false;

    if(filename.isEmpty()){
        _defaultfile_map[17] = 0;
    }else{
        QTextStream out(&fp);

        out.seek(18);
        out << filename;

        _defaultfile_map[17] = 1;
    }

    fp.close();
    return true;
}

QString WorkerFileHandler::GetDefaultAnchorFilename(){
    QFile fp(_defaultfilename);

    if(!fp.open(QIODevice::ReadOnly | QIODevice::Text)){
        emit Message("WorkerFileHandler: Unable to open default anchors file");
        return QString();
    }

    QTextStream in(&fp);
    QString filename;

    in.seek(18);

    filename = in.readLine();

    fp.close();
    return filename;
}

void WorkerFileHandler::SetSaturation(uint8_t value){
    if(value == _MAP_SATURATION_VALUE)
        return;
    _defaultfile_map[16] = value;
}

void WorkerFileHandler::SetScaleWidth(uint32_t value){
    if(value == _MAP_SCALEWIDTH_VALUE)
        return;
    _defaultfile_map[0] = ((value >> 24) & 0xFF);
    _defaultfile_map[1] = ((value >> 16) & 0xFF);
    _defaultfile_map[2] = ((value >> 8) & 0xFF);
    _defaultfile_map[3] = (value & 0xFF);
}

void WorkerFileHandler::SetLaserMax(uint32_t value){
    if(value == _MAP_LASERMAX_VALUE)
        return;
    _defaultfile_map[8] = ((value >> 24) & 0xFF);
    _defaultfile_map[9] = ((value >> 16) & 0xFF);
    _defaultfile_map[10] = ((value >> 8) & 0xFF);
    _defaultfile_map[11] = (value & 0xFF);
}

void WorkerFileHandler::SetLaserMin(uint32_t value){
    if(value == _MAP_LASERMIN_VALUE)
        return;
    _defaultfile_map[4] = ((value >> 24) & 0xFF);
    _defaultfile_map[5] = ((value >> 16) & 0xFF);
    _defaultfile_map[6] = ((value >> 8) & 0xFF);
    _defaultfile_map[7] = (value & 0xFF);
}

void WorkerFileHandler::SetClock(float value){
    uint32_t auxvalue;
    memcpy(&auxvalue, &value ,4);
    if(auxvalue == _MAP_CLOCKFREQ_VALUE)
        return;
    _defaultfile_map[12] = ((auxvalue >> 24UL) & 0xFF);
    _defaultfile_map[13] = ((auxvalue >> 16UL) & 0xFF);
    _defaultfile_map[14] = ((auxvalue >> 8UL) & 0xFF);
    _defaultfile_map[15] = (auxvalue & 0xFF);
}

void WorkerFileHandler::OpenAnchorFile(QString filename){
    if(!QFileInfo::exists(filename))
        return;

    QFile fp(filename);
    QString line;
    QStringList strlist;
    QMatrix3x3 rmatrix;
    bool ok1, ok2, ok3;

    if(!fp.open(QIODevice::ReadOnly | QIODevice::Text)){
        emit Message("WorkerFileHandler: Unable to open anchor file");
        return;
    }

    QTextStream in(&fp);

    line = in.readLine();

    if(line.compare("Fx;Fy") != 0){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    line = in.readLine();
    strlist = line.split(';');

    if(strlist.size()!= 2){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    emit SetFx(strlist[0]);
    emit SetFy(strlist[1]);

    line = in.readLine();

    if(line.compare("Cx;Cy") != 0){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    line = in.readLine();
    strlist = line.split(';');

    if(strlist.size()!= 2){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    emit SetCx(strlist[0]);
    emit SetCy(strlist[1]);

    line = in.readLine();

    if(line.compare("K1;K2") != 0){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    line = in.readLine();
    strlist = line.split(';');

    if(strlist.size()!= 2){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    emit SetK1(strlist[0]);
    emit SetK2(strlist[1]);

    line = in.readLine();

    if(line.compare("P1;P2") != 0){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    line = in.readLine();
    strlist = line.split(';');

    if(strlist.size()!= 2){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    emit SetP1(strlist[0]);
    emit SetP2(strlist[1]);

    line = in.readLine();
    strlist = line.split(';');

    if(strlist.size()!= 4){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    rmatrix(0,0) = strlist[1].toFloat(&ok1);
    rmatrix(0,1) = strlist[2].toFloat(&ok2);
    rmatrix(0,2) = strlist[3].toFloat(&ok3);
    if(!ok1 || !ok2 || !ok3){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    line = in.readLine();
    strlist = line.split(';');

    if(strlist.size()!= 4){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    if(strlist[0].compare("R") != 0){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    rmatrix(1,0) = strlist[1].toFloat(&ok1);
    rmatrix(1,1) = strlist[2].toFloat(&ok2);
    rmatrix(1,2) = strlist[3].toFloat(&ok3);
    if(!ok1 || !ok2 || !ok3){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    line = in.readLine();
    strlist = line.split(';');

    if(strlist.size()!= 4){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    rmatrix(2,0) = strlist[1].toFloat(&ok1);
    rmatrix(2,1) = strlist[2].toFloat(&ok2);
    rmatrix(2,2) = strlist[3].toFloat(&ok3);
    if(!ok1 || !ok2 || !ok3){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    emit SetRmatrix(rmatrix);

    line = in.readLine();
    if(line.compare("Anchors") != 0){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    line = in.readLine();
    if(line.compare("Code;X;Y;Z") != 0){
        fp.close();
        emit Message("WorkerFileHandler: Corrupted anchor file");
        return;
    }

    while(!in.atEnd()){
        line = in.readLine();
        strlist = line.split(';');
        if(strlist.size()!= 4){
            fp.close();
            emit Message("WorkerFileHandler: Corrupted anchor file");
            return;
        }
        emit AppendAnchor(strlist[0], strlist[1].toFloat(&ok1), strlist[2].toFloat(&ok2), strlist[3].toFloat(&ok3));
        if(!ok1 || !ok2 || !ok3){
            fp.close();
            emit Message("WorkerFileHandler: Corrupted anchor file");
            return;
        }
    }

    fp.close();
}
