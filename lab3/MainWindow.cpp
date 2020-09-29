
#include <windows.h>
#include <stdio.h>
#include "Draw.h"
#include "resource.h"

bool hasMinus, hasDot, shouldClose;
Draw* draw;
WNDPROC oldEdit1Proc = NULL;
WNDPROC oldEdit2Proc = NULL;

void checkIsCLassRegistered(WNDCLASSEX windowClass) {
    if (!RegisterClassEx(&windowClass)) {
        MessageBox(NULL, L"Не удалось создать класс окна. Ошибка " + GetLastError(), L"Создание класса окна", MB_ICONERROR);
        exit(0);
    }
}

void checkIsWindowCreated(HWND window) {
    if (!window) {
        MessageBox(NULL, L"Не удалось создать окно. Ошибка " + GetLastError(), L"Создание окна", MB_ICONERROR);
        exit(0);
    }
}

LRESULT CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    wchar_t xmin[100000], xmax[100000];
    std::wstringstream wss;
    std::wstring converted;
    std::wstring converted2;
    LPCWSTR value1, value2;
    bool hasMinus1 = false;
    bool hasMinus2 = false;
    bool hasDot1 = false;
    bool hasDot2 = false;
    int dot1 = -1;
    int dot2 = -1;
    bool isNumber = true;
    switch (message) {
        case WM_INITDIALOG:

            wss << draw->getXmax();
            converted = wss.str();
            value1 = converted.c_str();

            wss.str(L"");
            
            wss << draw->getXmin();
            converted2 = wss.str();
            value2 = converted2.c_str();

            wss.str(L"");

            SetDlgItemText(hDlg, IDC_EDIT1, value2);
            SetDlgItemText(hDlg, IDC_EDIT2, value1);
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    GetDlgItemText(hDlg, IDC_EDIT1, xmin, sizeof(xmin));
                    GetDlgItemText(hDlg, IDC_EDIT2, xmax, sizeof(xmax));

                    for (int i = 0; i < 100000; i++) {
                        if (i == 0 && xmin[i] == '-')
                            hasMinus1 = true;
                        else if (i != 0 && xmin[i] == '.') {
                            hasDot1 = true;
                            dot1 = i;
                        }
                        else if (xmin[i] == '\0')
                            break;
                        else if ((hasMinus1 && xmin[i] == '-' && i != 0) || (hasDot1 && xmin[i] == '.' && dot1 != i)
                            || ((xmin[i] < '0' || xmin[i]>'9') && xmin[i] != '.' && xmin[i] != '-')) {
                            isNumber = false;
                            break;
                        }
                    }

                    for (int i = 0; i < 100000; i++) {
                        if (i == 0 && xmax[i] == '-')
                            hasMinus2 = true;
                        else if (i != 0 && xmax[i] == '.') {
                            hasDot2 = true;
                            dot2 = i;
                        }
                        else if (xmax[i] == '\0')
                            break;
                        else if ((hasMinus2 && xmax[i] == '-' && i != 0) || (hasDot2 && xmax[i] == '.' && dot2 != i)
                            || ((xmax[i] < '0' || xmax[i]>'9') && xmax[i] != '.' && xmax[i] != '-')) {
                            isNumber = false;
                            break;
                        }
                    }

                    if (!isNumber) {
                        MessageBox(NULL, L"Введите число пожалуйста" + GetLastError(), L"Неверное число", MB_ICONERROR);
                        break;
                    }


                    draw->setXmin(_wtof(xmin));
                    draw->setXmax(_wtof(xmax));
                    if (draw->getXmin() >= draw->getXmax())
                        MessageBox(hDlg, L"Значение Xmin должно быть меньше Xmax", L"Значения границ", MB_ICONINFORMATION);
                    else
                        EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                    break;
                case IDCANCEL:
                    shouldClose = true;
                    EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                    break;
            }
            break;
        case WM_CLOSE:
            shouldClose = true;
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
    }
    return FALSE;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    RECT rt;
    switch (uMsg) {
        case WM_PAINT:

            draw->setMainWindowSize(hWnd);

            draw->beginPaint(hWnd);

            GetClientRect(hWnd, &rt);
            FillRect(draw->getHDC(), &rt, (HBRUSH)(COLOR_WINDOW + 1));

            draw->calculateWorkRect();

            draw->drawX();
            draw->drawY();

            draw->setNewView();

            draw->setStepsCount();
            draw->drawDivisionX();
            draw->drawDivisionY();
            

            draw->calculatePoints();

            draw->drawGraph();

            draw->endPaint(hWnd);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_40001:
                    if (DialogBox(NULL, (LPCTSTR)IDD_DIALOG1, hWnd, (DLGPROC)DlgProc) == IDOK)
                        InvalidateRect(hWnd, NULL, TRUE);
                    break;
            }
            break;
        case WM_LBUTTONDOWN:
            if (DialogBox(NULL, (LPCTSTR)IDD_DIALOG1, hWnd, (DLGPROC)DlgProc) == IDOK)
                InvalidateRect(hWnd, NULL, TRUE);
            break;
        case WM_SIZE:
            GetClientRect(hWnd, &rt);
            InvalidateRect(hWnd, &rt, FALSE);
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst,
                   HINSTANCE hPreviousInst,
                   LPSTR lpCommandLine,
                   int nCommandShow) {
    double screenWidth = GetSystemMetrics(SM_CXSCREEN),
        screenHeight = GetSystemMetrics(SM_CYSCREEN);
    draw = new Draw();
    hasMinus = false;
    hasDot = false;
    shouldClose = false;
    MSG uMsg;
    WNDCLASSEX mainWindowClass;
    HWND mainWindow;

    memset(&mainWindowClass, 0, sizeof(WNDCLASSEX));
    mainWindowClass.cbSize = sizeof(WNDCLASSEX);
    mainWindowClass.hbrBackground = (HBRUSH)WHITE_BRUSH;
    mainWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    mainWindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    mainWindowClass.hInstance = hInst;
    mainWindowClass.lpfnWndProc = WindowProc;
    mainWindowClass.lpszClassName = L"mainWindow";
    mainWindowClass.lpszMenuName = (LPCWSTR)IDR_MENU1;

    checkIsCLassRegistered(mainWindowClass);
    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, (DLGPROC)DlgProc);
    if (!shouldClose) {
        mainWindow = CreateWindow(mainWindowClass.lpszClassName, L"График функции", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME,
                                  (screenWidth - draw->getMainWindowWidth()) / 2, (screenHeight - draw->getMainWindowHeight()) / 2, 
                                  draw->getMainWindowWidth(), draw->getMainWindowHeight(), NULL, NULL, hInst, NULL);

        checkIsWindowCreated(mainWindow);

        ShowWindow(mainWindow, nCommandShow);

        while (GetMessage(&uMsg, NULL, NULL, NULL)) {
            TranslateMessage(&uMsg);
            DispatchMessage(&uMsg);
        }
        return uMsg.wParam;
    }
    return 0;
}