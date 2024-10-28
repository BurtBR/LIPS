#include "workerpositioning.h"

WorkerPositioning::WorkerPositioning(QObject *parent) : QObject{parent}{

}

void WorkerPositioning::CheckCodes(){

    uint8_t fps = qRound(_fps), transition, valuecounter, qtd;
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
            if(Validate(code))
                emit Message("Found: " + code);
            else
                _anchors.remove(i--);
            // _anchors[j].frames[_framecounter/32] |= (1 << (31-(_framecounter%32)));
            //emit Message("Found: " + QString::number(_anchors[i].frames,2).rightJustified(32,'0'));
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
        .frames = 0,
        .found = true,
    };

    newanchor.frames |= (1 << (31-(_framecounter%32)));
    _anchors.append(newanchor);
}

bool WorkerPositioning::Validate(QString &code){
    uint8_t onescode = Ones(code), oneanchors;
    for(int i=0; i<_tableSource->rowCount() ;i++){
        oneanchors = Ones(_tableSource->item(i,0)->text());
        if(onescode == oneanchors){
            for(int j=0; j<code.size() ;j++){
                if(code.compare(_tableSource->item(i,0)->text()) == 0){
                    return true;
                }else{
                    code.prepend(code[code.size()-1]);
                    code.removeLast();
                }
            }
        }
    }
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
    emit Message("WorkerPositioning: Clock frequency set to " + QString::number(_clockfreq));
}
