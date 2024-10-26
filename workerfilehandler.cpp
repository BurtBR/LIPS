#include "workerfilehandler.h"
#include <QFileInfo>
#include <QDataStream>

volatile uint8_t* WorkerFileHandler::_defaultfile_map = nullptr;
QFile* WorkerFileHandler::_defaultfile = nullptr;

#define _MAP_SCALEWIDTH_VALUE (_defaultfile_map[0] << 24U | _defaultfile_map[1] << 16U | _defaultfile_map[2] << 8U | _defaultfile_map[3])
#define _MAP_LASERMIN_VALUE (_defaultfile_map[4] << 24U | _defaultfile_map[5] << 16U | _defaultfile_map[6] << 8U | _defaultfile_map[7])
#define _MAP_LASERMAX_VALUE (_defaultfile_map[8] << 24U | _defaultfile_map[9] << 16U | _defaultfile_map[10] << 8U | _defaultfile_map[11])
#define _MAP_CLOCKFREQ_VALUE (double)((uint64_t)_defaultfile_map[12] << 56ULL | (uint64_t)_defaultfile_map[13] << 48ULL | (uint64_t)_defaultfile_map[14] << 40ULL | (uint64_t)_defaultfile_map[15] << 32ULL | (uint64_t)_defaultfile_map[16] << 24ULL | (uint64_t)_defaultfile_map[17] << 16ULL | (uint64_t)_defaultfile_map[18] << 8ULL | (uint64_t)_defaultfile_map[19])
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

    _saturation = _MAP_SATURATION_VALUE;
    _scalewidth = _MAP_SCALEWIDTH_VALUE;
    _lasermin = _MAP_LASERMIN_VALUE;
    _lasermax = _MAP_LASERMAX_VALUE;
    _clockfreq = _MAP_CLOCKFREQ_VALUE;
    _defaultanchorfile = QString(); // CHANGE HERE TO LOAD FILE

    emit SetDefaultValues(_saturation, _scalewidth, _lasermin, _lasermax, _clockfreq);
}

bool WorkerFileHandler::CreateDefaultFile(){
    QFile fp(_defaultfilename);

    if(!fp.open(QIODevice::WriteOnly))
        return false;

    QDataStream out(&fp);

    _defaultanchorfile.clear();
    _saturation = 230;
    _scalewidth = 700;
    _lasermin = 5;
    _lasermax = 20;
    _clockfreq = 5;

    out << _scalewidth << _lasermin << _lasermax << _clockfreq << _saturation << (uint8_t)0;

    fp.close();
    return true;
}
