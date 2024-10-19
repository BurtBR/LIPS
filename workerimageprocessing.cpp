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
    int multvalue, y, x, limitx, limity;
    uint8_t color;

    QImage filtered(frame.size(),QImage::Format_Grayscale8);

    limity = frame.height()-1;
    limitx = frame.width()-1;

    // Implemented in separate parts so that the
    // algorithm does not need to check valid pixels

    // MIDDLE OF IMAGE
    for(y=1; y<limity ;y++){
        for(x=1; x<limitx ;x++){
            multvalue = 0;
            multvalue += ((frame.pixel(x-1,y-1)&0xFF)*kernel[0]);
            multvalue += ((frame.pixel(  x,y-1)&0xFF)*kernel[1]);
            multvalue += ((frame.pixel(x+1,y-1)&0xFF)*kernel[2]);
            multvalue += ((frame.pixel(x-1,  y)&0xFF)*kernel[3]);
            multvalue += ((frame.pixel(  x,  y)&0xFF)*kernel[4]);
            multvalue += ((frame.pixel(x+1,  y)&0xFF)*kernel[5]);
            multvalue += ((frame.pixel(x-1,y+1)&0xFF)*kernel[6]);
            multvalue += ((frame.pixel(  x,y+1)&0xFF)*kernel[7]);
            multvalue += ((frame.pixel(x+1,y+1)&0xFF)*kernel[8]);
            color = multvalue;
            filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));
        }
    }

    //BORDERS y=0
    y=0;
    for(x=1; x<limitx ;x++){
        multvalue = 0;
        multvalue += ((frame.pixel(x-1,  y)&0xFF)*kernel[3]);
        multvalue += ((frame.pixel(  x,  y)&0xFF)*kernel[4]);
        multvalue += ((frame.pixel(x+1,  y)&0xFF)*kernel[5]);
        multvalue += ((frame.pixel(x-1,y+1)&0xFF)*kernel[6]);
        multvalue += ((frame.pixel(  x,y+1)&0xFF)*kernel[7]);
        multvalue += ((frame.pixel(x+1,y+1)&0xFF)*kernel[8]);
        color = multvalue;
        filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));
    }

    //BORDERS y=height-1
    y = limity;
    for(x=1; x<limitx ;x++){
        multvalue = 0;
        multvalue += ((frame.pixel(x-1,y-1)&0xFF)*kernel[0]);
        multvalue += ((frame.pixel(  x,y-1)&0xFF)*kernel[1]);
        multvalue += ((frame.pixel(x+1,y-1)&0xFF)*kernel[2]);
        multvalue += ((frame.pixel(x-1,  y)&0xFF)*kernel[3]);
        multvalue += ((frame.pixel(  x,  y)&0xFF)*kernel[4]);
        multvalue += ((frame.pixel(x+1,  y)&0xFF)*kernel[5]);
        color = multvalue;
        filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));
    }

    //BORDERS x=0
    x=0;
    for(y=1; y<limity ;y++){
        multvalue = 0;
        multvalue += ((frame.pixel(  x,y-1)&0xFF)*kernel[1]);
        multvalue += ((frame.pixel(x+1,y-1)&0xFF)*kernel[2]);
        multvalue += ((frame.pixel(  x,  y)&0xFF)*kernel[4]);
        multvalue += ((frame.pixel(x+1,  y)&0xFF)*kernel[5]);
        multvalue += ((frame.pixel(  x,y+1)&0xFF)*kernel[7]);
        multvalue += ((frame.pixel(x+1,y+1)&0xFF)*kernel[8]);
        color = multvalue;
        filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));
    }

    //BORDERS x=width-1
    x = limitx;
    for(y=1; y<limity ;y++){
        multvalue = 0;
        multvalue += ((frame.pixel(x-1,y-1)&0xFF)*kernel[0]);
        multvalue += ((frame.pixel(  x,y-1)&0xFF)*kernel[1]);
        multvalue += ((frame.pixel(x-1,  y)&0xFF)*kernel[3]);
        multvalue += ((frame.pixel(  x,  y)&0xFF)*kernel[4]);
        multvalue += ((frame.pixel(x-1,y+1)&0xFF)*kernel[6]);
        multvalue += ((frame.pixel(  x,y+1)&0xFF)*kernel[7]);
        color = multvalue;
        filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));
    }

    //CORNERS x=y=0
    x = y = 0;
    multvalue = 0;
    multvalue += ((frame.pixel(  x,  y)&0xFF)*kernel[4]);
    multvalue += ((frame.pixel(x+1,  y)&0xFF)*kernel[5]);
    multvalue += ((frame.pixel(  x,y+1)&0xFF)*kernel[7]);
    multvalue += ((frame.pixel(x+1,y+1)&0xFF)*kernel[8]);
    color = multvalue;
    filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));

    //CORNERS x=y=end-1
    x = limitx;
    y = limity;
    multvalue = 0;
    multvalue += ((frame.pixel(x-1,y-1)&0xFF)*kernel[0]);
    multvalue += ((frame.pixel(  x,y-1)&0xFF)*kernel[1]);
    multvalue += ((frame.pixel(x-1,  y)&0xFF)*kernel[3]);
    multvalue += ((frame.pixel(  x,  y)&0xFF)*kernel[4]);
    color = multvalue;
    filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));

    //CORNERS x=0, y=height-1
    x = 0;
    y = limity;
    multvalue = 0;
    multvalue += ((frame.pixel(  x,y-1)&0xFF)*kernel[1]);
    multvalue += ((frame.pixel(x+1,y-1)&0xFF)*kernel[2]);
    multvalue += ((frame.pixel(  x,  y)&0xFF)*kernel[4]);
    multvalue += ((frame.pixel(x+1,  y)&0xFF)*kernel[5]);
    color = multvalue;
    filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));

    //CORNERS x=width-1, y=0
    x = limitx;
    y = 0;
    multvalue = 0;
    multvalue += ((frame.pixel(x-1,  y)&0xFF)*kernel[3]);
    multvalue += ((frame.pixel(  x,  y)&0xFF)*kernel[4]);
    multvalue += ((frame.pixel(x-1,y+1)&0xFF)*kernel[6]);
    multvalue += ((frame.pixel(  x,y+1)&0xFF)*kernel[7]);
    color = multvalue;
    filtered.setPixel(x, y, (0xFF<<24) | (color<<16) | (color<<8) | (color));

    frame = filtered;
}

void WorkerImageProcessing::ProcessFrame(QImage frame){

    if(_saturation)
        ThresholdImage(frame);
    if(_filter)
        FilterImage(frame);

    emit FrameReady(QPixmap::fromImage(frame));
}

void WorkerImageProcessing::SetFPS(float fps){
    _fps = fps;
}

void WorkerImageProcessing::SetThreshold(int value){
    if(value > 0 && value < 255)
        _threshold = value;
}

void WorkerImageProcessing::SetSaturationOn(bool condition){
    _saturation = condition;
}

void WorkerImageProcessing::SetFilterOn(bool condition){
    _filter = condition;
}

void WorkerImageProcessing::SetDistortionOn(bool condition){
    _distortion = condition;
}

void WorkerImageProcessing::SetPositionOn(bool condition){
    _positioning = condition;
}
