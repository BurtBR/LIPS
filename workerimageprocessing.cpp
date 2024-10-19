#include "workerimageprocessing.h"

WorkerImageProcessing::WorkerImageProcessing(QObject *parent) : QObject{parent}{

}

void WorkerImageProcessing::ThresholdImage(QImage &frame){
    for(int y=0; y<frame.height() ;y++){
        for(int x=0; x<frame.width() ;x++){
            if((frame.pixel(x,y)&0xFF) >= _threshold)
                frame.setPixel(x,y, 0xFFFFFFFF);
            else
                frame.setPixel(x,y, 0xFF000000);
        }
    }
}

void WorkerImageProcessing::FilterImage(QImage &frame){

    int kernel[9] = {-1, -1, -1,
                     -1,  8, -1,
                     -1, -1, -1};
    int multvalue;
    uint8_t color;

    QImage filtered(frame.size(),QImage::Format_Grayscale8);

    for(int y=0; y<frame.height() ;y++){
        for(int x=0; x<frame.width() ;x++){
            multvalue = 0;
            multvalue += MultIfValidPixel(frame, x-1, y-1, kernel[0]);
            multvalue += MultIfValidPixel(frame,   x, y-1, kernel[1]);
            multvalue += MultIfValidPixel(frame, x+1, y-1, kernel[2]);
            multvalue += MultIfValidPixel(frame, x-1, y,   kernel[3]);
            multvalue += MultIfValidPixel(frame,   x, y,   kernel[4]);
            multvalue += MultIfValidPixel(frame, x+1, y,   kernel[5]);
            multvalue += MultIfValidPixel(frame, x-1, y+1, kernel[6]);
            multvalue += MultIfValidPixel(frame,   x, y+1, kernel[7]);
            multvalue += MultIfValidPixel(frame, x+1, y+1, kernel[8]);
            color = multvalue;
            filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));
        }
    }
    frame = filtered;
}

int WorkerImageProcessing::MultIfValidPixel(QImage &frame, int x, int y, int &value){
    if(x<0 || y<0 || x>=frame.width() || y>=frame.height())
        return 0;
    return (frame.pixel(x,y)&0xFF)*value;
}

void WorkerImageProcessing::ProcessFrame(QImage frame){

    frame = frame.convertToFormat(QImage::Format_Grayscale8);

    ThresholdImage(frame);
    FilterImage(frame);

    emit FrameReady(QPixmap::fromImage(frame));
}

void WorkerImageProcessing::SetFPS(float fps){
    _fps = fps;
}

void WorkerImageProcessing::SetSaturation(int value){
    if(value > 0 && value < 255)
        _threshold = value;
}
