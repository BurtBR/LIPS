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

QRgb WorkerImageProcessing::MapDistortion(QImage &distorted, float x, float y){

    float cx = _cx*_scaleratio;
    float cy = _cy*_scaleratio;
    float fx = _fx*_scaleratio;
    float fy = _fy*_scaleratio;

    float rpart1 = ((x-cx)/fx), rpart2 = ((y-cy)/fy);
    float r2 = (rpart1*rpart1) + (rpart2*rpart2);
    float radial = (((float)1.0) + _k1*r2 + _k2*r2*r2);

    float newx = qRound( (radial*(x-cx))+cx );
    float newy = qRound( (radial*(y-cy))+cy );

    if(newx>=0 && newy>=0 && newx<distorted.width() && newy<distorted.height())
        return distorted.pixel(newx, newy);

    return 0xFF000000;
}

void WorkerImageProcessing::ProcessFrame(QImage frame){

    _scaleratio = ( ((float)_scalewidth)/((float)frame.width()) );

    frame = frame.scaledToWidth(_scalewidth);

    if(_distortion){
        QImage frameaux = frame.convertToFormat(QImage::Format_RGB32);
        for(int y=0; y<frame.height() ;y++){
            for(int x=0; x<frame.width() ;x++){
                frame.setPixel(x, y, MapDistortion(frameaux, x, y));
            }
        }
    }

    if(_positioning){
        QImage frameaux = frame.convertToFormat(QImage::Format_ARGB32);
        QVector<QRect> anchors;

        QPainter p(&frameaux);

        ThresholdImage(frame);
        FilterImage(frame);

        anchors = GroupSaturation(frame);
        emit AnchorsOnFrame(anchors);
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
    if(value > 0 && value < 255){
        _threshold = value;
        emit Message("ImageProcessing: Threshold Set " + QString::number(value));
    }
}

void WorkerImageProcessing::SetScaleWidth(int value){
    if(value > 0){
        _scalewidth = value;
        emit Message("ImageProcessing: ScaleWidth Set " + QString::number(value));
    }
}

void WorkerImageProcessing::SetMinRadius(int value){
    if(value >= 0){
        _minradius = value;
        emit Message("ImageProcessing: MinRadius Set " + QString::number(value));
    }
}

void WorkerImageProcessing::SetMaxRadius(int value){
    if(value > 0){
        _maxradius = value;
        emit Message("ImageProcessing: MaxRaius Set " + QString::number(value));
    }
}

void WorkerImageProcessing::SetFx(float value){
    _fx = value;
    emit Message("ImageProcessing: Fx Set " + QString::number(value));
}

void WorkerImageProcessing::SetFy(float value){
    _fy = value;
    emit Message("ImageProcessing: Fy Set " + QString::number(value));
}

void WorkerImageProcessing::SetCx(float value){
    _cx = value;
    emit Message("ImageProcessing: Cx Set " + QString::number(value));
}

void WorkerImageProcessing::SetCy(float value){
    _cy = value;
    emit Message("ImageProcessing: Cy Set " + QString::number(value));
}

void WorkerImageProcessing::SetK1(float value){
    _k1 = value;
    emit Message("ImageProcessing: K1 Set " + QString::number(value));
}

void WorkerImageProcessing::SetK2(float value){
    _k2 = value;
    emit Message("ImageProcessing: K2 Set " + QString::number(value));
}

void WorkerImageProcessing::SetP1(float value){
    _p1 = value;
    emit Message("ImageProcessing: P1 Set " + QString::number(value));
}

void WorkerImageProcessing::SetP2(float value){
    _p2 = value;
    emit Message("ImageProcessing: P2 Set " + QString::number(value));
}

void WorkerImageProcessing::SetSaturationOn(bool condition){
    _saturation = condition;
    if(_saturation)
        emit Message("ImageProcessing: Saturation ON");
    else
        emit Message("ImageProcessing: Saturation OFF");
}

void WorkerImageProcessing::SetFilterOn(bool condition){
    _filter = condition;
    if(_filter)
        emit Message("ImageProcessing: Filtering ON");
    else
        emit Message("ImageProcessing: Filtering OFF");
}

void WorkerImageProcessing::SetDistortionOn(bool condition){
    _distortion = condition;
    if(_distortion)
        emit Message("ImageProcessing: Disrtortion ON");
    else
        emit Message("ImageProcessing: Disrtortion OFF");
}

void WorkerImageProcessing::SetPositionOn(bool condition){
    _positioning = condition;
    if(_positioning)
        emit Message("ImageProcessing: Positioning ON");
    else
        emit Message("ImageProcessing: Positioning OFF");
}
