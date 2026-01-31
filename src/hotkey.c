/**
 * hotkey.c - 전역 단축키 관리 구현
 */

#include "hotkey.h"

// 초기화 상태
static bool g_bInitialized = false;

/**
 * 전역 단축키 등록
 */
bool Hotkey_Initialize(HWND hwnd)
{
    if (g_bInitialized) {
        return true;
    }

    bool success = true;

    // Ctrl+1: 확대 모드
    if (!RegisterHotKey(hwnd, HOTKEY_ZOOM, MOD_CONTROL, '1')) {
        OutputDebugStringW(L"[LetsZoom] Failed to register Ctrl+1\n");
        success = false;
    }

    // Ctrl+2: 그리기 모드
    if (!RegisterHotKey(hwnd, HOTKEY_DRAW, MOD_CONTROL, '2')) {
        OutputDebugStringW(L"[LetsZoom] Failed to register Ctrl+2\n");
        success = false;
    }

    // Ctrl+3: 확대+그리기 모드
    if (!RegisterHotKey(hwnd, HOTKEY_ZOOM_DRAW, MOD_CONTROL, '3')) {
        OutputDebugStringW(L"[LetsZoom] Failed to register Ctrl+3\n");
        success = false;
    }

    // Ctrl+4: 스크린샷
    if (!RegisterHotKey(hwnd, HOTKEY_SCREENSHOT, MOD_CONTROL, '4')) {
        OutputDebugStringW(L"[LetsZoom] Failed to register Ctrl+4\n");
        success = false;
    }

    if (success) {
        g_bInitialized = true;
        OutputDebugStringW(L"[LetsZoom] Hotkeys registered\n");
    } else {
        // 실패 시 등록된 단축키 해제
        Hotkey_Shutdown(hwnd);
        MessageBoxW(
            hwnd,
            L"일부 단축키 등록에 실패했습니다.\n"
            L"다른 프로그램이 이미 사용 중일 수 있습니다.",
            L"경고",
            MB_OK | MB_ICONWARNING
        );
    }

    return success;
}

/**
 * 전역 단축키 해제
 */
void Hotkey_Shutdown(HWND hwnd)
{
    if (!g_bInitialized) {
        return;
    }

    UnregisterHotKey(hwnd, HOTKEY_ZOOM);
    UnregisterHotKey(hwnd, HOTKEY_DRAW);
    UnregisterHotKey(hwnd, HOTKEY_ZOOM_DRAW);
    UnregisterHotKey(hwnd, HOTKEY_SCREENSHOT);

    g_bInitialized = false;

    OutputDebugStringW(L"[LetsZoom] Hotkeys unregistered\n");
}

/**
 * 단축키 메시지 처리
 */
void Hotkey_HandleMessage(WPARAM hotkeyId)
{
    switch (hotkeyId) {
        case HOTKEY_ZOOM:
            OutputDebugStringW(L"[LetsZoom] Hotkey: Zoom (Ctrl+1)\n");
            MessageBoxW(
                NULL,
                L"확대 모드 (Ctrl+1) 실행!\n\nPhase 3에서 구현됩니다.",
                L"LetsZoom - 확대 모드",
                MB_OK | MB_ICONINFORMATION
            );
            break;

        case HOTKEY_DRAW:
            OutputDebugStringW(L"[LetsZoom] Hotkey: Draw (Ctrl+2)\n");
            MessageBoxW(
                NULL,
                L"그리기 모드 (Ctrl+2) 실행!\n\nPhase 4에서 구현됩니다.",
                L"LetsZoom - 그리기 모드",
                MB_OK | MB_ICONINFORMATION
            );
            break;

        case HOTKEY_ZOOM_DRAW:
            OutputDebugStringW(L"[LetsZoom] Hotkey: Zoom+Draw (Ctrl+3)\n");
            MessageBoxW(
                NULL,
                L"확대+그리기 모드 (Ctrl+3) 실행!\n\nPhase 3-4에서 구현됩니다.",
                L"LetsZoom - 확대+그리기 모드",
                MB_OK | MB_ICONINFORMATION
            );
            break;

        case HOTKEY_SCREENSHOT:
            OutputDebugStringW(L"[LetsZoom] Hotkey: Screenshot (Ctrl+4)\n");
            MessageBoxW(
                NULL,
                L"스크린샷 (Ctrl+4) 실행!\n\nPhase 5에서 구현됩니다.",
                L"LetsZoom - 스크린샷",
                MB_OK | MB_ICONINFORMATION
            );
            break;

        default:
            OutputDebugStringW(L"[LetsZoom] Unknown hotkey\n");
            break;
    }
}
