/**
 * settings.h - 설정 관리
 */

#ifndef LETSZOOM_SETTINGS_H
#define LETSZOOM_SETTINGS_H

#include <windows.h>
#include <stdbool.h>

// 설정 구조체
typedef struct {
    // 단축키 설정
    UINT hotkeyZoomMod;          // 확대 모드 수정자 (MOD_CONTROL 등)
    UINT hotkeyZoomKey;          // 확대 모드 키
    UINT hotkeyDrawMod;          // 그리기 모드 수정자
    UINT hotkeyDrawKey;          // 그리기 모드 키
    UINT hotkeyZoomDrawMod;      // 확대+그리기 모드 수정자
    UINT hotkeyZoomDrawKey;      // 확대+그리기 모드 키
    UINT hotkeyScreenshotMod;    // 스크린샷 수정자
    UINT hotkeyScreenshotKey;    // 스크린샷 키

    // 확대 설정
    int zoomLevel;               // 확대 배율 (100-800%)
    bool smoothZoom;             // 부드러운 확대 활성화

    // 그리기 설정
    COLORREF penColor;           // 펜 색상
    int penWidth;                // 펜 두께 (1-20 픽셀)
    int penOpacity;              // 펜 투명도 (0-255)

    // 스크린샷 설정
    WCHAR screenshotPath[MAX_PATH];  // 스크린샷 저장 경로
    int screenshotFormat;        // 저장 형식 (0=PNG, 1=JPG, 2=BMP)

    // 일반 설정
    bool startWithWindows;       // Windows 시작 시 자동 실행
    bool showNotifications;      // 알림 표시
} Settings;

/**
 * 설정 초기화 (기본값 설정)
 */
void Settings_Initialize(Settings* settings);

/**
 * 설정 파일에서 불러오기
 * %APPDATA%\LetsZoom\config.ini
 */
bool Settings_Load(Settings* settings);

/**
 * 설정 파일에 저장
 */
bool Settings_Save(const Settings* settings);

/**
 * 설정 파일 경로 가져오기
 */
bool Settings_GetConfigPath(WCHAR* path, size_t pathSize);

#endif // LETSZOOM_SETTINGS_H
