/**
 * tray.h - 트레이 아이콘 관리
 */

#ifndef LETSZOOM_TRAY_H
#define LETSZOOM_TRAY_H

#include <windows.h>
#include <stdbool.h>

// 트레이 아이콘 메시지
#define WM_TRAYICON (WM_USER + 1)

// 메뉴 아이템 ID
#define IDM_EXIT 100
#define IDM_SETTINGS 101
#define IDM_ABOUT 102

/**
 * 트레이 아이콘 초기화
 */
bool Tray_Initialize(HWND hwnd);

/**
 * 트레이 아이콘 제거
 */
void Tray_Shutdown(void);

/**
 * 트레이 아이콘 메시지 처리
 */
void Tray_HandleMessage(HWND hwnd, LPARAM lParam);

/**
 * 트레이 알림 표시
 */
void Tray_ShowNotification(const wchar_t* title, const wchar_t* message);

#endif // LETSZOOM_TRAY_H
