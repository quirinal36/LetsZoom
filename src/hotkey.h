/**
 * hotkey.h - 전역 단축키 관리
 */

#ifndef LETSZOOM_HOTKEY_H
#define LETSZOOM_HOTKEY_H

#include <windows.h>
#include <stdbool.h>

// 단축키 ID
enum {
    HOTKEY_ZOOM = 1,
    HOTKEY_DRAW = 2,
    HOTKEY_ZOOM_DRAW = 3,
    HOTKEY_SCREENSHOT = 4
};

/**
 * 전역 단축키 초기화
 */
bool Hotkey_Initialize(HWND hwnd);

/**
 * 전역 단축키 해제
 */
void Hotkey_Shutdown(HWND hwnd);

/**
 * 단축키 메시지 처리
 */
void Hotkey_HandleMessage(WPARAM hotkeyId);

#endif // LETSZOOM_HOTKEY_H
