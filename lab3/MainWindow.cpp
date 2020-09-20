
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

int sizeOfInt(int value) {
    int size = 1;
    value = abs(value);
    while (value > 0) {
        size++;
        value /= 10;
    }
    return size;
}

wchar_t* longToLPWSTR(long value) {
    int size = sizeOfInt(value);
    wchar_t* converted = new wchar_t[size];
    _ltow_s(value, converted, sizeof(converted), 10);
    return converted;
}

LRESULT CALLBACK edit1Proc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam) {
    wchar_t value[100000];
    switch (msg) {
        case WM_KEYDOWN:
            if ((wParam >= '0' && wParam <= '9') || wParam == VK_RETURN || wParam == VK_DELETE || wParam == VK_BACK)
                break;
            else if (wParam == '-' && !hasMinus) {
                hasMinus = true;
            }
            else if (wParam == '.' && !hasDot) {
                hasDot = true;
            }
            else {
                if (((wParam == '-' && hasMinus) || (wParam == '.' && hasDot) || wParam < '0' || wParam>'9')) {
                    SendMessage(hEdit, EM_UNDO, wParam, lParam);
                    return TRUE;
                }
            }
        break;
        case WM_CHAR:
            if ((wParam >= '0' && wParam <= '9') || wParam == VK_RETURN || wParam == VK_DELETE || wParam == VK_BACK)
                break;
            else if (wParam == '-' && !hasMinus) {
                hasMinus = true;
            }
            else if (wParam == '.' && !hasDot) {
                hasDot = true;
            }
            else {
                if (((wParam == '-' && hasMinus) || (wParam == '.' && hasDot) || wParam < '0' || wParam>'9')
                    && (wParam != VK_RETURN || wParam != VK_DELETE || wParam != VK_BACK)) {
                    SendMessage(hEdit, EM_UNDO, wParam, lParam);
                    return TRUE;
                }
            }
            break;
    }
    return CallWindowProc(oldEdit1Proc, hEdit, msg, wParam, lParam);
}

LRESULT CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    wchar_t xmin[100000], xmax[100000];
    BOOL* isNumber = FALSE;
    UINT res;
    switch (message) {
        case WM_INITDIALOG:
            oldEdit1Proc = (WNDPROC)SetWindowLong(GetDlgItem(hDlg, IDC_EDIT1),GWL_WNDPROC, (LONG)edit1Proc);
            //oldEdit2Proc = (WNDPROC)SetWindowLong(GetDlgItem(hDlg, IDC_EDIT2),GWL_WNDPROC, (LONG)edit2Proc);
            SetDlgItemText(hDlg, IDC_EDIT1, longToLPWSTR(draw->getXmin()));
            SetDlgItemText(hDlg, IDC_EDIT2, longToLPWSTR(draw->getXmax()));
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    GetDlgItemText(hDlg, IDC_EDIT1, xmin, sizeof(xmin));
                    GetDlgItemText(hDlg, IDC_EDIT2, xmax, sizeof(xmax));
                    draw->setXmin(_wtol(xmin));
                    draw->setXmax(_wtol(xmax));
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
        case WM_CREATE:

            break;
        case WM_PAINT:
            
            draw->beginPaint(hWnd);
             
            draw->calculateWorkRect();

            draw->drawX();
            draw->drawY();

            draw->setStepX();

            draw->calculatePoints();

            draw->drawGraph();

            draw->endPaint(hWnd);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_40001:
                    if (DialogBox(NULL, (LPCTSTR)IDD_DIALOG1, hWnd, (DLGPROC)DlgProc) == IDOK) {
                        InvalidateRect(hWnd, NULL, TRUE);
                        SendMessage(hWnd, WM_PAINT, wParam, lParam);
                    }
                    break;
            }
            break;
        case WM_LBUTTONDOWN:
            if (DialogBox(NULL, (LPCTSTR)IDD_DIALOG1, hWnd, (DLGPROC)DlgProc) == IDOK) {
                InvalidateRect(hWnd, NULL, TRUE);
                SendMessage(hWnd, WM_PAINT, wParam, lParam);
            }
            break;
        case WM_SIZE:
            GetClientRect(hWnd, &rt);
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
    HWND dialogWindow;

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