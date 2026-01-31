/**
 * LetsZoom - 경량 화면 확대 및 주석 도구
 *
 * Phase 1: 프로젝트 셋업
 * Issue #2: CMake 빌드 시스템 구성 테스트
 */

#include <windows.h>

/**
 * WinMain - Windows GUI 애플리케이션 진입점
 */
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    // 빌드 테스트용 메시지 박스
    MessageBoxW(
        NULL,
        L"LetsZoom 빌드 시스템이 정상적으로 작동합니다!\n\n"
        L"다음 단계: Issue #3 - 기본 윈도우 및 메시지 루프 구현",
        L"LetsZoom - Build Test",
        MB_OK | MB_ICONINFORMATION
    );

    return 0;
}
