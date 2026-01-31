/**
 * drawing_overlay.c - 그리기 오버레이 윈도우 구현
 */

#include "drawing_overlay.h"
#include <gdiplus.h>
#include <stdio.h>

// GDI+ 네임스페이스 (C에서는 접두사 사용)
#pragma comment(lib, "gdiplus.lib")

// 윈도우 클래스 이름
#define DRAW_CLASS_NAME L"LetsZoomDrawingWindow"

// 스트로크 포인트 구조체
typedef struct {
    int x;
    int y;
} StrokePoint;

// 스트로크 구조체
typedef struct {
    StrokePoint* points;
    int pointCount;
    int capacity;
    COLORREF color;
    int width;
    int opacity;
} Stroke;

// 전역 변수
static HINSTANCE g_hInstance = NULL;
static HWND g_hwndDraw = NULL;
static bool g_bActive = false;

// GDI+ 변수
static ULONG_PTR g_gdiplusToken = 0;

// 펜 설정
static COLORREF g_penColor = RGB(255, 0, 0);
static int g_penWidth = 3;
static int g_penOpacity = 255;

// 스트로크 배열
static Stroke* g_strokes = NULL;
static int g_strokeCount = 0;
static int g_strokeCapacity = 0;

// 현재 그리기 중인 스트로크
static Stroke* g_currentStroke = NULL;
static bool g_bDrawing = false;

// 화면 크기
static int g_screenWidth = 0;
static int g_screenHeight = 0;

// 렌더링용 메모리 DC
static HDC g_hdcMem = NULL;
static HBITMAP g_hbmMem = NULL;
static HBITMAP g_hbmOld = NULL;

/**
 * 스트로크 생성
 */
static Stroke* CreateStroke(COLORREF color, int width, int opacity)
{
    Stroke* stroke = (Stroke*)malloc(sizeof(Stroke));
    if (!stroke) return NULL;

    stroke->capacity = 100;
    stroke->points = (StrokePoint*)malloc(sizeof(StrokePoint) * stroke->capacity);
    if (!stroke->points) {
        free(stroke);
        return NULL;
    }

    stroke->pointCount = 0;
    stroke->color = color;
    stroke->width = width;
    stroke->opacity = opacity;

    return stroke;
}

/**
 * 스트로크에 포인트 추가
 */
static void AddPointToStroke(Stroke* stroke, int x, int y)
{
    if (!stroke) return;

    if (stroke->pointCount >= stroke->capacity) {
        // 용량 확장
        int newCapacity = stroke->capacity * 2;
        StrokePoint* newPoints = (StrokePoint*)realloc(stroke->points, sizeof(StrokePoint) * newCapacity);
        if (!newPoints) return;

        stroke->points = newPoints;
        stroke->capacity = newCapacity;
    }

    stroke->points[stroke->pointCount].x = x;
    stroke->points[stroke->pointCount].y = y;
    stroke->pointCount++;
}

/**
 * 스트로크 파괴
 */
static void DestroyStroke(Stroke* stroke)
{
    if (!stroke) return;

    if (stroke->points) {
        free(stroke->points);
    }
    free(stroke);
}

/**
 * 모든 스트로크 파괴
 */
static void ClearAllStrokes(void)
{
    for (int i = 0; i < g_strokeCount; i++) {
        DestroyStroke(g_strokes[i]);
    }

    if (g_strokes) {
        free(g_strokes);
        g_strokes = NULL;
    }

    g_strokeCount = 0;
    g_strokeCapacity = 0;
}

/**
 * 스트로크 배열에 추가
 */
static void AddStroke(Stroke* stroke)
{
    if (!stroke) return;

    if (g_strokeCount >= g_strokeCapacity) {
        int newCapacity = g_strokeCapacity == 0 ? 10 : g_strokeCapacity * 2;
        Stroke** newStrokes = (Stroke**)realloc(g_strokes, sizeof(Stroke*) * newCapacity);
        if (!newStrokes) return;

        g_strokes = newStrokes;
        g_strokeCapacity = newCapacity;
    }

    g_strokes[g_strokeCount++] = stroke;
}

/**
 * 렌더링 리소스 생성
 */
static bool CreateRenderResources(void)
{
    // 화면 크기
    g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
    g_screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // 메모리 DC 생성
    HDC hdcScreen = GetDC(NULL);
    g_hdcMem = CreateCompatibleDC(hdcScreen);
    g_hbmMem = CreateCompatibleBitmap(hdcScreen, g_screenWidth, g_screenHeight);
    g_hbmOld = (HBITMAP)SelectObject(g_hdcMem, g_hbmMem);
    ReleaseDC(NULL, hdcScreen);

    // 초기 배경 (투명)
    RECT rect = {0, 0, g_screenWidth, g_screenHeight};
    FillRect(g_hdcMem, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    return true;
}

/**
 * 렌더링 리소스 해제
 */
static void DestroyRenderResources(void)
{
    if (g_hbmOld) {
        SelectObject(g_hdcMem, g_hbmOld);
        g_hbmOld = NULL;
    }

    if (g_hbmMem) {
        DeleteObject(g_hbmMem);
        g_hbmMem = NULL;
    }

    if (g_hdcMem) {
        DeleteDC(g_hdcMem);
        g_hdcMem = NULL;
    }
}

/**
 * 스트로크 렌더링 (GDI+)
 */
static void RenderStroke(HDC hdc, const Stroke* stroke)
{
    if (!stroke || stroke->pointCount < 2) return;

    // GDI+ Graphics 객체 생성
    GpGraphics* graphics = NULL;
    GdipCreateFromHDC(hdc, &graphics);
    if (!graphics) return;

    // 안티앨리어싱 활성화
    GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);

    // GDI+ Pen 생성
    GpPen* pen = NULL;
    ARGB color = (stroke->opacity << 24) | (GetRValue(stroke->color) << 16) |
                 (GetGValue(stroke->color) << 8) | GetBValue(stroke->color);
    GdipCreatePen1(color, (REAL)stroke->width, UnitPixel, &pen);

    // 라인 캡 설정 (둥근 끝)
    GdipSetPenLineCap(pen, LineCapRound, LineCapRound, DashCapRound);

    // 스트로크 그리기
    for (int i = 0; i < stroke->pointCount - 1; i++) {
        GdipDrawLine(graphics, pen,
            (REAL)stroke->points[i].x, (REAL)stroke->points[i].y,
            (REAL)stroke->points[i + 1].x, (REAL)stroke->points[i + 1].y);
    }

    // 정리
    GdipDeletePen(pen);
    GdipDeleteGraphics(graphics);
}

/**
 * 모든 스트로크 렌더링
 */
static void RenderAllStrokes(HDC hdc)
{
    // 배경 지우기 (투명)
    RECT rect = {0, 0, g_screenWidth, g_screenHeight};
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // 모든 스트로크 렌더링
    for (int i = 0; i < g_strokeCount; i++) {
        RenderStroke(hdc, g_strokes[i]);
    }

    // 현재 그리기 중인 스트로크
    if (g_bDrawing && g_currentStroke) {
        RenderStroke(hdc, g_currentStroke);
    }

    // UI 오버레이 (힌트)
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    WCHAR hintText[] = L"ESC: 종료 | C: 지우기 | 마우스: 그리기";
    RECT hintRect = {20, g_screenHeight - 60, g_screenWidth - 20, g_screenHeight - 20};
    DrawTextW(hdc, hintText, -1, &hintRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

/**
 * 그리기 윈도우 프로시저
 */
static LRESULT CALLBACK DrawWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CREATE:
            OutputDebugStringW(L"[DrawingOverlay] Window created\n");
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // 메모리 DC에 렌더링
            RenderAllStrokes(g_hdcMem);

            // 레이어드 윈도우 업데이트
            POINT ptSrc = {0, 0};
            SIZE sizeWnd = {g_screenWidth, g_screenHeight};
            BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, 0};

            UpdateLayeredWindow(hwnd, hdc, NULL, &sizeWnd, g_hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            // 그리기 시작
            g_bDrawing = true;
            g_currentStroke = CreateStroke(g_penColor, g_penWidth, g_penOpacity);

            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            AddPointToStroke(g_currentStroke, x, y);

            SetCapture(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (g_bDrawing && g_currentStroke) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                AddPointToStroke(g_currentStroke, x, y);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_bDrawing) {
                // 스트로크 완료
                AddStroke(g_currentStroke);
                g_currentStroke = NULL;
                g_bDrawing = false;
                ReleaseCapture();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    OutputDebugStringW(L"[DrawingOverlay] ESC pressed, hiding draw mode\n");
                    DrawingOverlay_Hide();
                    break;

                case 'C':
                    // 화면 지우기
                    DrawingOverlay_Clear();
                    break;
            }
            return 0;

        case WM_DESTROY:
            OutputDebugStringW(L"[DrawingOverlay] Window destroyed\n");
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

/**
 * 그리기 윈도우 클래스 등록
 */
static bool RegisterDrawWindow(HINSTANCE hInstance)
{
    WNDCLASSEXW wc = {0};

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DrawWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = DRAW_CLASS_NAME;
    wc.hIconSm = NULL;

    if (!RegisterClassExW(&wc)) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            OutputDebugStringW(L"[DrawingOverlay] Failed to register window class\n");
            return false;
        }
    }

    OutputDebugStringW(L"[DrawingOverlay] Window class registered\n");
    return true;
}

/**
 * 그리기 오버레이 초기화
 */
bool DrawingOverlay_Initialize(HINSTANCE hInstance)
{
    if (g_hInstance) {
        return true;  // 이미 초기화됨
    }

    g_hInstance = hInstance;

    // GDI+ 초기화
    GdiplusStartupInput gdiplusStartupInput;
    gdiplusStartupInput.GdiplusVersion = 1;
    gdiplusStartupInput.DebugEventCallback = NULL;
    gdiplusStartupInput.SuppressBackgroundThread = FALSE;
    gdiplusStartupInput.SuppressExternalCodecs = FALSE;

    if (GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL) != Ok) {
        OutputDebugStringW(L"[DrawingOverlay] Failed to initialize GDI+\n");
        g_hInstance = NULL;
        return false;
    }

    if (!RegisterDrawWindow(hInstance)) {
        GdiplusShutdown(g_gdiplusToken);
        g_hInstance = NULL;
        return false;
    }

    OutputDebugStringW(L"[DrawingOverlay] Initialized\n");
    return true;
}

/**
 * 그리기 오버레이 정리
 */
void DrawingOverlay_Shutdown(void)
{
    if (g_bActive) {
        DrawingOverlay_Hide();
    }

    ClearAllStrokes();

    if (g_hInstance) {
        UnregisterClassW(DRAW_CLASS_NAME, g_hInstance);
        GdiplusShutdown(g_gdiplusToken);
        g_hInstance = NULL;
    }

    OutputDebugStringW(L"[DrawingOverlay] Shutdown\n");
}

/**
 * 그리기 모드 시작
 */
bool DrawingOverlay_Show(COLORREF penColor, int penWidth, int penOpacity)
{
    if (g_bActive) {
        return true;  // 이미 활성화됨
    }

    OutputDebugStringW(L"[DrawingOverlay] Showing drawing overlay\n");

    g_penColor = penColor;
    g_penWidth = penWidth;
    g_penOpacity = penOpacity;

    // 렌더링 리소스 생성
    if (!CreateRenderResources()) {
        return false;
    }

    // 투명 레이어드 윈도우 생성
    g_hwndDraw = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        DRAW_CLASS_NAME,
        L"LetsZoom Drawing",
        WS_POPUP,
        0, 0,
        g_screenWidth, g_screenHeight,
        NULL, NULL,
        g_hInstance,
        NULL
    );

    if (!g_hwndDraw) {
        OutputDebugStringW(L"[DrawingOverlay] Failed to create window\n");
        DestroyRenderResources();
        return false;
    }

    // 레이어드 윈도우 속성 설정
    SetLayeredWindowAttributes(g_hwndDraw, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // 마우스 투과 해제 (그리기를 위해)
    LONG_PTR style = GetWindowLongPtr(g_hwndDraw, GWL_EXSTYLE);
    style &= ~WS_EX_TRANSPARENT;
    SetWindowLongPtr(g_hwndDraw, GWL_EXSTYLE, style);

    // 윈도우 표시
    ShowWindow(g_hwndDraw, SW_SHOW);
    UpdateWindow(g_hwndDraw);
    SetForegroundWindow(g_hwndDraw);

    g_bActive = true;

    OutputDebugStringW(L"[DrawingOverlay] Drawing overlay shown\n");
    return true;
}

/**
 * 그리기 모드 종료
 */
void DrawingOverlay_Hide(void)
{
    if (!g_bActive) {
        return;
    }

    OutputDebugStringW(L"[DrawingOverlay] Hiding drawing overlay\n");

    // 윈도우 파괴
    if (g_hwndDraw) {
        DestroyWindow(g_hwndDraw);
        g_hwndDraw = NULL;
    }

    // 렌더링 리소스 해제
    DestroyRenderResources();

    // 스트로크 지우기
    ClearAllStrokes();

    g_bActive = false;
    g_bDrawing = false;
    g_currentStroke = NULL;

    OutputDebugStringW(L"[DrawingOverlay] Drawing overlay hidden\n");
}

/**
 * 그리기 모드 활성 상태 확인
 */
bool DrawingOverlay_IsActive(void)
{
    return g_bActive;
}

/**
 * 화면 지우기
 */
void DrawingOverlay_Clear(void)
{
    OutputDebugStringW(L"[DrawingOverlay] Clearing all strokes\n");

    ClearAllStrokes();

    if (g_hwndDraw) {
        InvalidateRect(g_hwndDraw, NULL, FALSE);
    }
}
