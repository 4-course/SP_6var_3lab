#pragma once
#define _USE_MATH_DEFINES

#include <windows.h>
#include <math.h>

class Draw {
	HDC hdc;
	HPEN pen, graphPen;
	PAINTSTRUCT ps;
	LONG Xmin, Xmax, Ymin, Ymax;
	double mainWindowWidth, mainWindowHeight, scaleX,scaleY, offsetWidth, offsetHeight, workWidth, workHeight, arrowLength, stepX;
	double pointsX[1000];
	double pointsY[1000];

	public:
	Draw();
	double getMainWindowWidth();
	double getMainWindowHeight();
	double getXmin();
	double getXmax();
	void setXmin(double xmin);
	void setXmax(double xmax);
	void calculatePoints();
	void beginPaint(HWND);
	void endPaint(HWND);

	void drawY();
	void drawX();
	void setStepX();
	void calculateWorkRect();

	void drawGraph();
};

