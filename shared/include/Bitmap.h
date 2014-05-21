#ifndef LR_BITMAP_H_
#define LR_BITMAP_H_

#include <iostream>
#include <fstream>


#define PIXEL_SIZE 3
#define HEADER_SIZE 54

using namespace std;

/**
 * Class used to create Bitmap pictures.
 * There probably exists a better, open source, implementation for this class
 * but I did not find it, and it was fun creating my own Bitmap class.
 */
class Bitmap{
	unsigned long width;            //The width of the picture
	unsigned long height;           //The height of the picture
	unsigned long actualRowLength;	//The actual length, in bytes, of one row of pixels.
	unsigned long rowLength;        //The estimated length, in bytes of a row of pixels, without considering padding.
	unsigned long fileSize;         //The size, in bytes, of the entire Bitmap file that will be written by this class.
    
	char* data;			//A pointer to the array containing the picture and all neccessary meta data.
    
	/**
	 * Writes the RGB values for a single pixel.
	 * The RGB values are stored in a single unsigned long.
	 *
	 * @param start A pointer to the byte from which to start writting the RGB values.
	 * @param value An unsigned long containing all RGB information for a single pixel.
	 */
	void writeWord(char* start, unsigned long value);
    
    
    public:
    
	/**
	 * Constructs a Bitmap object.
	 * All pixel in the bitmap are initialized to be white.
	 *
	 * @param width The width of the bitmap in pixels.
	 * @param height The height of the bitmap in pixels.
	 */
	Bitmap(unsigned long width, unsigned long height);
    
	/**
	 * Destructs the Bitmap object.
	 * Deletes the array with bitmap data.
	 */
	virtual ~Bitmap();
    
	/**
	 * Writes the Bitmap data to an actual bitmap file.
	 *
	 * @param fileName String containing the name and directory of the output file.
	 */
	void write(string fileName);
    
	/**
	 * Sets one pixel to the specified colour.
	 * Will not draw anything nor throw any exceptions when the pixel lies outside the canvas.
	 *
	 * @param x The x coordinate of the pixel.
	 * @param y The y coordinate of the pixel.
	 * @param r The red RGB value of the pixel.
	 * @param g The green RGB value of the pixel.
	 * @param b The blue RGB value of the pixel.
	 */
	void setPixel(unsigned long x, unsigned long y, char r, char g, char b);
    
	/**
	 * Draws a point with a certain size on the canvas.
	 *
	 * @param x The x coordinate of the point.
	 * @param y The y coordinate of the point.
	 * @param pointSize The size of the point. The point will be expanded by this size to every side.
	 * As such, a pointSize of zero will paint a single pixel.
	 * @param r The red RGB value of the point.
	 * @param g The green RGB value of the point.
	 * @param b The blue RGB value of the point.
	 */
	void drawPoint(unsigned long x, unsigned long y, unsigned long pointSize = 0, char r=0, char g=0, char b=0);
    
	/**
	 * Draws a line with a certain width on the canvas.
	 *
	 * @param x1 The x coordinate of the start of the line.
	 * @param y1 The y coordinate of the start of the line.
	 * @param x2 The x coordinate of the end of the line.
	 * @param y2 The y coordinate of the end of the line.
	 * @param pointSize The size of the points used to draw the line.
	 * Note that this means that a line with a point-size larger than zero will be longer than the point start and end would indicate.
	 * @param r The red RGB value of the line.
	 * @param g The green RGB value of the line.
	 * @param b The blue RGB value of the line.
	 */
	void drawLine(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, unsigned long pointSize = 0, char r=0, char g=0, char b=0);
    
	/**
	 * Draws a box to the canvas.
	 *
	 * @param x1 The x coordinate for the bottom left of the box.
	 * @param y1 The y coordinate for the bottom left of the box.
	 * @param x2 The x coordinate for the top right of the box.
	 * @param y2 The y coordinate for the top right of the box.
	 * @param pointSize The width of the box borders.
	 * A pointsize of zero will create a box with one pixel borders.
	 * A pointsize of one will create a box with three pixel borders.
	 * @param r The red RGB value of the box.
	 * @param g The green RGB value of the box.
	 * @param b The blue RGB value of the box.
	 */
	void drawBox(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, unsigned long pointSize = 0, char r=0, char g=0, char b=0);
    
	/**
	 * Draws a filled box to the canvas.
	 *
	 * @param x1 The x coordinate for the bottom left of the box.
	 * @param y1 The y coordinate for the bottom left of the box.
	 * @param x2 The x coordinate for the top right of the box.
	 * @param y2 The y coordinate for the top right of the box.
	 * @param r The red RGB value of the box.
	 * @param g The green RGB value of the box.
	 * @param b The blue RGB value of the box.
	 */
	void drawFilledBox(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2, char r=0, char g=0, char b=0);
};

#endif /* OM_BUILDPLAN_H_ */
