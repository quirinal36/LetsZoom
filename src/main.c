/**
 * LetsZoom - 경량 화면 확대 및 주석 도구
 *
 * main.c - 메인 진입점 및 메시지 루프
 */

#include <windows.h>
#include <stdbool.h>
#include "tray.h"
#include "hotkey.h"
#include "settings.h"

// 윈도우 클래스 이름
#define WINDOW_CLASS_NAME L"LetsZoomMainWindow"
#define WINDOW_TITLE L"LetsZoom"

// 전역 변수
static HWND g_hwndMain = NULL;
static bool g_bRunning = true;
static Settings g_settings = {0};

/**
 * 윈도우 프로시저 - 메시지 처리
 */
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CREATE:
            // 윈도우 생성 시
            OutputDebugStringW(L"[LetsZoom] Main window created\n");
            return 0;

        case WM_DESTROY:
            // 윈도우 파괴 시
            OutputDebugStringW(L"[LetsZoom] Main window destroyed\n");
            Tray_Shutdown();
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            // 윈도우 닫기 시 (최소화만 하고 종료 안 함)
            OutputDebugStringW(L"[LetsZoom] WM_CLOSE received\n");
            return 0;

        case WM_TRAYICON:
            // 트레이 아이콘 메시지
            Tray_HandleMessage(hwnd, lParam);
            return 0;

        case WM_HOTKEY:
            // 전역 단축키 메시지
            Hotkey_HandleMessage(wParam);
            return 0;

        case WM_COMMAND:
            // 메뉴 명령 처리
            switch (LOWORD(wParam)) {
                case IDM_EXIT:
                    DestroyWindow(hwnd);
                    break;

                case IDM_SETTINGS:
                    MessageBoxW(hwnd, L"설정 창은 나중에 구현됩니다.", L"설정", MB_OK);
                    break;

                case IDM_ABOUT:
                    MessageBoxW(
                        hwnd,
                        L"LetsZoom v0.3.0\n\n"
                        L"경량 화면 확대 및 주석 도구\n\n"
                        L"Phase 1 완료:\n"
                        L"✓ VS Code 개발 환경\n"
                        L"✓ CMake 빌드 시스템\n"
                        L"✓ 기본 윈도우 및 메시지 루프\n"
                        L"✓ 트레이 아이콘 및 메뉴\n\n"
                        L"Phase 2 완료:\n"
                        L"✓ 전역 단축키 (Ctrl+1~4)\n"
                        L"✓ 설정 저장/불러오기 (INI 파일)",
                        L"LetsZoom 정보",
                        MB_OK | MB_ICONINFORMATION
                    );
                    break;
            }
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

/**
 * 윈도우 클래스 등록
 */
static bool RegisterMainWindow(HINSTANCE hInstance)
{
    WNDCLASSEXW wc = {0};

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = 0;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"윈도우 클래스 등록 실패!", L"오류", MB_ICONERROR);
        return false;
    }

    OutputDebugStringW(L"[LetsZoom] Window class registered\n");
    return true;
}

/**
 * 숨겨진 메인 윈도우 생성 (메시지 수신용)
 */
static HWND CreateMainWindow(HINSTANCE hInstance)
{
    // 화면 밖에 숨겨진 윈도우 생성
    HWND hwnd = CreateWindowExW(
        0,                          // dwExStyle
        WINDOW_CLASS_NAME,          // lpClassName
        WINDOW_TITLE,               // lpWindowName
        WS_OVERLAPPED,              // dwStyle (최소 스타일)
        -32000, -32000,             // x, y (화면 밖)
        1, 1,                       // width, height (최소 크기)
        NULL,                       // hWndParent
        NULL,                       // hMenu
        hInstance,                  // hInstance
        NULL                        // lpParam
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"메인 윈도우 생성 실패!", L"오류", MB_ICONERROR);
        return NULL;
    }

    OutputDebugStringW(L"[LetsZoom] Main window created (hidden)\n");
    return hwnd;
}

/**
 * 초기화
 */
static bool Initialize(HINSTANCE hInstance)
{
    OutputDebugStringW(L"[LetsZoom] Initializing...\n");

    // 1. 설정 불러오기
    Settings_Initialize(&g_settings);
    Settings_Load(&g_settings);

    // 2. 윈도우 클래스 등록
    if (!RegisterMainWindow(hInstance)) {
        return false;
    }

    // 3. 메인 윈도우 생성
    g_hwndMain = CreateMainWindow(hInstance);
    if (!g_hwndMain) {
        return false;
    }

    // 4. 트레이 아이콘 초기화
    if (!Tray_Initialize(g_hwndMain)) {
        return false;
    }

    // 5. 전역 단축키 등록
    if (!Hotkey_Initialize(g_hwndMain)) {
        return false;
    }

    OutputDebugStringW(L"[LetsZoom] Initialization completed\n");

    // 초기화 완료 알림 (설정에서 활성화된 경우)
    if (g_settings.showNotifications) {
        Tray_ShowNotification(
            L"LetsZoom",
            L"LetsZoom이 시작되었습니다!\n트레이 아이콘을 우클릭하여 메뉴를 여세요."
        );
    }

    return true;
}

/**
 * 정리
 */
static void Cleanup(void)
{
    OutputDebugStringW(L"[LetsZoom] Cleaning up...\n");

    // 설정 저장
    Settings_Save(&g_settings);

    if (g_hwndMain) {
        Hotkey_Shutdown(g_hwndMain);
        Tray_Shutdown();
        DestroyWindow(g_hwndMain);
        g_hwndMain = NULL;
    }

    OutputDebugStringW(L"[LetsZoom] Cleanup completed\n");
}

/**
 * 메시지 루프
 */
static int MessageLoop(void)
{
    MSG msg;

    OutputDebugStringW(L"[LetsZoom] Entering message loop...\n");

    // 메시지 루프
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    OutputDebugStringW(L"[LetsZoom] Exiting message loop\n");
    return (int)msg.wParam;
}

/**
 * WinMain - 진입점
 */
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    int exitCode = 0;

    OutputDebugStringW(L"[LetsZoom] Application started\n");

    // 1. 초기화
    if (!Initialize(hInstance)) {
        MessageBoxW(NULL, L"초기화 실패!", L"오류", MB_ICONERROR);
        return 1;
    }

    // 2. 메시지 루프
    exitCode = MessageLoop();

    // 3. 정리
    Cleanup();

    OutputDebugStringW(L"[LetsZoom] Application exited\n");
    return exitCode;
}
