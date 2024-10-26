#include "workerfilehandler.h"
#include <QFileInfo>
#include <QDataStream>

volatile uint8_t* WorkerFileHandler::_defaultfile_map = nullptr;
QFile* WorkerFileHandler::_defaultfile = nullptr;

#define _MAP_SCALEWIDTH_VALUE (_defaultfile_map[0] << 24U | _defaultfile_map[1] << 16U | _defaultfile_map[2] << 8U | _defaultfile_map[3])
#define _MAP_LASERMIN_VALUE (_defaultfile_map[4] << 24U | _defaultfile_map[5] << 16U | _defaultfile_map[6] << 8U | _defaultfile_map[7])
#define _MAP_LASERMAX_VALUE (_defaultfile_map[8] << 24U | _defaultfile_map[9] << 16U | _defaultfile_map[10] << 8U | _defaultfile_map[11])
#define _MAP_CLOCKFREQ_VALUE ((uint64_t)_defaultfile_map[12] << 56UL | (uint64_t)_defaultfile_map[13] << 48UL | (uint64_t)_defaultfile_map[14] << 40UL | (uint64_t)_defaultfile_map[15] << 32UL | (uint64_t)_defaultfile_map[16] << 24UL | (uint64_t)_defaultfile_map[17] << 16UL | (uint64_t)_defaultfile_map[18] << 8UL | (uint64_t)_defaultfile_map[19])
#define _MAP_SATURATION_VALUE (uint8_t)_defaultfile_map[20]
#define _MAP_HASDEFAULTANCHORS_VALUE (uint8_t)_defaultfile_map[21]

WorkerFileHandler::WorkerFileHandler(QObject *parent) : QObject{parent}{

}

WorkerFileHandler::~WorkerFileHandler(){
    if(_defaultfile_map){
        _defaultfile->unmap((uchar*)_defaultfile_map);
        delete _defaultfile;
        _defaultfile = nullptr;
        _defaultfile_map = nullptr;
    }
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

    try{
        _defaultfile = new QFile(_defaultfilename);
    }catch(...){
        emit Message("WorkerFileHandler: Unable to allocate memory for default values file");
        return;
    }

    if(!_defaultfile->open(QIODevice::ReadWrite)){
        emit Message("WorkerFileHandler: Unable to open default values file");
        delete _defaultfile;
        _defaultfile = nullptr;
        return;
    }

    _defaultfile_map = _defaultfile->map(0,22);

    _defaultfile->close();

    if(!_defaultfile_map){
        delete _defaultfile;
        _defaultfile = nullptr;
        emit Message("WorkerFileHandler: Unable to map default values file");
        return;
    }

    double doubleaux;
    uint64_t intaux = _MAP_CLOCKFREQ_VALUE;
    memcpy(&doubleaux, &intaux, 8);

    _defaultanchorfile = QString(); // CHANGE HERE TO LOAD FILE

    emit SetDefaultValues(_MAP_SATURATION_VALUE, _MAP_SCALEWIDTH_VALUE, _MAP_LASERMIN_VALUE, _MAP_LASERMAX_VALUE, doubleaux);
}

bool WorkerFileHandler::CreateDefaultFile(){
    QFile fp(_defaultfilename);

    if(!fp.open(QIODevice::WriteOnly))
        return false;

    QDataStream out(&fp);

    _defaultanchorfile.clear();

    out << 700 << 5 << 20 << 5.0 << (uint8_t)230 << (uint8_t)0;

    fp.close();
    return true;
}

void WorkerFileHandler::SetSaturation(uint8_t value){
    if(value == _MAP_SATURATION_VALUE)
        return;
    _defaultfile_map[20] = (value & 0xFF);
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

void WorkerFileHandler::SetClock(double value){
    uint64_t auxvalue;
    memcpy(&auxvalue, &value ,8);
    if(auxvalue == _MAP_CLOCKFREQ_VALUE)
        return;
    _defaultfile_map[12] = ((auxvalue >> 56UL) & 0xFF);
    _defaultfile_map[13] = ((auxvalue >> 48UL) & 0xFF);
    _defaultfile_map[14] = ((auxvalue >> 40UL) & 0xFF);
    _defaultfile_map[15] = ((auxvalue >> 32UL) & 0xFF);
    _defaultfile_map[16] = ((auxvalue >> 24UL) & 0xFF);
    _defaultfile_map[17] = ((auxvalue >> 16UL) & 0xFF);
    _defaultfile_map[18] = ((auxvalue >> 8UL) & 0xFF);
    _defaultfile_map[19] = (auxvalue & 0xFF);
}
