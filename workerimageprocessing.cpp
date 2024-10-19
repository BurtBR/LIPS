#include "workerimageprocessing.h"

WorkerImageProcessing::WorkerImageProcessing(QObject *parent) : QObject{parent}{

}

void WorkerImageProcessing::ThresholdImage(QImage &frame){
    for(int y=0; y<frame.height() ;y++){
        for(int x=0; x<frame.width() ;x++){
            if((frame.pixel(x,y)&0xFF) > _threshold)
                frame.setPixel(x,y, 0xFFFFFFFF);
            else
                frame.setPixel(x,y, 0xFF000000);
        }
    }
}

void WorkerImageProcessing::ProcessFrame(QImage frame){

    frame = frame.convertToFormat(QImage::Format_Grayscale8);

    ThresholdImage(frame);

    emit FrameReady(QPixmap::fromImage(frame));
}

void WorkerImageProcessing::SetFPS(float fps){
    _fps = fps;
}
