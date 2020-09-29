#pragma once
#define _USE_MATH_DEFINES

#include <windows.h>
#include <math.h>
#include <sstream>
#include <iostream>

class Draw {
	HDC hdc;
	HPEN pen, graphPen;
	HFONT font;
	PAINTSTRUCT ps;
	POINT center;
	double Xmin, Xmax, Ymin, Ymax,
		mainWindowWidth, mainWindowHeight,
		scaleX, scaleY,
		offsetWidth, offsetHeight, 
		workWidth, workHeight, 
		arrowLength, 
		divisionStepX, divisionStepY, divisionLength;
	int divisionsCountX, divisionsCountY;
	double pointsX[1000];
	double pointsY[1000];

	void setCenter(double x, double y);

	public:
	Draw();
	double getMainWindowWidth();
	double getMainWindowHeight();
	void setMainWindowSize(HWND);
	double getXmin();
	double getXmax();
	HDC getHDC();
	void setXmin(double xmin);
	void setXmax(double xmax);
	void calculatePoints();
	void beginPaint(HWND);
	void endPaint(HWND);

	void drawY();
	void drawX();
	void setStepsCount();
	void drawDivisionX();
	void drawDivisionY();
	void calculateWorkRect();

	void setNewView();

	void drawGraph();

};

