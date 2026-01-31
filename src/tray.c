/**
 * tray.c - 트레이 아이콘 관리 구현
 */

#include "tray.h"
#include <shellapi.h>

// 트레이 아이콘 데이터
static NOTIFYICONDATAW g_nid = {0};
static bool g_bInitialized = false;

/**
 * 트레이 아이콘 초기화
 */
bool Tray_Initialize(HWND hwnd)
{
    if (g_bInitialized) {
        return true;
    }

    // NOTIFYICONDATA 구조체 초기화
    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = hwnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;

    // 기본 아이콘 로드 (나중에 커스텀 아이콘으로 교체)
    g_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    // 툴팁 설정
    wcscpy_s(g_nid.szTip, sizeof(g_nid.szTip) / sizeof(wchar_t), L"LetsZoom");

    // 트레이 아이콘 추가
    if (!Shell_NotifyIconW(NIM_ADD, &g_nid)) {
        MessageBoxW(NULL, L"트레이 아이콘 추가 실패!", L"오류", MB_ICONERROR);
        return false;
    }

    g_bInitialized = true;
    OutputDebugStringW(L"[LetsZoom] Tray icon initialized\n");

    return true;
}

/**
 * 트레이 아이콘 제거
 */
void Tray_Shutdown(void)
{
    if (!g_bInitialized) {
        return;
    }

    Shell_NotifyIconW(NIM_DELETE, &g_nid);
    g_bInitialized = false;

    OutputDebugStringW(L"[LetsZoom] Tray icon removed\n");
}

/**
 * 컨텍스트 메뉴 생성
 */
static HMENU CreateTrayMenu(void)
{
    HMENU hMenu = CreatePopupMenu();

    AppendMenuW(hMenu, MF_STRING, IDM_ABOUT, L"LetsZoom 정보(&A)");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_SETTINGS, L"설정(&S)...");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"종료(&X)");

    return hMenu;
}

/**
 * 트레이 아이콘 메시지 처리
 */
void Tray_HandleMessage(HWND hwnd, LPARAM lParam)
{
    switch (lParam) {
        case WM_RBUTTONUP:
        case WM_CONTEXTMENU:
            {
                // 우클릭 - 컨텍스트 메뉴 표시
                POINT pt;
                GetCursorPos(&pt);

                HMENU hMenu = CreateTrayMenu();

                // 메뉴를 표시하기 전에 포그라운드 설정 (Windows 요구사항)
                SetForegroundWindow(hwnd);

                // 메뉴 표시
                TrackPopupMenu(
                    hMenu,
                    TPM_BOTTOMALIGN | TPM_LEFTALIGN,
                    pt.x, pt.y,
                    0,
                    hwnd,
                    NULL
                );

                // 메뉴 파괴
                DestroyMenu(hMenu);

                OutputDebugStringW(L"[LetsZoom] Tray menu shown\n");
            }
            break;

        case WM_LBUTTONDBLCLK:
            // 더블클릭 - 설정 창 열기 (나중에 구현)
            MessageBoxW(
                hwnd,
                L"설정 창은 나중에 구현됩니다.",
                L"LetsZoom",
                MB_OK | MB_ICONINFORMATION
            );
            break;
    }
}

/**
 * 트레이 알림 표시
 */
void Tray_ShowNotification(const wchar_t* title, const wchar_t* message)
{
    if (!g_bInitialized) {
        return;
    }

    // Windows Vista+ Balloon 알림
    g_nid.uFlags = NIF_INFO;
    g_nid.dwInfoFlags = NIIF_INFO;

    wcscpy_s(g_nid.szInfoTitle, sizeof(g_nid.szInfoTitle) / sizeof(wchar_t), title);
    wcscpy_s(g_nid.szInfo, sizeof(g_nid.szInfo) / sizeof(wchar_t), message);

    Shell_NotifyIconW(NIM_MODIFY, &g_nid);

    // 플래그 복원
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

    OutputDebugStringW(L"[LetsZoom] Notification shown\n");
}
