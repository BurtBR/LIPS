#include "workerpositioning.h"

WorkerPositioning::WorkerPositioning(QObject *parent) : QObject{parent}{
    _K.fill(0);
    _K.data()[8] = 1;
    _R.fill(0);
}

QString WorkerPositioning::GetModelStr(PositioningModel modeltype){
    switch(modeltype){
    case PositioningModel::Trilateration:
        return "Trilateration";
        break;
    case PositioningModel::Proportions:
        return "Proportions";
        break;
    case PositioningModel::Pinhole:
        return "Pinhole";
        break;
    default:
        return QString();
        break;
    }
}

void WorkerPositioning::CheckCodes(){

    uint8_t fps = qRound(_fps), transition, valuecounter, qtd, tableidx;
    bool waszero = false, value;
    QString code;

    for(int i=0; i<_anchors.size() ;i++){
        if(!_anchors[i].frames)
            _anchors.remove(i--);
        else{
            transition = 0;
            waszero = false;
            for(int j=0; j<fps ;j++){
                value = (_anchors[i].frames & (1 << (31-(j%32))));
                if( value && waszero ){
                    transition = j;
                    break;
                }else if(!value)
                    waszero = true;
            }

            valuecounter = 0;
            waszero = false;
            code.clear();
            value = 0;
            for(int j=0; j<fps ;j++){
                value = (_anchors[i].frames & (1 << (31-((j+transition)%fps))));
                if( value && waszero ){
                    qtd = qRound(((float)valuecounter)/_avgframespersymbol);
                    while(qtd){
                        code.append('0');
                        qtd--;
                    }
                    valuecounter = 1;
                    waszero = false;
                }else if(!value && !waszero){
                    qtd = qRound(((float)valuecounter)/_avgframespersymbol);
                    while(qtd){
                        code.append('1');
                        qtd--;
                    }
                    valuecounter = 1;
                    waszero = true;
                }else{
                    valuecounter ++;
                }
            }
            if( value ){
                qtd = qRound(((float)valuecounter)/_avgframespersymbol);
                while(qtd){
                    code.append('1');
                    qtd--;
                }
            }else{
                qtd = qRound(((float)valuecounter)/_avgframespersymbol);
                while(qtd){
                    code.append('0');
                    qtd--;
                }
            }
            if(Validate(code,tableidx)){
                if(!_validAnchors.contains(code)){
                    _anchors[i].code = code;
                    _anchors[i].X = _tableSource->item(tableidx,1)->text().toFloat();
                    _anchors[i].Y = _tableSource->item(tableidx,2)->text().toFloat();
                    _anchors[i].Z = _tableSource->item(tableidx,3)->text().toFloat();
                    emit Message("Found: " + code +
                                 " - Real: X: " + _tableSource->item(tableidx,1)->text() +
                                 " Y: " + _tableSource->item(tableidx,2)->text() +
                                 " Z: " + _tableSource->item(tableidx,3)->text() +
                                 "  - Image: x: " + QString::number(_anchors[i].im_pos.center().x()) +
                                 " y: " + QString::number(_anchors[i].im_pos.center().y()));
                    _validAnchors.insert(code, _anchors[i]);
                }else{
                    //if(!_validAnchors[code].im_pos.intersects(_anchors[i].im_pos)){
                    if((_validAnchors[code].im_pos.center() - _anchors[i].im_pos.center()).manhattanLength()>3){
                        emit Message("Anchor " + code + " moved to x: " + QString::number(_anchors[i].im_pos.center().x()) +
                                     " y: " + QString::number(_anchors[i].im_pos.center().y()));
                    }
                    _validAnchors[code].im_pos = _anchors[i].im_pos;
                }
            }
            else
                _anchors.remove(i--);
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
        _anchors[i].frames = 0;
    }
}

void WorkerPositioning::SetCurrentModel(PositioningModel model){
    _currModel = model;
    emit Message("WorkerPositioning: Positioning model set to " + GetModelStr(_currModel));
}

void WorkerPositioning::SetAnchorSource(QTableWidget *w){
    _tableSource = w;
}

void WorkerPositioning::ResetResults(){
    _anchors.clear();
    _validAnchors.clear();
    _framecounter = 0;
}

void WorkerPositioning::InsertFound(QRect impos){

    Anchor newanchor = {
        .code = QString(),
        .X = 0,
        .Y = 0,
        .Z = 0,
        .im_pos = impos,
        .frames = 0,
        .found = true,
    };

    newanchor.frames |= (1 << (31-(_framecounter%32)));
    _anchors.append(newanchor);
}

bool WorkerPositioning::Validate(QString &code, uint8_t &tableidx){
    uint8_t onescode = Ones(code), oneanchors;
    for(int i=0; i<_tableSource->rowCount() ;i++){
        oneanchors = Ones(_tableSource->item(i,0)->text());
        if(onescode == oneanchors){
            for(int j=0; j<code.size() ;j++){
                if(code.compare(_tableSource->item(i,0)->text()) == 0){
                    tableidx = i;
                    return true;
                }else{
                    code.prepend(code[code.size()-1]);
                    code.removeLast();
                }
            }
        }
    }
    tableidx = -1;
    return false;
}

uint8_t WorkerPositioning::Ones(QString code){
    uint8_t result = 0;
    for(int i=0; i<code.size() ;i++){
        if(code[i] == '1')
            result++;
    }
    return result;
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
                    _anchors[j].frames |= (1 << (31-(_framecounter%32)));
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
    if(fps > 32)
        emit Message("WorkerPositioning: FPS is too high. Logic will not work");
    ResetResults();
    _fps = fps;
    _avgframespersymbol = (_fps/_clockfreq);
}

void WorkerPositioning::SetClockFreq(float clock){
    _clockfreq = clock;
    emit Message("WorkerPositioning: Bits per second set to " + QString::number(_clockfreq));
}

void WorkerPositioning::SetFx(float value){
    _K.data()[0] = value;
}

void WorkerPositioning::SetFy(float value){
    _K.data()[4] = value;
}

void WorkerPositioning::SetCx(float value){
    _K.data()[6] = value;
}

void WorkerPositioning::SetCy(float value){
    _K.data()[7] = value;
}

void WorkerPositioning::SetR(QMatrix3x3 R){
    _R = R;
    emit Message(QString("WorkerPositioning: R set to\n") +
                 "\t" + QString::number(R.data()[0]) + " " + QString::number(R.data()[3]) + " " + QString::number(R.data()[6]) + "\n" +
                 "R=\t" + QString::number(R.data()[1]) + " " + QString::number(R.data()[4]) + " " + QString::number(R.data()[7]) + "\n" +
                 "\t" + QString::number(R.data()[2]) + " " + QString::number(R.data()[5]) + " " + QString::number(R.data()[8]));
}
