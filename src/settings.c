/**
 * settings.c - 설정 관리 구현
 */

#include "settings.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>

// INI 파일 섹션 이름
#define SECTION_HOTKEYS     L"Hotkeys"
#define SECTION_ZOOM        L"Zoom"
#define SECTION_DRAW        L"Draw"
#define SECTION_SCREENSHOT  L"Screenshot"
#define SECTION_GENERAL     L"General"

/**
 * 기본값으로 설정 초기화
 */
void Settings_Initialize(Settings* settings)
{
    if (!settings) {
        return;
    }

    // 단축키 기본값 (Ctrl+1~4)
    settings->hotkeyZoomMod = MOD_CONTROL;
    settings->hotkeyZoomKey = '1';
    settings->hotkeyDrawMod = MOD_CONTROL;
    settings->hotkeyDrawKey = '2';
    settings->hotkeyZoomDrawMod = MOD_CONTROL;
    settings->hotkeyZoomDrawKey = '3';
    settings->hotkeyScreenshotMod = MOD_CONTROL;
    settings->hotkeyScreenshotKey = '4';

    // 확대 기본값
    settings->zoomLevel = 200;        // 200%
    settings->smoothZoom = true;

    // 그리기 기본값
    settings->penColor = RGB(255, 0, 0);  // 빨간색
    settings->penWidth = 3;
    settings->penOpacity = 255;       // 불투명

    // 스크린샷 기본값
    WCHAR userProfile[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, userProfile))) {
        swprintf_s(settings->screenshotPath, MAX_PATH, L"%s\\Pictures\\LetsZoom", userProfile);
    } else {
        wcscpy_s(settings->screenshotPath, MAX_PATH, L"C:\\Screenshots");
    }
    settings->screenshotFormat = 0;   // PNG

    // 일반 기본값
    settings->startWithWindows = false;
    settings->showNotifications = true;

    OutputDebugStringW(L"[LetsZoom] Settings initialized with defaults\n");
}

/**
 * 설정 파일 경로 가져오기
 */
bool Settings_GetConfigPath(WCHAR* path, size_t pathSize)
{
    WCHAR appDataPath[MAX_PATH];

    // %APPDATA%\LetsZoom 경로 가져오기
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        OutputDebugStringW(L"[LetsZoom] Failed to get APPDATA path\n");
        return false;
    }

    // LetsZoom 폴더 경로 생성
    swprintf_s(path, pathSize, L"%s\\LetsZoom", appDataPath);

    // 폴더가 없으면 생성
    if (!PathFileExistsW(path)) {
        if (!CreateDirectoryW(path, NULL)) {
            OutputDebugStringW(L"[LetsZoom] Failed to create config directory\n");
            return false;
        }
    }

    // config.ini 경로 추가
    swprintf_s(path, pathSize, L"%s\\LetsZoom\\config.ini", appDataPath);

    return true;
}

/**
 * 설정 파일에서 불러오기
 */
bool Settings_Load(Settings* settings)
{
    if (!settings) {
        return false;
    }

    WCHAR configPath[MAX_PATH];
    if (!Settings_GetConfigPath(configPath, MAX_PATH)) {
        return false;
    }

    // 파일이 없으면 기본값 사용
    if (!PathFileExistsW(configPath)) {
        OutputDebugStringW(L"[LetsZoom] Config file not found, using defaults\n");
        Settings_Initialize(settings);
        return true;
    }

    OutputDebugStringW(L"[LetsZoom] Loading settings from: ");
    OutputDebugStringW(configPath);
    OutputDebugStringW(L"\n");

    // 단축키 불러오기
    settings->hotkeyZoomMod = GetPrivateProfileIntW(SECTION_HOTKEYS, L"ZoomMod", MOD_CONTROL, configPath);
    settings->hotkeyZoomKey = GetPrivateProfileIntW(SECTION_HOTKEYS, L"ZoomKey", '1', configPath);
    settings->hotkeyDrawMod = GetPrivateProfileIntW(SECTION_HOTKEYS, L"DrawMod", MOD_CONTROL, configPath);
    settings->hotkeyDrawKey = GetPrivateProfileIntW(SECTION_HOTKEYS, L"DrawKey", '2', configPath);
    settings->hotkeyZoomDrawMod = GetPrivateProfileIntW(SECTION_HOTKEYS, L"ZoomDrawMod", MOD_CONTROL, configPath);
    settings->hotkeyZoomDrawKey = GetPrivateProfileIntW(SECTION_HOTKEYS, L"ZoomDrawKey", '3', configPath);
    settings->hotkeyScreenshotMod = GetPrivateProfileIntW(SECTION_HOTKEYS, L"ScreenshotMod", MOD_CONTROL, configPath);
    settings->hotkeyScreenshotKey = GetPrivateProfileIntW(SECTION_HOTKEYS, L"ScreenshotKey", '4', configPath);

    // 확대 설정 불러오기
    settings->zoomLevel = GetPrivateProfileIntW(SECTION_ZOOM, L"ZoomLevel", 200, configPath);
    settings->smoothZoom = GetPrivateProfileIntW(SECTION_ZOOM, L"SmoothZoom", 1, configPath) != 0;

    // 그리기 설정 불러오기
    settings->penColor = GetPrivateProfileIntW(SECTION_DRAW, L"PenColor", RGB(255, 0, 0), configPath);
    settings->penWidth = GetPrivateProfileIntW(SECTION_DRAW, L"PenWidth", 3, configPath);
    settings->penOpacity = GetPrivateProfileIntW(SECTION_DRAW, L"PenOpacity", 255, configPath);

    // 스크린샷 설정 불러오기
    GetPrivateProfileStringW(SECTION_SCREENSHOT, L"Path", L"", settings->screenshotPath, MAX_PATH, configPath);
    if (wcslen(settings->screenshotPath) == 0) {
        // 기본 경로 설정
        WCHAR userProfile[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, userProfile))) {
            swprintf_s(settings->screenshotPath, MAX_PATH, L"%s\\Pictures\\LetsZoom", userProfile);
        }
    }
    settings->screenshotFormat = GetPrivateProfileIntW(SECTION_SCREENSHOT, L"Format", 0, configPath);

    // 일반 설정 불러오기
    settings->startWithWindows = GetPrivateProfileIntW(SECTION_GENERAL, L"StartWithWindows", 0, configPath) != 0;
    settings->showNotifications = GetPrivateProfileIntW(SECTION_GENERAL, L"ShowNotifications", 1, configPath) != 0;

    OutputDebugStringW(L"[LetsZoom] Settings loaded successfully\n");
    return true;
}

/**
 * 설정 파일에 저장
 */
bool Settings_Save(const Settings* settings)
{
    if (!settings) {
        return false;
    }

    WCHAR configPath[MAX_PATH];
    if (!Settings_GetConfigPath(configPath, MAX_PATH)) {
        return false;
    }

    OutputDebugStringW(L"[LetsZoom] Saving settings to: ");
    OutputDebugStringW(configPath);
    OutputDebugStringW(L"\n");

    WCHAR buffer[256];

    // 단축키 저장
    swprintf_s(buffer, 256, L"%u", settings->hotkeyZoomMod);
    WritePrivateProfileStringW(SECTION_HOTKEYS, L"ZoomMod", buffer, configPath);
    swprintf_s(buffer, 256, L"%u", settings->hotkeyZoomKey);
    WritePrivateProfileStringW(SECTION_HOTKEYS, L"ZoomKey", buffer, configPath);

    swprintf_s(buffer, 256, L"%u", settings->hotkeyDrawMod);
    WritePrivateProfileStringW(SECTION_HOTKEYS, L"DrawMod", buffer, configPath);
    swprintf_s(buffer, 256, L"%u", settings->hotkeyDrawKey);
    WritePrivateProfileStringW(SECTION_HOTKEYS, L"DrawKey", buffer, configPath);

    swprintf_s(buffer, 256, L"%u", settings->hotkeyZoomDrawMod);
    WritePrivateProfileStringW(SECTION_HOTKEYS, L"ZoomDrawMod", buffer, configPath);
    swprintf_s(buffer, 256, L"%u", settings->hotkeyZoomDrawKey);
    WritePrivateProfileStringW(SECTION_HOTKEYS, L"ZoomDrawKey", buffer, configPath);

    swprintf_s(buffer, 256, L"%u", settings->hotkeyScreenshotMod);
    WritePrivateProfileStringW(SECTION_HOTKEYS, L"ScreenshotMod", buffer, configPath);
    swprintf_s(buffer, 256, L"%u", settings->hotkeyScreenshotKey);
    WritePrivateProfileStringW(SECTION_HOTKEYS, L"ScreenshotKey", buffer, configPath);

    // 확대 설정 저장
    swprintf_s(buffer, 256, L"%d", settings->zoomLevel);
    WritePrivateProfileStringW(SECTION_ZOOM, L"ZoomLevel", buffer, configPath);
    swprintf_s(buffer, 256, L"%d", settings->smoothZoom ? 1 : 0);
    WritePrivateProfileStringW(SECTION_ZOOM, L"SmoothZoom", buffer, configPath);

    // 그리기 설정 저장
    swprintf_s(buffer, 256, L"%u", settings->penColor);
    WritePrivateProfileStringW(SECTION_DRAW, L"PenColor", buffer, configPath);
    swprintf_s(buffer, 256, L"%d", settings->penWidth);
    WritePrivateProfileStringW(SECTION_DRAW, L"PenWidth", buffer, configPath);
    swprintf_s(buffer, 256, L"%d", settings->penOpacity);
    WritePrivateProfileStringW(SECTION_DRAW, L"PenOpacity", buffer, configPath);

    // 스크린샷 설정 저장
    WritePrivateProfileStringW(SECTION_SCREENSHOT, L"Path", settings->screenshotPath, configPath);
    swprintf_s(buffer, 256, L"%d", settings->screenshotFormat);
    WritePrivateProfileStringW(SECTION_SCREENSHOT, L"Format", buffer, configPath);

    // 일반 설정 저장
    swprintf_s(buffer, 256, L"%d", settings->startWithWindows ? 1 : 0);
    WritePrivateProfileStringW(SECTION_GENERAL, L"StartWithWindows", buffer, configPath);
    swprintf_s(buffer, 256, L"%d", settings->showNotifications ? 1 : 0);
    WritePrivateProfileStringW(SECTION_GENERAL, L"ShowNotifications", buffer, configPath);

    OutputDebugStringW(L"[LetsZoom] Settings saved successfully\n");
    return true;
}
