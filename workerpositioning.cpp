#include "workerpositioning.h"

WorkerPositioning::WorkerPositioning(QObject *parent) : QObject{parent}{

}

void WorkerPositioning::SetAnchorSource(QTableWidget *w){
    _tableSource = w;
}

void WorkerPositioning::UpdateAnchorValues(){
    if(_tableSource){
        qDebug() << "UPDATE TABELA:";
        for(int i=0; i<_tableSource->rowCount() ;i++){
            qDebug() << _tableSource->item(i,0)->text();
        }
    }
}
