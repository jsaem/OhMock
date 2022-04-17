// OhMock.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "OhMock.h"

#define MAX_LOADSTRING 100
#define X_COUNT 20
#define Y_COUNT 20
#define START_X 45
#define START_Y 65
#define INTERVAL 38
#define HALF_INTERVAL INTERVAL/2
#define XPOS(x) (START_X + (x) * INTERVAL)
#define YPOS(y) (START_Y+ (y) * INTERVAL)

/*
* 돌의 지름: 
*/

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
char state[Y_COUNT][X_COUNT]; 
char step;
TCHAR whichStone[2][128] = { NULL };

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OHMOCK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OHMOCK));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OHMOCK));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(70, 140, 255));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OHMOCK);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 900, 900, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

HFONT hMyFont = NULL;
TCHAR g_TpStr[128] = { _T('\0') };

HPEN MyPen = NULL;
HBRUSH YellowBrush = NULL;
HBRUSH BlackBS = NULL;
HBRUSH WhiteBS = NULL;

HPEN NULL_Pen = NULL;
HBRUSH NULL_Brush = NULL;

COLORREF WhichColor[2] = { NULL };
int bStone = 0;
int wStone = 0;

void Clear(HWND hWnd);    //초기화 함수 선언
void Judgment(HWND hWnd); //판정 함수 선언

void InitGame()
{
    hMyFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
        VARIABLE_PITCH | FF_ROMAN, _T("굴림"));

    MyPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0)); 
    YellowBrush = CreateSolidBrush(RGB(220, 179, 92));

    BlackBS = CreateSolidBrush(RGB(0, 0, 0));
    WhiteBS = CreateSolidBrush(RGB(255, 255, 255));

    NULL_Pen = (HPEN)GetStockObject(NULL_PEN); // 투명펜
    NULL_Brush = (HBRUSH)GetStockObject(NULL_BRUSH); // 투명브러쉬

    _tcscpy_s(whichStone[0], _T("흑돌"));
    _tcscpy_s(whichStone[1], _T("백돌"));

    WhichColor[0] = RGB(0, 0, 0);
    WhichColor[1] = RGB(255, 255, 255);
}

void DrawGame(HWND hWnd, HDC a_hDC)
{
    /*
    RECT a_Rect;
    GetClientRect(hWnd, &a_Rect);
    TCHAR a_str[128];
    _stprintf_s(a_str, _T("현재 윈도우 DC 사이즈 -> %d : %d"),
        a_Rect.right, a_Rect.bottom);
    TextOut(a_hDC, 10, 10, a_str, _tcslen(a_str));
    */

    // --- 바둑판 그리기
    SelectObject(a_hDC, YellowBrush);
    Rectangle(a_hDC, 30, 50, 780, 800);

    HPEN OldPen = (HPEN)SelectObject(a_hDC, MyPen);
    HBRUSH OldBrush = (HBRUSH)SelectObject(a_hDC, YellowBrush);
    SelectObject(a_hDC, OldPen);
    SelectObject(a_hDC, OldBrush);

    // 그리드
    for (int y = 0; y < Y_COUNT; y++)
    {
        for (int x = 0; x < X_COUNT; x++)
        {
            MoveToEx(a_hDC, XPOS(x), YPOS(0), NULL);
            LineTo(a_hDC, XPOS(x), YPOS(Y_COUNT - 1));
            MoveToEx(a_hDC, XPOS(0), YPOS(y), NULL);
            LineTo(a_hDC, XPOS(X_COUNT - 1), YPOS(y));
        }
    }
    // --- 바둑판 그리기

    // --- 돌 놓기
    for (int y = 0; y < Y_COUNT; y++)
    {
        for (int x = 0; x < X_COUNT; x++)
        {
            if (state[y][x] > 0)
            {
                if (state[y][x] == 1)
                    // 흑돌
                    SelectObject(a_hDC, GetStockObject(BLACK_BRUSH));
                else 
                    // 백돌
                    SelectObject(a_hDC, GetStockObject(WHITE_BRUSH));
                Ellipse(a_hDC, XPOS(x) - HALF_INTERVAL, YPOS(y) - HALF_INTERVAL,
                    XPOS(x) + HALF_INTERVAL, YPOS(y) + HALF_INTERVAL);
            }
        }
    }
    // --- 돌 놓기


    HFONT OldFont = (HFONT)SelectObject(a_hDC, hMyFont); // <-- 내가 만들어놓은 폰트 적용
    SetBkMode(a_hDC, TRANSPARENT); // <-- 글씨 배경을 빼는 방법
    TCHAR a_TempStr[128];

    SetTextColor(a_hDC, WhichColor[step]);
    _stprintf_s(a_TempStr, _T("%s 차례입니다."), whichStone[step]);
    TextOut(a_hDC, 30, 10, a_TempStr, _tcslen(a_TempStr));
    
    SetTextColor(a_hDC, RGB(255, 255, 0)); // <-- 글씨 색 바꾸기
    _stprintf_s(a_TempStr, _T("흑돌(%d : %d)백돌"), bStone, wStone);
    TextOut(a_hDC, 300, 10, a_TempStr, _tcslen(a_TempStr));
}

void EndGame()
{

    if (hMyFont != NULL)
    {
        DeleteObject(hMyFont);
        hMyFont = NULL;
    }

    if (YellowBrush != NULL)
    {
        DeleteObject(YellowBrush);
        YellowBrush = NULL;
    }

    if (BlackBS != NULL)
    {
        DeleteObject(BlackBS);
        BlackBS = NULL;
    }

    if (WhiteBS != NULL)
    {
        DeleteObject(WhiteBS);
        WhiteBS = NULL;
    }

    if (NULL_Pen != NULL)
    {
        DeleteObject(NULL_Pen);
        NULL_Pen = NULL;
    }

    if (NULL_Brush != NULL)
    {
        DeleteObject(NULL_Brush);
        NULL_Brush = NULL;
    }
}

bool isWin(int x, int y) // 이겼으면 true, 졌으면 false
{
    int count = 1;
    // 오른쪽 방향
    for (int i = x + 1; i < 19; i++)
    {
        if (state[x][y] != -1)
        {
            if (state[i][y] == state[x][y])
                count++;
            else
                break;
        }
    }
    // 왼쪽 방향
    for (int i = x + 1; i < 19; i++)
    {
        if (state[x][y] != -1)
        {
            if (state[i][y] == state[x][y])
                count++;
            else
                break;
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        InitGame();
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        int a_x = LOWORD(lParam);
        int a_y = HIWORD(lParam);

        if (a_x > (XPOS(0) - HALF_INTERVAL)
            && a_y > (YPOS(0) - HALF_INTERVAL)
            && a_x < (XPOS(X_COUNT -1) + HALF_INTERVAL)
            && a_y < (YPOS(Y_COUNT -1) + HALF_INTERVAL))
        {
            int x = (a_x - START_X + HALF_INTERVAL) / INTERVAL;
            int y = (a_y - START_Y + HALF_INTERVAL) / INTERVAL;
            if (state[y][x] == 0)
            {
                state[y][x] = step + 1;
                step = !step;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }

        /*
        if (MessageBox(NULL, _T("흑돌이 승리하였습니다."), _T("승리"), MB_OK) == IDOK)
        {
            _stprintf_s(g_TpStr, _T("메세지 박스 정상 종료 -> %d"), 77777);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        */
    }
    break;
    case WM_LBUTTONUP:
        Judgment(hWnd);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        DrawGame(hWnd, hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        EndGame();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void Clear(HWND hWnd)
{
    for (int ii = 0; ii < 19; ii++)
    {
        for (int aa = 0; aa < 19; aa++)
        {
            g_GridType[ii][aa].m_Type = 0;
        }
    }
    InvalidateRect(hWnd, NULL, TRUE);
}
void Judgment(HWND hWnd)
{
    for (int yy = 0; yy < 19; yy++)
    {
        for (int xx = 0; xx < 19; xx++)
        {
            if ((xx + 1 < 19) && (xx + 2 < 19) && (xx + 3 < 19) && (xx + 4 < 19))
                if ((g_GridType[xx][yy].m_Type == 1) &&
                    (g_GridType[xx + 1][yy].m_Type == 1) &&
                    (g_GridType[xx + 2][yy].m_Type == 1) &&
                    (g_GridType[xx + 3][yy].m_Type == 1) &&
                    (g_GridType[xx + 4][yy].m_Type == 1))
                {
                    g_BlackWin++;
                    TCHAR winner[15];
                    _stprintf_s(winner, _T("흑돌이 승리하였습니다."));
                    MessageBoxW(hWnd, winner, _T("승리"), MB_OK);
                    Clear(hWnd);
                }

            if ((yy + 1 < 19) && (yy + 2 < 19) && (yy + 3 < 19) && (yy + 4 < 19))
                if ((g_GridType[xx][yy].m_Type == 1) &&
                    (g_GridType[xx][yy + 1].m_Type == 1) &&
                    (g_GridType[xx][yy + 2].m_Type == 1) &&
                    (g_GridType[xx][yy + 3].m_Type == 1) &&
                    (g_GridType[xx][yy + 4].m_Type == 1))
                {
                    g_BlackWin++;
                    TCHAR winner[15];
                    _stprintf_s(winner, _T("흑돌이 승리하였습니다."));
                    MessageBoxW(hWnd, winner, _T("승리"), MB_OK);
                    Clear(hWnd);
                }

            if ((xx + 1 < 19) && (xx + 2 < 19) && (xx + 3 < 19) && (xx + 4 < 19))
                if ((yy + 1 < 19) && (yy + 2 < 19) && (yy + 3 < 19) && (yy + 4 < 19))
                    if ((g_GridType[xx][yy].m_Type == 1) &&
                        (g_GridType[xx + 1][yy + 1].m_Type == 1) &&
                        (g_GridType[xx + 2][yy + 2].m_Type == 1) &&
                        (g_GridType[xx + 3][yy + 3].m_Type == 1) &&
                        (g_GridType[xx + 4][yy + 4].m_Type == 1))
                    {
                        g_BlackWin++;
                        TCHAR winner[15];
                        _stprintf_s(winner, _T("흑돌이 승리하였습니다."));
                        MessageBoxW(hWnd, winner, _T("승리"), MB_OK);
                        Clear(hWnd);
                    }

            if ((0 <= xx - 1) && (0 <= xx - 2) && (0 <= xx - 3) && (0 <= xx - 4))
                if ((yy + 1 < 19) && (yy + 2 < 19) && (yy + 3 < 19) && (yy + 4 < 19))
                    if ((g_GridType[xx][yy].m_Type == 1) &&
                        (g_GridType[xx - 1][yy + 1].m_Type == 1) &&
                        (g_GridType[xx - 2][yy + 2].m_Type == 1) &&
                        (g_GridType[xx - 3][yy + 3].m_Type == 1) &&
                        (g_GridType[xx - 4][yy + 4].m_Type == 1))
                    {
                        g_BlackWin++;
                        TCHAR winner[15];
                        _stprintf_s(winner, _T("흑돌이 승리하였습니다."));
                        MessageBoxW(hWnd, winner, _T("승리"), MB_OK);
                        Clear(hWnd);
                    }

            if ((xx + 1 < 19) && (xx + 2 < 19) && (xx + 3 < 19) && (xx + 4 < 19))
                if ((g_GridType[xx][yy].m_Type == 2) &&
                    (g_GridType[xx + 1][yy].m_Type == 2) &&
                    (g_GridType[xx + 2][yy].m_Type == 2) &&
                    (g_GridType[xx + 3][yy].m_Type == 2) &&
                    (g_GridType[xx + 4][yy].m_Type == 2))
                {
                    g_WhiteWin++;
                    TCHAR winner[15];
                    _stprintf_s(winner, _T("백돌이 승리하였습니다."));
                    MessageBoxW(hWnd, winner, _T("승리"), MB_OK);
                    Clear(hWnd);
                }

            if ((yy + 1 < 19) && (yy + 2 < 19) && (yy + 3 < 19) && (yy + 4 < 19))
                if ((g_GridType[xx][yy].m_Type == 2) &&
                    (g_GridType[xx][yy + 1].m_Type == 2) &&
                    (g_GridType[xx][yy + 2].m_Type == 2) &&
                    (g_GridType[xx][yy + 3].m_Type == 2) &&
                    (g_GridType[xx][yy + 4].m_Type == 2))
                {
                    g_WhiteWin++;
                    TCHAR winner[15];
                    _stprintf_s(winner, _T("백돌이 승리하였습니다."));
                    MessageBoxW(hWnd, winner, _T("승리"), MB_OK);
                    Clear(hWnd);
                }
            if ((xx + 1 < 19) && (xx + 2 < 19) && (xx + 3 < 19) && (xx + 4 < 19))
                if ((yy + 1 < 19) && (yy + 2 < 19) && (yy + 3 < 19) && (yy + 4 < 19))
                    if ((g_GridType[xx][yy].m_Type == 2) &&
                        (g_GridType[xx + 1][yy + 1].m_Type == 2) &&
                        (g_GridType[xx + 2][yy + 2].m_Type == 2) &&
                        (g_GridType[xx + 3][yy + 3].m_Type == 2) &&
                        (g_GridType[xx + 4][yy + 4].m_Type == 2))
                    {
                        g_WhiteWin++;
                        TCHAR winner[15];
                        _stprintf_s(winner, _T("백돌이 승리하였습니다."));
                        MessageBoxW(hWnd, winner, _T("승리"), MB_OK);
                        Clear(hWnd);
                    }

            if ((0 <= xx - 1) && (0 <= xx - 2) && (0 <= xx - 3) && (0 <= xx - 4))
                if ((yy + 1 < 19) && (yy + 2 < 19) && (yy + 3 < 19) && (yy + 4 < 19))
                    if ((g_GridType[xx][yy].m_Type == 2) &&
                        (g_GridType[xx - 1][yy + 1].m_Type == 2) &&
                        (g_GridType[xx - 2][yy + 2].m_Type == 2) &&
                        (g_GridType[xx - 3][yy + 3].m_Type == 2) &&
                        (g_GridType[xx - 4][yy + 4].m_Type == 2))
                    {
                        g_WhiteWin++;
                        TCHAR winner[15];
                        _stprintf_s(winner, _T("백돌이 승리하였습니다."));
                        MessageBoxW(hWnd, winner, _T("승리"), MB_OK);
                        Clear(hWnd);
                    }
        }
    }
}
