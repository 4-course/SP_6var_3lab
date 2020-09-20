#include "Draw.h"

Draw::Draw() {
    mainWindowWidth = 850;
    mainWindowHeight = 600;
    Xmin = -10;
    Xmax = 50;
    Ymin = 0;
    Ymax = 2;
}

double Draw::getMainWindowWidth() {
    return mainWindowWidth;
}

double Draw::getMainWindowHeight() {
    return mainWindowHeight;
}

double Draw::getXmin() {
    return Xmin;
}

double Draw::getXmax() {
    return Xmax;
}

void Draw::setXmin(double xmin) {
    Xmin = xmin;
}

void Draw::setXmax(double xmax) {
    Xmax = xmax;
}

void Draw::calculatePoints() {
    double step = (Xmax - Xmin) / 1000.0;
    double plusX = offsetWidth;
    long k = Xmin;
    double plus = mainWindowHeight - offsetHeight;
   /* pointsX[0] = plusX;
    pointsY[0] = plus - scaleY * (1 - cos(k));*/
    for (int i = 0; i < 1000; i++) {
        pointsX[i] = plusX+scaleX*k;
        pointsY[i] = plus-scaleY*(1 - cos(k));
        k += step;
    }
}

void Draw::beginPaint(HWND hWnd) {
    hdc = BeginPaint(hWnd, &ps);

    pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, pen);
}

void Draw::endPaint(HWND hWnd) {
    EndPaint(hWnd, &ps);

    DeleteObject(pen);
    DeleteObject(graphPen);
}

void Draw::drawY() {
    double x, y;
    //рисуем ось Y
    //тут может быть несколько вариантов
    //1 - когда ось Y видна
    if (Xmin <= 0 && Xmax >= 0) {
        x = offsetWidth + scaleX * abs(Xmin);
        y = mainWindowHeight - offsetHeight;
    }
    //2 - когда ось Y не видна, рисуем ее просто слева/справа
    else if (Xmin < 0 && Xmax < 0) {
        x = mainWindowWidth - offsetWidth;
        y = mainWindowHeight - offsetHeight;
    }
    else if (Xmin > 0 && Xmax > 0) {
        x = offsetWidth;
        y = mainWindowHeight - offsetHeight;
    }

    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x, offsetHeight / 2.0);
    //и стрелочку 
    LineTo(hdc, x - arrowLength / 2.0, offsetHeight / 2.0 + arrowLength * sqrt(3.0) / 2.0);
    MoveToEx(hdc, x, offsetHeight / 2.0, NULL);
    LineTo(hdc, x + arrowLength / 2.0, offsetHeight / 2.0 + arrowLength * sqrt(3.0) / 2.0);
}

void Draw::drawX() {
    //рисуем оси. всегда видна ось X, рисуем ее
    MoveToEx(hdc, offsetWidth, mainWindowHeight - offsetHeight, NULL);
    LineTo(hdc, mainWindowWidth - offsetWidth, mainWindowHeight - offsetHeight);

    //рисуем стрелочку у оси Х
    //определим длину стрелочки
    arrowLength = offsetWidth / 3.0;
    LineTo(hdc, mainWindowWidth - offsetWidth - arrowLength * sqrt(3.0) / 2.0, mainWindowHeight - offsetHeight - arrowLength / 2.0);
    MoveToEx(hdc, mainWindowWidth - offsetWidth, mainWindowHeight - offsetHeight, NULL);
    LineTo(hdc, mainWindowWidth - offsetWidth - arrowLength * sqrt(3.0) / 2.0, mainWindowHeight - offsetHeight + arrowLength / 2.0);
}

void Draw::setStepX() {
    //определимся с делением по оси Х
    //количество делений пусть будет всегда не больше 20
    stepX = workWidth / M_PI;
}

void Draw::calculateWorkRect() {
    //на данном этапе надо посчитать размер шрифта, размер интервала
    //определим рабочую область
    offsetWidth = 0.05 * mainWindowWidth;
    offsetHeight = 0.15 * mainWindowHeight;

    workWidth = mainWindowWidth - 2.0 * offsetWidth;
    workHeight = mainWindowHeight - 2.0 * offsetHeight;

    //и машстаб
    scaleX = workWidth / (Xmax - Xmin);
    scaleY = workHeight/3.0;
}

void Draw::drawGraph() {
    graphPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    SelectObject(hdc, graphPen);
    MoveToEx(hdc, pointsX[0], pointsY[0], NULL);

    for (int i = 1; i < 1000; i++) {
        LineTo(hdc, pointsX[i], pointsY[i]);
    }
}
