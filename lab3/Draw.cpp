#include "Draw.h"

void Draw::setCenter(double x, double y) {
    center.x = x;
    center.y = y;
}

Draw::Draw() {
    arrowLength = divisionLength = divisionStepX = divisionStepY = divisionsCountY = 
        divisionsCountX = offsetWidth = offsetHeight = scaleX = scaleY = workHeight = workWidth = 0;

    font = NULL;
    graphPen =pen= NULL;
    hdc = NULL;

    mainWindowWidth = 850;
    mainWindowHeight = 600;
    Xmin = -100;
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

void Draw::setMainWindowSize(HWND hWnd) {
    RECT rect;
    GetWindowRect(hWnd, &rect);
    mainWindowWidth = (double)rect.right - rect.left;
    mainWindowHeight = (double)rect.bottom - rect.top;
}

double Draw::getXmin() {
    return Xmin;
}

double Draw::getXmax() {
    return Xmax;
}

HDC Draw::getHDC() {
    return hdc;
}

void Draw::setXmin(double xmin) {
    Xmin = xmin;
}

void Draw::setXmax(double xmax) {
    Xmax = xmax;
}

void Draw::calculatePoints() {
    double step = ((double)Xmax - Xmin) / 1000.0;
    double k = Xmin;
    for (int i = 0; i < 1000; i++) {
        pointsX[i] = scaleX*k;
        pointsY[i] = -scaleY * (1 - cos(k));
        k += step;
    }
}

void Draw::beginPaint(HWND hWnd) {
    hdc = BeginPaint(hWnd, &ps);

    pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    font = CreateFont(mainWindowHeight*0.035, 8, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET,
                      OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
    SelectObject(hdc, font);
    SelectObject(hdc, pen);
}

void Draw::endPaint(HWND hWnd) {
    EndPaint(hWnd, &ps);

    DeleteObject(pen);
    DeleteObject(graphPen);
    DeleteObject(font);
}

void Draw::drawY() {
    double x=0, y=0;
    //рисуем ось Y
    //тут может быть несколько вариантов
    //1 - когда ось Y видна
    if (Xmin <= 0 && Xmax >= 0) {
        x = offsetWidth + scaleX * abs(Xmin);
        y = mainWindowHeight - offsetHeight;
        setCenter(x, y);
    }
    //2 - когда ось Y не видна, рисуем ее просто слева/справа
    else if (Xmin < 0 && Xmax < 0) {
        x = mainWindowWidth - offsetWidth;
        y = mainWindowHeight - offsetHeight;
        setCenter(x + scaleX * abs(Xmax), y);
    }
    else if (Xmin > 0 && Xmax > 0) {
        x = offsetWidth;
        y = mainWindowHeight - offsetHeight;
        setCenter(x - Xmin * scaleX, y);
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

void Draw::setStepsCount() {
    //определимся с делением по оси Х
    divisionsCountX = 0.05*workWidth /M_PI;
    divisionStepX = ((double)abs(Xmin) + abs(Xmax))/ divisionsCountX;

    divisionsCountY = workHeight / 50;
    divisionStepY = Ymax*scaleY/divisionsCountY;

    divisionLength = 0.025 * mainWindowHeight;
}

void Draw::drawDivisionX() {

    int leftDivisionsCount = 0;
    int rightDivisionCount = divisionsCountX - 1;
    double centerX = 0;
    double divisionValue = ((double)abs(Xmax) + abs(Xmin))/ divisionsCountX;
    double step = divisionValue;

    if (Xmin <= 0 && Xmax >= 0) {
        leftDivisionsCount = abs(Xmin)/ divisionStepX;
        rightDivisionCount = divisionsCountX - leftDivisionsCount - 1;
    }
    else if (Xmin < 0 && Xmax < 0) {
        leftDivisionsCount = divisionsCountX - 1;
        rightDivisionCount = 0;
    }

    for (int i = 1; i <= rightDivisionCount; i++) {
        MoveToEx(hdc, centerX+i * divisionStepX*scaleX, divisionLength / 2, NULL);
        LineTo(hdc, centerX+i * divisionStepX * scaleX, -divisionLength / 2);

        std::wstringstream wss;
        wss << round(divisionValue * 100) / 100;
        std::wstring converted = wss.str();
        LPCWSTR value = converted.c_str();

        TextOut(hdc, centerX+i * divisionStepX * scaleX - 8, (divisionLength / 2 + mainWindowHeight * 0.005), value, converted.size());
        divisionValue += step;
    }

    divisionValue = -step;

    for (int i = 1; i <=leftDivisionsCount; i++) {
        MoveToEx(hdc, centerX - i * divisionStepX * scaleX, divisionLength / 2, NULL);
        LineTo(hdc, centerX - i * divisionStepX * scaleX, -divisionLength / 2);

        std::wstringstream wss;
        wss << round(divisionValue * 100) / 100;
        std::wstring converted = wss.str();
        LPCWSTR value = converted.c_str();

        TextOut(hdc, centerX - i * divisionStepX * scaleX - 8, (divisionLength / 2 + mainWindowHeight * 0.005), value, converted.size());
        divisionValue -= step;
    }

    TextOut(hdc, -4, (divisionLength / 2 + mainWindowHeight * 0.005), L"0", 1);
}

void Draw::drawDivisionY() {
    double divisionValue = (double)Ymax / divisionsCountY;
    double step = divisionValue; 
    double centerY;
    double textLocation = 0.0;
    if (Xmin <= 0 && Xmax >= 0) {
        centerY = 0;
        textLocation = divisionLength / 2 + 1;
    }
    else if (Xmin > 0 && Xmax > 0) {
        centerY = Xmin * scaleX;
        textLocation = centerY+divisionLength / 2 + 1;
    }
    else {
        centerY = Xmax * scaleX;
        textLocation = centerY-divisionLength / 2 - 28;
    }
    for (int i = 1; i <= divisionsCountY; i++) {
        MoveToEx(hdc, centerY+divisionLength / 2, -i * divisionStepY, NULL);
        LineTo(hdc, centerY-divisionLength / 2, -i * divisionStepY);

        std::wstringstream wss;
        wss << round(divisionValue*100)/100;
        std::wstring converted = wss.str();
        LPCWSTR value = converted.c_str();

        TextOut(hdc, textLocation, -i * divisionStepY - 8, value, converted.size());
        divisionValue+=step;
    }
}

void Draw::calculateWorkRect() {
    //на данном этапе надо посчитать размер шрифта, размер интервала
    //определим рабочую область
    offsetWidth = 0.05 * mainWindowWidth;
    offsetHeight = 0.15 * mainWindowHeight;

    workWidth = mainWindowWidth - 2.0 * offsetWidth;
    workHeight = mainWindowHeight - 2.0 * offsetHeight;

    //и машстаб
    scaleX = workWidth / ((double)Xmax - Xmin);
    scaleY = workHeight/2.0;
}

void Draw::setNewView() {
    SetViewportExtEx(hdc, workWidth, -workHeight, NULL);
    SetViewportOrgEx(hdc, center.x, center.y, NULL);
}

void Draw::drawGraph() {
    graphPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
    SelectObject(hdc, graphPen);
    MoveToEx(hdc, pointsX[0], pointsY[0], NULL);

    for (int i = 1; i < 1000; i++) {
        LineTo(hdc, pointsX[i], pointsY[i]);
    }
}
