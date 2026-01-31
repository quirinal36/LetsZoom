/**
 * zoom_overlay.h - 화면 확대 오버레이 윈도우
 */

#ifndef LETSZOOM_ZOOM_OVERLAY_H
#define LETSZOOM_ZOOM_OVERLAY_H

#include <windows.h>
#include <stdbool.h>

/**
 * 확대 오버레이 초기화
 */
bool ZoomOverlay_Initialize(HINSTANCE hInstance);

/**
 * 확대 오버레이 정리
 */
void ZoomOverlay_Shutdown(void);

/**
 * 확대 모드 시작
 * zoomLevel: 확대 배율 (100-800%)
 * smoothZoom: 부드러운 확대 활성화
 */
bool ZoomOverlay_Show(int zoomLevel, bool smoothZoom);

/**
 * 확대 모드 종료
 */
void ZoomOverlay_Hide(void);

/**
 * 확대 모드 활성 상태 확인
 */
bool ZoomOverlay_IsActive(void);

#endif // LETSZOOM_ZOOM_OVERLAY_H
