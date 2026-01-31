/**
 * drawing_overlay.h - 그리기 오버레이 윈도우
 */

#ifndef LETSZOOM_DRAWING_OVERLAY_H
#define LETSZOOM_DRAWING_OVERLAY_H

#include <windows.h>
#include <stdbool.h>

/**
 * 그리기 오버레이 초기화
 */
bool DrawingOverlay_Initialize(HINSTANCE hInstance);

/**
 * 그리기 오버레이 정리
 */
void DrawingOverlay_Shutdown(void);

/**
 * 그리기 모드 시작
 * penColor: 펜 색상 (COLORREF)
 * penWidth: 펜 두께 (1-20 픽셀)
 * penOpacity: 펜 투명도 (0-255)
 */
bool DrawingOverlay_Show(COLORREF penColor, int penWidth, int penOpacity);

/**
 * 그리기 모드 종료
 */
void DrawingOverlay_Hide(void);

/**
 * 그리기 모드 활성 상태 확인
 */
bool DrawingOverlay_IsActive(void);

/**
 * 화면 지우기
 */
void DrawingOverlay_Clear(void);

#endif // LETSZOOM_DRAWING_OVERLAY_H
