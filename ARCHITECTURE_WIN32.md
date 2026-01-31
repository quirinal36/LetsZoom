# LetsZoom - Win32 API 아키텍처 문서

## 1. 기술 스택 선택

### 1.1 Win32 API를 선택한 이유
- **VS Code 호환**: Visual Studio 불필요, 경량 개발 환경
- **최경량**: 실행 파일 2-3MB 목표 달성 가능
- **네이티브 성능**: Windows API 직접 호출, 오버헤드 최소화
- **완전한 제어**: 모든 기능을 직접 구현
- **런타임 불필요**: MFC/ATL 런타임 라이브러리 불필요

### 1.2 기술 구성
- **Language**: C11 또는 C++17
- **Framework**: Win32 API (순수 Windows API)
- **Compiler**: MinGW-w64 (GCC) 또는 MSVC
- **Build System**: CMake 또는 Makefile
- **Graphics**: GDI / GDI+
- **IDE**: Visual Studio Code

## 2. 아키텍처 개요

```
┌─────────────────────────────────────────────────────────┐
│                      WinMain()                          │
│                    (진입점)                              │
└────────────────────┬────────────────────────────────────┘
                     │
    ┌────────────────┼────────────────┐
    │                │                │
┌───▼──────┐  ┌─────▼──────┐  ┌─────▼─────┐
│  Hidden  │  │   Hotkey   │  │   Tray    │
│  Window  │  │  Handler   │  │   Icon    │
└──────────┘  └────────────┘  └───────────┘
                     │
    ┌────────────────┼────────────────┐
    │                │                │
┌───▼──────┐  ┌─────▼──────┐  ┌─────▼─────┐
│   Zoom   │  │  Drawing   │  │ Screenshot│
│  Overlay │  │   Overlay  │  │  Capture  │
└──────────┘  └────────────┘  └───────────┘
```

## 3. 핵심 모듈 구조

### 3.1 WinMain (진입점)
```c
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    // 1. 초기화
    InitCommonControls();
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 2. 숨겨진 메인 윈도우 생성 (메시지 수신용)
    HWND hwndMain = CreateHiddenWindow(hInstance);

    // 3. 전역 단축키 등록
    RegisterGlobalHotkeys(hwndMain);

    // 4. 트레이 아이콘 추가
    AddTrayIcon(hwndMain);

    // 5. 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 6. 정리
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}
```

### 3.2 숨겨진 메인 윈도우
```c
// 윈도우 클래스 등록
WNDCLASSEX wc = {0};
wc.cbSize = sizeof(WNDCLASSEX);
wc.lpfnWndProc = MainWndProc;
wc.hInstance = hInstance;
wc.lpszClassName = TEXT("LetsZoomMainWindow");
RegisterClassEx(&wc);

// 숨겨진 윈도우 생성 (메시지만 받음)
HWND hwnd = CreateWindowEx(
    0,
    TEXT("LetsZoomMainWindow"),
    TEXT("LetsZoom"),
    0,  // 스타일 없음
    -32000, -32000, 1, 1,  // 화면 밖
    NULL, NULL, hInstance, NULL
);

// 메시지 프로시저
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_HOTKEY:
            OnHotkey(wParam);
            break;

        case WM_TRAYICON:
            OnTrayIcon(lParam);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
```

### 3.3 전역 단축키
```c
// hotkey.h
typedef struct {
    int id;
    UINT modifiers;  // MOD_CONTROL, MOD_SHIFT, MOD_ALT, MOD_WIN
    UINT vk;         // Virtual key code
} HotkeyInfo;

// 단축키 등록
void RegisterGlobalHotkeys(HWND hwnd)
{
    // Ctrl+1: 확대 모드
    RegisterHotKey(hwnd, HOTKEY_ZOOM, MOD_CONTROL, '1');

    // Ctrl+2: 그리기 모드
    RegisterHotKey(hwnd, HOTKEY_DRAW, MOD_CONTROL, '2');

    // Ctrl+3: 확대+그리기 모드
    RegisterHotKey(hwnd, HOTKEY_ZOOM_DRAW, MOD_CONTROL, '3');

    // Ctrl+4: 스크린샷
    RegisterHotKey(hwnd, HOTKEY_SCREENSHOT, MOD_CONTROL, '4');
}

// 단축키 핸들러
void OnHotkey(WPARAM hotkeyId)
{
    switch (hotkeyId) {
        case HOTKEY_ZOOM:
            ShowZoomOverlay();
            break;

        case HOTKEY_DRAW:
            ShowDrawingOverlay();
            break;

        case HOTKEY_ZOOM_DRAW:
            ShowZoomOverlay();
            ShowDrawingOverlay();
            break;

        case HOTKEY_SCREENSHOT:
            CaptureScreenshot();
            break;
    }
}
```

### 3.4 트레이 아이콘
```c
// tray.h
#define WM_TRAYICON (WM_USER + 1)
#define IDM_EXIT 100
#define IDM_SETTINGS 101

NOTIFYICONDATA g_nid = {0};

void AddTrayIcon(HWND hwnd)
{
    g_nid.cbSize = sizeof(NOTIFYICONDATA);
    g_nid.hWnd = hwnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAINICON));
    lstrcpy(g_nid.szTip, TEXT("LetsZoom"));

    Shell_NotifyIcon(NIM_ADD, &g_nid);
}

void OnTrayIcon(LPARAM lParam)
{
    if (lParam == WM_RBUTTONUP) {
        // 우클릭 메뉴 표시
        POINT pt;
        GetCursorPos(&pt);

        HMENU hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, IDM_SETTINGS, TEXT("설정(&S)"));
        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, MF_STRING, IDM_EXIT, TEXT("종료(&X)"));

        SetForegroundWindow(g_nid.hWnd);
        TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN,
                      pt.x, pt.y, 0, g_nid.hWnd, NULL);
        DestroyMenu(hMenu);
    }
}
```

### 3.5 확대 오버레이 윈도우
```c
// zoom_overlay.h
typedef struct {
    HWND hwnd;
    HDC hdcScreen;
    HBITMAP hbmScreen;
    int screenWidth;
    int screenHeight;
    int zoomLevel;      // 100 ~ 2000
    POINT centerPoint;  // 확대 중심점
    BOOL isPanning;
} ZoomOverlay;

ZoomOverlay* CreateZoomOverlay(HINSTANCE hInstance)
{
    ZoomOverlay* overlay = (ZoomOverlay*)malloc(sizeof(ZoomOverlay));

    // 윈도우 클래스 등록
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = ZoomOverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("LetsZoomOverlay");
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    RegisterClassEx(&wc);

    // 전체화면 레이어드 윈도우 생성
    overlay->hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        TEXT("LetsZoomOverlay"),
        TEXT(""),
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL
    );

    // 윈도우에 구조체 포인터 저장
    SetWindowLongPtr(overlay->hwnd, GWLP_USERDATA, (LONG_PTR)overlay);

    overlay->zoomLevel = 200;
    overlay->isPanning = FALSE;

    return overlay;
}

LRESULT CALLBACK ZoomOverlayWndProc(HWND hwnd, UINT msg,
                                     WPARAM wParam, LPARAM lParam)
{
    ZoomOverlay* overlay = (ZoomOverlay*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_CREATE:
            // 화면 캡처
            CaptureScreen(overlay);
            break;

        case WM_PAINT:
            OnPaintZoom(overlay);
            break;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                HideZoomOverlay(overlay);
            }
            else if (wParam == VK_ADD || wParam == VK_OEM_PLUS) {
                overlay->zoomLevel = min(2000, overlay->zoomLevel + 10);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else if (wParam == VK_SUBTRACT || wParam == VK_OEM_MINUS) {
                overlay->zoomLevel = max(100, overlay->zoomLevel - 10);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;

        case WM_MOUSEWHEEL:
            {
                short delta = GET_WHEEL_DELTA_WPARAM(wParam);
                overlay->zoomLevel += (delta > 0) ? 10 : -10;
                overlay->zoomLevel = max(100, min(2000, overlay->zoomLevel));
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;

        case WM_LBUTTONDOWN:
            overlay->isPanning = TRUE;
            SetCapture(hwnd);
            break;

        case WM_LBUTTONUP:
            overlay->isPanning = FALSE;
            ReleaseCapture();
            break;

        case WM_MOUSEMOVE:
            if (overlay->isPanning) {
                // 패닝 구현
                int dx = GET_X_LPARAM(lParam) - overlay->centerPoint.x;
                int dy = GET_Y_LPARAM(lParam) - overlay->centerPoint.y;
                overlay->centerPoint.x += dx / (overlay->zoomLevel / 100.0f);
                overlay->centerPoint.y += dy / (overlay->zoomLevel / 100.0f);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;

        case WM_DESTROY:
            if (overlay->hbmScreen) DeleteObject(overlay->hbmScreen);
            if (overlay->hdcScreen) DeleteDC(overlay->hdcScreen);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void OnPaintZoom(ZoomOverlay* overlay)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(overlay->hwnd, &ps);

    // 더블 버퍼링
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN));
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    // 배경 검정
    RECT rc = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // 확대된 화면 렌더링
    RenderZoomedView(overlay, hdcMem);

    // 화면에 복사
    BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);

    EndPaint(overlay->hwnd, &ps);
}

void RenderZoomedView(ZoomOverlay* overlay, HDC hdc)
{
    // 확대할 소스 영역 계산
    float zoom = overlay->zoomLevel / 100.0f;
    int srcWidth = (int)(GetSystemMetrics(SM_CXSCREEN) / zoom);
    int srcHeight = (int)(GetSystemMetrics(SM_CYSCREEN) / zoom);

    int srcX = overlay->centerPoint.x - srcWidth / 2;
    int srcY = overlay->centerPoint.y - srcHeight / 2;

    // 경계 체크
    srcX = max(0, min(overlay->screenWidth - srcWidth, srcX));
    srcY = max(0, min(overlay->screenHeight - srcHeight, srcY));

    // StretchBlt로 확대
    HDC hdcScreen = CreateCompatibleDC(hdc);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcScreen, overlay->hbmScreen);

    SetStretchBltMode(hdc, HALFTONE);
    StretchBlt(hdc,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        hdcScreen,
        srcX, srcY, srcWidth, srcHeight,
        SRCCOPY);

    SelectObject(hdcScreen, hbmOld);
    DeleteDC(hdcScreen);

    // 십자선 그리기
    DrawCrosshair(hdc);
}
```

### 3.6 그리기 오버레이 윈도우
```c
// drawing_overlay.h
typedef enum {
    TOOL_PEN,
    TOOL_LINE,
    TOOL_ARROW,
    TOOL_RECTANGLE,
    TOOL_ELLIPSE,
    TOOL_TEXT,
    TOOL_HIGHLIGHTER,
    TOOL_ERASER
} DrawTool;

typedef struct {
    DrawTool tool;
    POINT* points;
    int pointCount;
    int pointCapacity;
    COLORREF color;
    int thickness;
    BYTE opacity;
} Stroke;

typedef struct {
    HWND hwnd;
    DrawTool currentTool;
    COLORREF currentColor;
    int currentThickness;
    BOOL isDrawing;
    Stroke** strokes;
    int strokeCount;
    Stroke* currentStroke;
} DrawingOverlay;

DrawingOverlay* CreateDrawingOverlay(HINSTANCE hInstance)
{
    DrawingOverlay* overlay = (DrawingOverlay*)malloc(sizeof(DrawingOverlay));

    // 윈도우 클래스 등록
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = DrawingOverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("LetsZoomDrawingOverlay");
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    RegisterClassEx(&wc);

    // 투명 전체화면 윈도우
    overlay->hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        TEXT("LetsZoomDrawingOverlay"),
        TEXT(""),
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL
    );

    // 완전 투명 설정
    SetLayeredWindowAttributes(overlay->hwnd, 0, 255, LWA_ALPHA);

    SetWindowLongPtr(overlay->hwnd, GWLP_USERDATA, (LONG_PTR)overlay);

    overlay->currentTool = TOOL_PEN;
    overlay->currentColor = RGB(255, 0, 0);
    overlay->currentThickness = 4;
    overlay->isDrawing = FALSE;
    overlay->strokes = NULL;
    overlay->strokeCount = 0;

    return overlay;
}

LRESULT CALLBACK DrawingOverlayWndProc(HWND hwnd, UINT msg,
                                        WPARAM wParam, LPARAM lParam)
{
    DrawingOverlay* overlay = (DrawingOverlay*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_LBUTTONDOWN:
            StartDrawing(overlay, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;

        case WM_MOUSEMOVE:
            if (overlay->isDrawing) {
                ContinueDrawing(overlay, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            }
            break;

        case WM_LBUTTONUP:
            EndDrawing(overlay);
            break;

        case WM_PAINT:
            OnPaintDrawing(overlay);
            break;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                HideDrawingOverlay(overlay);
            }
            else if (wParam == 'Z' && GetKeyState(VK_CONTROL) < 0) {
                UndoStroke(overlay);
            }
            else if (wParam == 'E') {
                ClearAllStrokes(overlay);
            }
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void OnPaintDrawing(DrawingOverlay* overlay)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(overlay->hwnd, &ps);

    // GDI+로 안티앨리어싱 렌더링
    Graphics* graphics = NULL;
    GdipCreateFromHDC(hdc, &graphics);
    GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);

    // 모든 스트로크 그리기
    for (int i = 0; i < overlay->strokeCount; i++) {
        DrawStroke(graphics, overlay->strokes[i]);
    }

    // 현재 그리는 중인 스트로크
    if (overlay->isDrawing && overlay->currentStroke) {
        DrawStroke(graphics, overlay->currentStroke);
    }

    GdipDeleteGraphics(graphics);
    EndPaint(overlay->hwnd, &ps);
}

void DrawStroke(Graphics* graphics, const Stroke* stroke)
{
    if (stroke->pointCount < 2) return;

    // GDI+ Pen 생성
    Pen* pen = NULL;
    GdipCreatePen1(
        (stroke->color & 0x00FFFFFF) | ((DWORD)stroke->opacity << 24),
        (REAL)stroke->thickness,
        UnitPixel,
        &pen
    );
    GdipSetPenLineCap(pen, LineCapRound, LineCapRound, DashCapRound);

    switch (stroke->tool) {
        case TOOL_PEN:
            // 자유 곡선
            for (int i = 0; i < stroke->pointCount - 1; i++) {
                GdipDrawLineI(graphics, pen,
                    stroke->points[i].x, stroke->points[i].y,
                    stroke->points[i+1].x, stroke->points[i+1].y);
            }
            break;

        case TOOL_LINE:
            // 직선
            GdipDrawLineI(graphics, pen,
                stroke->points[0].x, stroke->points[0].y,
                stroke->points[stroke->pointCount-1].x,
                stroke->points[stroke->pointCount-1].y);
            break;

        case TOOL_ARROW:
            // 화살표
            DrawArrow(graphics, pen, stroke);
            break;

        case TOOL_RECTANGLE:
            // 사각형
            {
                int x = min(stroke->points[0].x, stroke->points[stroke->pointCount-1].x);
                int y = min(stroke->points[0].y, stroke->points[stroke->pointCount-1].y);
                int w = abs(stroke->points[stroke->pointCount-1].x - stroke->points[0].x);
                int h = abs(stroke->points[stroke->pointCount-1].y - stroke->points[0].y);
                GdipDrawRectangleI(graphics, pen, x, y, w, h);
            }
            break;

        case TOOL_ELLIPSE:
            // 타원
            {
                int x = min(stroke->points[0].x, stroke->points[stroke->pointCount-1].x);
                int y = min(stroke->points[0].y, stroke->points[stroke->pointCount-1].y);
                int w = abs(stroke->points[stroke->pointCount-1].x - stroke->points[0].x);
                int h = abs(stroke->points[stroke->pointCount-1].y - stroke->points[0].y);
                GdipDrawEllipseI(graphics, pen, x, y, w, h);
            }
            break;
    }

    GdipDeletePen(pen);
}
```

### 3.7 스크린샷 캡처
```c
// screenshot.h
HBITMAP CaptureScreen()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbm = CreateCompatibleBitmap(hdcScreen, width, height);

    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
    BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hbmOld);

    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    return hbm;
}

BOOL SaveBitmapToPNG(HBITMAP hbm, const WCHAR* filename)
{
    // GDI+로 PNG 저장
    Bitmap* bitmap = NULL;
    GdipCreateBitmapFromHBITMAP(hbm, NULL, &bitmap);

    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);

    Status status = GdipSaveImageToFile(bitmap, filename, &pngClsid, NULL);

    GdipDisposeImage(bitmap);

    return (status == Ok);
}

BOOL CopyBitmapToClipboard(HBITMAP hbm)
{
    if (!OpenClipboard(NULL)) return FALSE;

    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hbm);
    CloseClipboard();

    return TRUE;
}
```

### 3.8 설정 관리
```c
// settings.h
typedef struct {
    // 일반
    BOOL launchOnStartup;
    BOOL showTrayIcon;

    // 확대
    int defaultZoomLevel;
    int animationDuration;

    // 그리기
    COLORREF defaultColor;
    int defaultThickness;
    DrawTool defaultTool;

    // 단축키
    HotkeyInfo hotkeyZoom;
    HotkeyInfo hotkeyDraw;
    HotkeyInfo hotkeyZoomDraw;
    HotkeyInfo hotkeyScreenshot;

    // 스크린샷
    WCHAR saveFolder[MAX_PATH];
    int imageFormat;  // 0=PNG, 1=JPG
} Settings;

Settings g_settings;

void LoadSettings()
{
    // %APPDATA%\LetsZoom\config.ini
    WCHAR configPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, configPath);
    wcscat_s(configPath, MAX_PATH, L"\\LetsZoom\\config.ini");

    // 기본값 설정
    g_settings.launchOnStartup = TRUE;
    g_settings.showTrayIcon = TRUE;
    g_settings.defaultZoomLevel = 200;
    g_settings.defaultColor = RGB(255, 0, 0);
    g_settings.defaultThickness = 4;

    // INI 파일에서 읽기
    g_settings.launchOnStartup = GetPrivateProfileIntW(
        L"General", L"LaunchOnStartup", TRUE, configPath);

    g_settings.defaultZoomLevel = GetPrivateProfileIntW(
        L"Zoom", L"DefaultZoomLevel", 200, configPath);

    // ... 기타 설정
}

void SaveSettings()
{
    WCHAR configPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, configPath);
    wcscat_s(configPath, MAX_PATH, L"\\LetsZoom\\config.ini");

    // 디렉토리 생성
    WCHAR dirPath[MAX_PATH];
    wcscpy_s(dirPath, MAX_PATH, configPath);
    PathRemoveFileSpecW(dirPath);
    SHCreateDirectoryExW(NULL, dirPath, NULL);

    // INI 파일에 쓰기
    WCHAR buffer[32];
    _itow_s(g_settings.launchOnStartup, buffer, 32, 10);
    WritePrivateProfileStringW(L"General", L"LaunchOnStartup", buffer, configPath);

    _itow_s(g_settings.defaultZoomLevel, buffer, 32, 10);
    WritePrivateProfileStringW(L"Zoom", L"DefaultZoomLevel", buffer, configPath);

    // ... 기타 설정
}
```

## 4. 빌드 시스템

### 4.1 CMake 설정
```cmake
cmake_minimum_required(VERSION 3.15)
project(LetsZoom C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# 소스 파일
set(SOURCES
    src/main.c
    src/hotkey.c
    src/tray.c
    src/zoom_overlay.c
    src/drawing_overlay.c
    src/screenshot.c
    src/settings.c
)

# 실행 파일
add_executable(LetsZoom WIN32 ${SOURCES})

# Windows 라이브러리 링크
target_link_libraries(LetsZoom
    user32
    gdi32
    gdiplus
    comctl32
    shell32
    shlwapi
)

# 리소스 파일
target_sources(LetsZoom PRIVATE res/LetsZoom.rc)

# 최적화 옵션 (Release)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(LetsZoom PRIVATE
        -Os           # 크기 최적화
        -ffunction-sections
        -fdata-sections
    )
    target_link_options(LetsZoom PRIVATE
        -Wl,--gc-sections    # 사용하지 않는 섹션 제거
        -s                   # 심볼 제거
    )
endif()
```

### 4.2 Makefile (MinGW-w64)
```makefile
CC = gcc
CFLAGS = -Wall -Os -ffunction-sections -fdata-sections
LDFLAGS = -mwindows -Wl,--gc-sections -s
LIBS = -luser32 -lgdi32 -lgdiplus -lcomctl32 -lshell32 -lshlwapi

SOURCES = src/main.c src/hotkey.c src/tray.c src/zoom_overlay.c \
          src/drawing_overlay.c src/screenshot.c src/settings.c

OBJECTS = $(SOURCES:.c=.o)

TARGET = LetsZoom.exe

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	strip $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
```

## 5. 용량 최적화

### 5.1 컴파일 옵션
```bash
# GCC/MinGW-w64
gcc -Os -ffunction-sections -fdata-sections \
    -mwindows -Wl,--gc-sections -s \
    main.c ... -o LetsZoom.exe \
    -luser32 -lgdi32 -lgdiplus

# 결과: 약 2-3MB
```

### 5.2 UPX 압축 (선택 사항)
```bash
upx --best --lzma LetsZoom.exe

# 압축 후: 약 1-1.5MB
```

### 5.3 예상 크기
```
코드 + 데이터:        ~800 KB
GDI/GDI+:            ~400 KB
Win32 API:           ~300 KB
정적 CRT:            ~500 KB
리소스 (아이콘 등):   ~200 KB
---------------------------------
총 합계:              ~2.2 MB
UPX 압축 후:          ~1.0 MB
```

## 6. 프로젝트 구조

```
LetsZoom/
├── src/
│   ├── main.c                    // WinMain, 메시지 루프
│   ├── hotkey.c/h                // 전역 단축키
│   ├── tray.c/h                  // 트레이 아이콘
│   ├── zoom_overlay.c/h          // 확대 오버레이
│   ├── drawing_overlay.c/h       // 그리기 오버레이
│   ├── screenshot.c/h            // 스크린샷 캡처
│   ├── settings.c/h              // 설정 관리
│   └── utils.c/h                 // 유틸리티 함수
├── res/
│   ├── LetsZoom.ico              // 아이콘
│   ├── LetsZoom.rc               // 리소스 스크립트
│   └── resource.h                // 리소스 ID
├── docs/
│   ├── PRD.md
│   ├── ARCHITECTURE_WIN32.md
│   ├── IMPLEMENTATION_PLAN_WIN32.md
│   └── DESIGN_GUIDE_MFC.md       // 디자인은 재사용
├── CMakeLists.txt                // CMake 빌드 설정
├── Makefile                      // Make 빌드 설정
├── .vscode/
│   ├── c_cpp_properties.json     // IntelliSense 설정
│   ├── tasks.json                // 빌드 태스크
│   └── launch.json               // 디버그 설정
└── .gitignore
```

## 7. 개발 환경 (VS Code)

### 7.1 필수 확장
- C/C++ (Microsoft)
- CMake Tools
- CMake

### 7.2 컴파일러
- **MinGW-w64** (권장): 가볍고 설정 간단
- **MSVC** (선택): Visual Studio Build Tools

## 8. 성능 목표

| 항목 | 목표 | 달성 가능성 |
|------|------|-------------|
| 실행 파일 크기 | < 3MB | ✅ 가능 (2-3MB) |
| 시작 시간 | < 300ms | ✅ 가능 |
| 메모리 (대기) | < 5MB | ✅ 가능 |
| 메모리 (활성) | < 30MB | ✅ 가능 |
| 확대 응답 | < 100ms | ✅ 가능 |
| 그리기 FPS | > 60fps | ✅ 가능 |
| CPU (대기) | < 0.5% | ✅ 가능 |
| CPU (활성) | < 5% | ✅ 가능 |

## 9. MFC vs Win32 API 비교

| 항목 | MFC | Win32 API |
|------|-----|-----------|
| 실행 파일 | 3-4MB | 2-3MB |
| 메모리 사용 | 50MB | 30MB |
| 시작 시간 | 500ms | 300ms |
| 개발 난이도 | 중간 | 높음 |
| VS Code 지원 | 제한적 | 완벽 |
| 학습 곡선 | 완만 | 가파름 |
| 유지보수 | 쉬움 | 중간 |

**결론**: Win32 API가 모든 면에서 더 가볍고 VS Code 친화적입니다!

## 10. 다음 단계

1. VS Code 개발 환경 설정
2. MinGW-w64 또는 MSVC 설치
3. CMake 설정
4. Phase 1 구현 시작
