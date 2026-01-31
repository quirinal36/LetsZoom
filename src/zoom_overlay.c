/**
 * zoom_overlay.c - 화면 확대 오버레이 윈도우 구현
 */

#include "zoom_overlay.h"
#include <stdio.h>

// 윈도우 클래스 이름
#define ZOOM_CLASS_NAME L"LetsZoomOverlayWindow"

// 전역 변수
static HINSTANCE g_hInstance = NULL;
static HWND g_hwndZoom = NULL;
static bool g_bActive = false;
static int g_zoomLevel = 200;
static bool g_smoothZoom = true;

// 패닝 관련
static int g_panOffsetX = 0;
static int g_panOffsetY = 0;
static bool g_bDragging = false;
static POINT g_ptDragStart = {0};
static POINT g_ptPanStart = {0};

// 렌더링 관련
static HDC g_hdcScreen = NULL;
static HDC g_hdcMem = NULL;
static HBITMAP g_hbmScreen = NULL;
static HBITMAP g_hbmOld = NULL;
static int g_screenWidth = 0;
static int g_screenHeight = 0;

// 타이머 ID
#define TIMER_UPDATE 1
#define UPDATE_INTERVAL 16  // ~60 FPS

/**
 * 렌더링 리소스 생성
 */
static bool CreateRenderResources(void)
{
    // 화면 크기 가져오기
    g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
    g_screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // 화면 DC 가져오기
    g_hdcScreen = GetDC(NULL);
    if (!g_hdcScreen) {
        OutputDebugStringW(L"[ZoomOverlay] Failed to get screen DC\n");
        return false;
    }

    // 메모리 DC 생성
    g_hdcMem = CreateCompatibleDC(g_hdcScreen);
    if (!g_hdcMem) {
        OutputDebugStringW(L"[ZoomOverlay] Failed to create memory DC\n");
        ReleaseDC(NULL, g_hdcScreen);
        g_hdcScreen = NULL;
        return false;
    }

    // 비트맵 생성
    g_hbmScreen = CreateCompatibleBitmap(g_hdcScreen, g_screenWidth, g_screenHeight);
    if (!g_hbmScreen) {
        OutputDebugStringW(L"[ZoomOverlay] Failed to create bitmap\n");
        DeleteDC(g_hdcMem);
        ReleaseDC(NULL, g_hdcScreen);
        g_hdcMem = NULL;
        g_hdcScreen = NULL;
        return false;
    }

    g_hbmOld = (HBITMAP)SelectObject(g_hdcMem, g_hbmScreen);

    OutputDebugStringW(L"[ZoomOverlay] Render resources created\n");
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

    if (g_hbmScreen) {
        DeleteObject(g_hbmScreen);
        g_hbmScreen = NULL;
    }

    if (g_hdcMem) {
        DeleteDC(g_hdcMem);
        g_hdcMem = NULL;
    }

    if (g_hdcScreen) {
        ReleaseDC(NULL, g_hdcScreen);
        g_hdcScreen = NULL;
    }

    OutputDebugStringW(L"[ZoomOverlay] Render resources destroyed\n");
}

/**
 * 화면 확대 렌더링
 */
static void RenderZoom(HDC hdc)
{
    POINT cursor;
    GetCursorPos(&cursor);

    // 확대 배율 계산
    float zoom = (float)g_zoomLevel / 100.0f;

    // 캡처할 영역 크기 (확대 후 화면 전체를 채우도록)
    int captureWidth = (int)(g_screenWidth / zoom);
    int captureHeight = (int)(g_screenHeight / zoom);

    // 캡처 영역의 중심을 커서 위치 + 패닝 오프셋으로
    int captureX = cursor.x - captureWidth / 2 + g_panOffsetX;
    int captureY = cursor.y - captureHeight / 2 + g_panOffsetY;

    // 화면 경계 체크
    if (captureX < 0) captureX = 0;
    if (captureY < 0) captureY = 0;
    if (captureX + captureWidth > g_screenWidth) {
        captureX = g_screenWidth - captureWidth;
    }
    if (captureY + captureHeight > g_screenHeight) {
        captureY = g_screenHeight - captureHeight;
    }

    // 화면 캡처
    BitBlt(g_hdcMem, 0, 0, g_screenWidth, g_screenHeight, g_hdcScreen, 0, 0, SRCCOPY);

    // 확대하여 그리기
    if (g_smoothZoom) {
        // 부드러운 확대 (HALFTONE 모드)
        int oldMode = SetStretchBltMode(hdc, HALFTONE);
        SetBrushOrgEx(hdc, 0, 0, NULL);
        StretchBlt(
            hdc, 0, 0, g_screenWidth, g_screenHeight,
            g_hdcMem, captureX, captureY, captureWidth, captureHeight,
            SRCCOPY
        );
        SetStretchBltMode(hdc, oldMode);
    } else {
        // 빠른 확대 (COLORONCOLOR 모드)
        SetStretchBltMode(hdc, COLORONCOLOR);
        StretchBlt(
            hdc, 0, 0, g_screenWidth, g_screenHeight,
            g_hdcMem, captureX, captureY, captureWidth, captureHeight,
            SRCCOPY
        );
    }

    // 십자선 그리기 (선택 사항)
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    int centerX = g_screenWidth / 2;
    int centerY = g_screenHeight / 2;
    int crossSize = 20;

    MoveToEx(hdc, centerX - crossSize, centerY, NULL);
    LineTo(hdc, centerX + crossSize, centerY);
    MoveToEx(hdc, centerX, centerY - crossSize, NULL);
    LineTo(hdc, centerX, centerY + crossSize);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // UI 오버레이 그리기 (줌 레벨 표시)
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    WCHAR zoomText[64];
    swprintf_s(zoomText, 64, L"확대: %d%%", g_zoomLevel);

    RECT textRect = {20, 20, 300, 60};

    // 반투명 배경
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hTransPen = CreatePen(PS_NULL, 0, 0);
    HPEN hOldTransPen = (HPEN)SelectObject(hdc, hTransPen);

    Rectangle(hdc, textRect.left, textRect.top, textRect.right, textRect.bottom);

    SelectObject(hdc, hOldTransPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hTransPen);
    DeleteObject(hBrush);

    // 텍스트
    DrawTextW(hdc, zoomText, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ESC 힌트
    WCHAR hintText[] = L"ESC: 종료 | 휠: 줌 | 드래그: 이동 | +/-: 줌 조절";
    RECT hintRect = {20, g_screenHeight - 60, g_screenWidth - 20, g_screenHeight - 20};
    DrawTextW(hdc, hintText, -1, &hintRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

/**
 * 확대 윈도우 프로시저
 */
static LRESULT CALLBACK ZoomWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CREATE:
            OutputDebugStringW(L"[ZoomOverlay] Window created\n");
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RenderZoom(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_TIMER:
            if (wParam == TIMER_UPDATE) {
                // 강제 다시 그리기
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    // ESC 키로 종료
                    OutputDebugStringW(L"[ZoomOverlay] ESC pressed, hiding zoom\n");
                    ZoomOverlay_Hide();
                    break;

                case VK_ADD:
                case VK_OEM_PLUS:
                case 187:  // '=' key
                    // + 키로 확대
                    g_zoomLevel += 25;
                    if (g_zoomLevel > 2000) g_zoomLevel = 2000;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;

                case VK_SUBTRACT:
                case VK_OEM_MINUS:
                case 189:  // '-' key
                    // - 키로 축소
                    g_zoomLevel -= 25;
                    if (g_zoomLevel < 100) g_zoomLevel = 100;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;

                case VK_LEFT:
                    // 왼쪽 화살표
                    g_panOffsetX -= 50;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;

                case VK_RIGHT:
                    // 오른쪽 화살표
                    g_panOffsetX += 50;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;

                case VK_UP:
                    // 위쪽 화살표
                    g_panOffsetY -= 50;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;

                case VK_DOWN:
                    // 아래쪽 화살표
                    g_panOffsetY += 50;
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
            }
            return 0;

        case WM_MOUSEWHEEL: {
            // 마우스 휠로 줌 레벨 조절
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (delta > 0) {
                // 휠 위로 - 확대
                g_zoomLevel += 25;
                if (g_zoomLevel > 2000) g_zoomLevel = 2000;
            } else {
                // 휠 아래로 - 축소
                g_zoomLevel -= 25;
                if (g_zoomLevel < 100) g_zoomLevel = 100;
            }
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            // 드래그 시작
            g_bDragging = true;
            g_ptDragStart.x = LOWORD(lParam);
            g_ptDragStart.y = HIWORD(lParam);
            g_ptPanStart.x = g_panOffsetX;
            g_ptPanStart.y = g_panOffsetY;
            SetCapture(hwnd);
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (g_bDragging) {
                // 드래그 중 - 패닝
                int deltaX = LOWORD(lParam) - g_ptDragStart.x;
                int deltaY = HIWORD(lParam) - g_ptDragStart.y;

                // 확대 배율에 따라 패닝 속도 조절
                float zoom = (float)g_zoomLevel / 100.0f;
                g_panOffsetX = g_ptPanStart.x + (int)(deltaX / zoom);
                g_panOffsetY = g_ptPanStart.y + (int)(deltaY / zoom);

                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP:
            // 드래그 종료
            if (g_bDragging) {
                g_bDragging = false;
                ReleaseCapture();
            }
            return 0;

        case WM_DESTROY:
            OutputDebugStringW(L"[ZoomOverlay] Window destroyed\n");
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

/**
 * 확대 윈도우 클래스 등록
 */
static bool RegisterZoomWindow(HINSTANCE hInstance)
{
    WNDCLASSEXW wc = {0};

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ZoomWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = ZOOM_CLASS_NAME;
    wc.hIconSm = NULL;

    if (!RegisterClassExW(&wc)) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            OutputDebugStringW(L"[ZoomOverlay] Failed to register window class\n");
            return false;
        }
    }

    OutputDebugStringW(L"[ZoomOverlay] Window class registered\n");
    return true;
}

/**
 * 확대 오버레이 초기화
 */
bool ZoomOverlay_Initialize(HINSTANCE hInstance)
{
    if (g_hInstance) {
        return true;  // 이미 초기화됨
    }

    g_hInstance = hInstance;

    if (!RegisterZoomWindow(hInstance)) {
        g_hInstance = NULL;
        return false;
    }

    OutputDebugStringW(L"[ZoomOverlay] Initialized\n");
    return true;
}

/**
 * 확대 오버레이 정리
 */
void ZoomOverlay_Shutdown(void)
{
    if (g_bActive) {
        ZoomOverlay_Hide();
    }

    if (g_hInstance) {
        UnregisterClassW(ZOOM_CLASS_NAME, g_hInstance);
        g_hInstance = NULL;
    }

    OutputDebugStringW(L"[ZoomOverlay] Shutdown\n");
}

/**
 * 확대 모드 시작
 */
bool ZoomOverlay_Show(int zoomLevel, bool smoothZoom)
{
    if (g_bActive) {
        return true;  // 이미 활성화됨
    }

    OutputDebugStringW(L"[ZoomOverlay] Showing zoom overlay\n");

    g_zoomLevel = zoomLevel;
    g_smoothZoom = smoothZoom;

    // 패닝 오프셋 초기화
    g_panOffsetX = 0;
    g_panOffsetY = 0;
    g_bDragging = false;

    // 렌더링 리소스 생성
    if (!CreateRenderResources()) {
        return false;
    }

    // 전체 화면 윈도우 생성
    g_hwndZoom = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,  // 항상 위, 작업표시줄에 안 뜸
        ZOOM_CLASS_NAME,
        L"LetsZoom Overlay",
        WS_POPUP,  // 테두리 없는 팝업
        0, 0,
        g_screenWidth, g_screenHeight,
        NULL, NULL,
        g_hInstance,
        NULL
    );

    if (!g_hwndZoom) {
        OutputDebugStringW(L"[ZoomOverlay] Failed to create window\n");
        DestroyRenderResources();
        return false;
    }

    // 윈도우 표시
    ShowWindow(g_hwndZoom, SW_SHOW);
    UpdateWindow(g_hwndZoom);
    SetForegroundWindow(g_hwndZoom);

    // 업데이트 타이머 시작
    SetTimer(g_hwndZoom, TIMER_UPDATE, UPDATE_INTERVAL, NULL);

    g_bActive = true;

    OutputDebugStringW(L"[ZoomOverlay] Zoom overlay shown\n");
    return true;
}

/**
 * 확대 모드 종료
 */
void ZoomOverlay_Hide(void)
{
    if (!g_bActive) {
        return;
    }

    OutputDebugStringW(L"[ZoomOverlay] Hiding zoom overlay\n");

    // 타이머 중지
    if (g_hwndZoom) {
        KillTimer(g_hwndZoom, TIMER_UPDATE);
    }

    // 윈도우 파괴
    if (g_hwndZoom) {
        DestroyWindow(g_hwndZoom);
        g_hwndZoom = NULL;
    }

    // 렌더링 리소스 해제
    DestroyRenderResources();

    g_bActive = false;

    OutputDebugStringW(L"[ZoomOverlay] Zoom overlay hidden\n");
}

/**
 * 확대 모드 활성 상태 확인
 */
bool ZoomOverlay_IsActive(void)
{
    return g_bActive;
}
