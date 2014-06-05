#include "Bitmap.h"


Bitmap::Bitmap(unsigned long width, unsigned long height) : width(width), height(height)
{
    
    //Add padding
    actualRowLength = (width*PIXEL_SIZE);
    rowLength = actualRowLength;
    while((rowLength % 4) != 0){
        rowLength++;
    }
    
    fileSize = HEADER_SIZE + rowLength*height;
    data = new char[fileSize];
    
    //magic word
    data[0] = 'B';
    data[1] = 'M';
    
    //file size
    writeWord(&data[2], fileSize);
    
    //something unknown
    writeWord(&data[6], 0);
    
    //Start of the bitmap.
    writeWord(&data[10], HEADER_SIZE);
    
    //Size of the header. Normally set to 40. The header is considered to start at offset 15.
    writeWord(&data[14], 40);
    
    //Width
    writeWord(&data[18], width);
    
    //height
    writeWord(&data[22], height);
    
    //Planes. Must be 1.
    data[26] = 1;
    data[27] = 0;
    
    //Bits per pixel. You'll need this as 24.
    data[28] = 24;
    data[29] = 0;
    
    //Compression. Leave it as zero.
    writeWord(&data[30], 0);
    
    //Compressed size. Since there's no compression, this value will be the same as file size.
    writeWord(&data[34], fileSize);
    
    //Horizontal resolution. See below.
    writeWord(&data[38], 2835);
    
    //Vertical resolution. Pixels per meter, according to Wikipedia. The values seem to vary, but 2835 should do for both.
    writeWord(&data[42], 2835);
    
    //Number of colors in the palette. Leave as zero.
    writeWord(&data[46], 0);
    
    //"Important colors". Leave as zero.
    writeWord(&data[50], 0);
    
    //Fill canvas with white
    for(unsigned int i=HEADER_SIZE; i<fileSize; i++){
        if((i-HEADER_SIZE)%rowLength > actualRowLength){
            //This is just padding
            data[i] = 0;
        } else {
            data[i] = 255;
        }
    }
}


Bitmap::~Bitmap()
{
    delete(data);
}


void Bitmap::write(string fileName)
{
    ofstream bmpFile;
    bmpFile.open(fileName.c_str());
    bmpFile.write(data, fileSize);
    bmpFile.close();
}


void Bitmap::setPixel(unsigned long x, unsigned long y, char r, char g, char b)
{
    if(x>width || y >height){
        return;
    }
    
    unsigned long index = HEADER_SIZE + x*PIXEL_SIZE + y*rowLength;
    
    data[index] = r;
    data[index+1] = g;
    data[index+2] = b;
}


void Bitmap::drawPoint(unsigned long x, unsigned long y, unsigned long pointSize, char r, char g, char b)
{
    drawFilledBox(x-pointSize, y-pointSize, x+pointSize, y+pointSize, r, g, b);
}


void Bitmap::drawLine(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, unsigned long pointSize, char r, char g, char b)
{
    if(x1>x2){
        unsigned long temp = x2;
        x2 = x1;
        x1 = temp;
    }
    
    if(y1>y2){
        unsigned long temp = y2;
        y2 = y1;
        y1 = temp;
    }
    
    x2++;
    y2++;
    double x1d = x1;
    double y1d = y1;
    double x2d = x2;
    double y2d = y2;
    double yPieceIncrease = (y2d-y1d)/(x2d-x1d);
    
    for(unsigned long x=x1; x<x2; x++){
        unsigned long yStart = y1 + (x-x1)*yPieceIncrease;
        unsigned long yEnd = y1 + (x-x1+1)*yPieceIncrease;
        if(yStart==yEnd) yEnd++;
        
        for(unsigned long y=yStart; y<yEnd; y++){
            drawPoint(x, y, pointSize, r, g, b);
        }
    }
}


void Bitmap::drawBox(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, unsigned long pointSize, char r, char g, char b)
{
    drawFilledBox(x1-pointSize, y1-pointSize, x1+pointSize, y2+pointSize, r, g, b);
    drawFilledBox(x1-pointSize, y1-pointSize, x2+pointSize, y1+pointSize, r, g, b);
    drawFilledBox(x1-pointSize, y2-pointSize, x2+pointSize, y2+pointSize, r, g, b);
    drawFilledBox(x2-pointSize, y1-pointSize, x2+pointSize, y2+pointSize, r, g, b);
}


void Bitmap::drawFilledBox(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, char r, char g, char b)
{
    for(unsigned long x=x1; x<=x2; x++){
        for(unsigned long y=y1; y<=y2; y++){
            setPixel(x, y, r, g, b);
        }
    }
}


void Bitmap::writeWord(char* start, unsigned long value){
    *(start+3) = value / 16777216;
    value -= *(start+3)*16777216;
    *(start+2) = value / 65536;
    value -= *(start+2)*65536;
    *(start+1) = value / 265;
    value -= *(start+1)*265;
    *(start) = value;
}