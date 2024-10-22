#include "workerimageprocessing.h"
#include <QPainter>

WorkerImageProcessing::WorkerImageProcessing(QObject *parent) : QObject{parent}{

}

void WorkerImageProcessing::ThresholdImage(QImage &frame){
    // The image MUST be grayscale
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
    //
    // The image MUST be binary
    // The result WILL BE binary

    // MIDDLE OF IMAGE
    for(y=1; y<limity ;y++){
        for(x=1; x<limitx ;x++){
            multvalue = 0;
            multvalue += ((frame.pixel(x-1,y-1)&0b1)*kernel[0]);
            multvalue += ((frame.pixel(  x,y-1)&0b1)*kernel[1]);
            multvalue += ((frame.pixel(x+1,y-1)&0b1)*kernel[2]);
            multvalue += ((frame.pixel(x-1,  y)&0b1)*kernel[3]);
            multvalue += ((frame.pixel(  x,  y)&0b1)*kernel[4]);
            multvalue += ((frame.pixel(x+1,  y)&0b1)*kernel[5]);
            multvalue += ((frame.pixel(x-1,y+1)&0b1)*kernel[6]);
            multvalue += ((frame.pixel(  x,y+1)&0b1)*kernel[7]);
            multvalue += ((frame.pixel(x+1,y+1)&0b1)*kernel[8]);
            if(multvalue)
                filtered.setPixel(x, y, 0xFFFFFFFF);
            else
                filtered.setPixel(x, y, 0xFF000000);
        }
    }

    //BORDERS y=0
    y=0;
    for(x=1; x<limitx ;x++){
        multvalue = 0;
        multvalue += ((frame.pixel(x-1,  y)&0b1)*kernel[3]);
        multvalue += ((frame.pixel(  x,  y)&0b1)*kernel[4]);
        multvalue += ((frame.pixel(x+1,  y)&0b1)*kernel[5]);
        multvalue += ((frame.pixel(x-1,y+1)&0b1)*kernel[6]);
        multvalue += ((frame.pixel(  x,y+1)&0b1)*kernel[7]);
        multvalue += ((frame.pixel(x+1,y+1)&0b1)*kernel[8]);
        if(multvalue)
            filtered.setPixel(x, y, 0xFFFFFFFF);
        else
            filtered.setPixel(x, y, 0xFF000000);
    }

    //BORDERS y=height-1
    y = limity;
    for(x=1; x<limitx ;x++){
        multvalue = 0;
        multvalue += ((frame.pixel(x-1,y-1)&0b1)*kernel[0]);
        multvalue += ((frame.pixel(  x,y-1)&0b1)*kernel[1]);
        multvalue += ((frame.pixel(x+1,y-1)&0b1)*kernel[2]);
        multvalue += ((frame.pixel(x-1,  y)&0b1)*kernel[3]);
        multvalue += ((frame.pixel(  x,  y)&0b1)*kernel[4]);
        multvalue += ((frame.pixel(x+1,  y)&0b1)*kernel[5]);
        if(multvalue)
            filtered.setPixel(x, y, 0xFFFFFFFF);
        else
            filtered.setPixel(x, y, 0xFF000000);
    }

    //BORDERS x=0
    x=0;
    for(y=1; y<limity ;y++){
        multvalue = 0;
        multvalue += ((frame.pixel(  x,y-1)&0b1)*kernel[1]);
        multvalue += ((frame.pixel(x+1,y-1)&0b1)*kernel[2]);
        multvalue += ((frame.pixel(  x,  y)&0b1)*kernel[4]);
        multvalue += ((frame.pixel(x+1,  y)&0b1)*kernel[5]);
        multvalue += ((frame.pixel(  x,y+1)&0b1)*kernel[7]);
        multvalue += ((frame.pixel(x+1,y+1)&0b1)*kernel[8]);
        if(multvalue)
            filtered.setPixel(x, y, 0xFFFFFFFF);
        else
            filtered.setPixel(x, y, 0xFF000000);
    }

    //BORDERS x=width-1
    x = limitx;
    for(y=1; y<limity ;y++){
        multvalue = 0;
        multvalue += ((frame.pixel(x-1,y-1)&0b1)*kernel[0]);
        multvalue += ((frame.pixel(  x,y-1)&0b1)*kernel[1]);
        multvalue += ((frame.pixel(x-1,  y)&0b1)*kernel[3]);
        multvalue += ((frame.pixel(  x,  y)&0b1)*kernel[4]);
        multvalue += ((frame.pixel(x-1,y+1)&0b1)*kernel[6]);
        multvalue += ((frame.pixel(  x,y+1)&0b1)*kernel[7]);
        if(multvalue)
            filtered.setPixel(x, y, 0xFFFFFFFF);
        else
            filtered.setPixel(x, y, 0xFF000000);
    }

    //CORNERS x=y=0
    x = y = 0;
    multvalue = 0;
    multvalue += ((frame.pixel(  x,  y)&0b1)*kernel[4]);
    multvalue += ((frame.pixel(x+1,  y)&0b1)*kernel[5]);
    multvalue += ((frame.pixel(  x,y+1)&0b1)*kernel[7]);
    multvalue += ((frame.pixel(x+1,y+1)&0b1)*kernel[8]);
    if(multvalue)
        filtered.setPixel(x, y, 0xFFFFFFFF);
    else
        filtered.setPixel(x, y, 0xFF000000);

    //CORNERS x=y=end-1
    x = limitx;
    y = limity;
    multvalue = 0;
    multvalue += ((frame.pixel(x-1,y-1)&0b1)*kernel[0]);
    multvalue += ((frame.pixel(  x,y-1)&0b1)*kernel[1]);
    multvalue += ((frame.pixel(x-1,  y)&0b1)*kernel[3]);
    multvalue += ((frame.pixel(  x,  y)&0b1)*kernel[4]);
    if(multvalue)
        filtered.setPixel(x, y, 0xFFFFFFFF);
    else
        filtered.setPixel(x, y, 0xFF000000);

    //CORNERS x=0, y=height-1
    x = 0;
    y = limity;
    multvalue = 0;
    multvalue += ((frame.pixel(  x,y-1)&0b1)*kernel[1]);
    multvalue += ((frame.pixel(x+1,y-1)&0b1)*kernel[2]);
    multvalue += ((frame.pixel(  x,  y)&0b1)*kernel[4]);
    multvalue += ((frame.pixel(x+1,  y)&0b1)*kernel[5]);
    if(multvalue)
        filtered.setPixel(x, y, 0xFFFFFFFF);
    else
        filtered.setPixel(x, y, 0xFF000000);

    //CORNERS x=width-1, y=0
    x = limitx;
    y = 0;
    multvalue = 0;
    multvalue += ((frame.pixel(x-1,  y)&0b1)*kernel[3]);
    multvalue += ((frame.pixel(  x,  y)&0b1)*kernel[4]);
    multvalue += ((frame.pixel(x-1,y+1)&0b1)*kernel[6]);
    multvalue += ((frame.pixel(  x,y+1)&0b1)*kernel[7]);
    if(multvalue)
        filtered.setPixel(x, y, 0xFFFFFFFF);
    else
        filtered.setPixel(x, y, 0xFF000000);

    frame = filtered;
}

QVector<QRect> WorkerImageProcessing::GroupSaturation(QImage &frame){
    QVector<QRect> anchors;
    QRect anchoraux;

    // Frame MUST BE binary

    for(int y=0; y<frame.height() ;y++){
        for(int x=0; x<frame.width() ;x++){
            if(frame.pixel(x,y)&0b1){
                anchoraux = QRect(x,y,1,1);
                RecursiveGroup(frame, x, y, anchoraux);
                if(!((anchoraux.width() < _minradius) ||
                    (anchoraux.width() > _maxradius) ||
                    (anchoraux.height() < _minradius) ||
                    (anchoraux.height() > _maxradius))){
                    // IF ANCHOR SIZES ARE INSIDE RADIUS
                    anchors.append(anchoraux);
                }
            }
        }
    }

    return anchors;
}

void WorkerImageProcessing::RecursiveGroup(QImage &frame, uint32_t x, uint32_t y, QRect &anchor){
    if(x<0 || y<0 || x>=frame.width() || y>=frame.height())
        return;

    if(!(frame.pixel(x,y)&0b1))
        return;

    frame.setPixel(x, y, 0xFF000000);

    RecursiveGroup(frame, x-1, y-1, anchor);
    RecursiveGroup(frame,   x, y-1, anchor);
    RecursiveGroup(frame, x+1, y-1, anchor);
    RecursiveGroup(frame, x-1,   y, anchor);
    RecursiveGroup(frame, x+1,   y, anchor);
    RecursiveGroup(frame, x-1, y+1, anchor);
    RecursiveGroup(frame,   x, y+1, anchor);
    RecursiveGroup(frame, x+1, y+1, anchor);

    if(anchor.x() > x)
        anchor.setX(x);
    if(anchor.y() > y)
        anchor.setY(y);
    if(anchor.bottom() < y)
        anchor.setBottom(y);
    if(anchor.right() < x)
        anchor.setRight(x);
}

QRgb WorkerImageProcessing::MapDistortion(QImage &distorted, double x, double y){

    double widthratio = ((double)distorted.width())/_originalwidth;
    double heightratio = ((double)distorted.height())/_originalheight;

    double cx = _cx*widthratio;
    double cy = _cy*heightratio;
    double fx = _fx*widthratio;
    double fy = _fy*widthratio;

    double rpart1 = ((x-cx)/fx), rpart2 = ((y-cy)/fy);
    double r2 = (rpart1*rpart1) + (rpart2*rpart2);
    double radial = (((double)1.0) + _k1*r2 + _k2*r2*r2);

    double newx = qRound( (radial*(x-cx))+cx );
    double newy = qRound( (radial*(y-cy))+cy );

    if(newx>=0 && newy>=0 && newx<distorted.width() && newy<distorted.height())
        return distorted.pixel(newx, newy);

    return 0xFF000000;
}

void WorkerImageProcessing::ProcessFrame(QImage frame){

    _originalwidth = frame.width();
    _originalheight = frame.height();

    frame = frame.scaledToWidth(_scalewidth);

    if(_distortion){
        QImage frameaux = frame.convertToFormat(QImage::Format_RGB32);
        for(int y=0; y<frame.height() ;y++){
            for(int x=0; x<frame.width() ;x++){
                frame.setPixel(x, y, MapDistortion(frameaux, x, y));
            }
        }
    }

    //frame = frame.scaledToWidth(_scalewidth);

    if(_positioning){
        QImage frameaux = frame.convertToFormat(QImage::Format_ARGB32);
        QVector<QRect> anchors;

        QPainter p(&frameaux);

        ThresholdImage(frame);
        FilterImage(frame);

        anchors = GroupSaturation(frame);
        for(int i=0; i<anchors.size() ;i++){
            p.setPen(Qt::red);
            p.drawRect(anchors[i]);
        }
        p.end();

        emit FrameReady(QPixmap::fromImage(frameaux));
    }else{
        if(_saturation)
            ThresholdImage(frame);
        if(_filter)
            FilterImage(frame);

        emit FrameReady(QPixmap::fromImage(frame));
    }
}

void WorkerImageProcessing::SetThreshold(int value){
    if(value > 0 && value < 255)
        _threshold = value;
}

void WorkerImageProcessing::SetScaleWidth(int value){
    if(value > 0){
        _scalewidth = value;
    }
}

void WorkerImageProcessing::SetMinRadius(int value){
    if(value >= 0)
        _minradius = value;
}

void WorkerImageProcessing::SetMaxRadius(int value){
    if(value > 0)
        _maxradius = value;
}

void WorkerImageProcessing::SetFx(double value){
    _fx = value;
}

void WorkerImageProcessing::SetFy(double value){
    _fy = value;
}

void WorkerImageProcessing::SetCx(double value){
    _cx = value;
}

void WorkerImageProcessing::SetCy(double value){
    _cy = value;
}

void WorkerImageProcessing::SetK1(double value){
    _k1 = value;
}

void WorkerImageProcessing::SetK2(double value){
    _k2 = value;
}

void WorkerImageProcessing::SetP1(double value){
    _p1 = value;
}

void WorkerImageProcessing::SetP2(double value){
    _p2 = value;
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
