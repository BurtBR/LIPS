#include "workerpositioning.h"

WorkerPositioning::WorkerPositioning(QObject *parent) : QObject{parent}{

}

void WorkerPositioning::CheckCodes(){
    for(int i=0; i<_anchors.size() ;i++){
        if(!_anchors[i].frames[0] && !_anchors[i].frames[1])
            _anchors.remove(i--);
        else{
            emit Message("Found: " + QString::number(_anchors[i].frames[0],2).rightJustified(32,'0'));
        }
    }
}

void WorkerPositioning::ResetFound(){
    for(int i=0; i<_anchors.size() ;i++){
        _anchors[i].found = false;
    }
}

void WorkerPositioning::ResetFrames(){
    for(int i=0; i<_anchors.size() ;i++){
        _anchors[i].frames[0] = 0;
        _anchors[i].frames[1] = 0;
    }
}

void WorkerPositioning::SetAnchorSource(QTableWidget *w){
    _tableSource = w;
}

void WorkerPositioning::ResetResults(){
    _anchors.clear();
    _framecounter = 0;
}

void WorkerPositioning::InsertFound(QRect impos){

    Anchor newanchor = {
        .code = QString(),
        .X = 0,
        .Y = 0,
        .Z = 0,
        .im_pos = impos,
        .frames = {0,0},
        .found = true,
    };

    newanchor.frames[_framecounter/32] |= (1 << (31-(_framecounter%32)));
    _anchors.append(newanchor);
}

void WorkerPositioning::AnchorsInFrame(QVector<QRect> found){
    if(_framecounter >= qRound(_fps)){
        _framecounter = 0;
        CheckCodes();
        ResetFrames();
    }

    bool alreadyadded = false;

    for(int i=0; i<found.size() ;i++){
        alreadyadded = false;
        for(int j=0; j<_anchors.size() ;j++){
            if(!_anchors[j].found){
                if(_anchors[j].im_pos.intersects(found[i])){
                    _anchors[j].found = true;
                    _anchors[j].im_pos = found[i];
                    _anchors[j].frames[_framecounter/32] |= (1 << (31-(_framecounter%32)));
                    alreadyadded = true;
                    break;
                }
            }
        }
        if(!alreadyadded)
            InsertFound(found[i]);
    }

    ResetFound();
    _framecounter++;
}

void WorkerPositioning::SetFPS(float fps){
    if(fps > 64)
        emit Message("WorkerPositioning: FPS is too high. Logic will not work");
    ResetResults();
    _fps = fps;
    _avgframespersymbol = (_fps/_clockfreq);
}

void WorkerPositioning::SetClockFreq(float clock){
    _clockfreq = clock;
    emit Message("WorkerPositioning: Clock frequency set to " + QString::number(_clockfreq));
}
