# LetsZoom - C++ MFC 아키텍처 문서

## 1. 기술 스택 선택

### 1.1 C++ MFC를 선택한 이유
- **경량화**: 실행 파일 5MB 이하 목표 달성 가능
- **네이티브 성능**: 화면 확대/그리기에서 최고의 성능
- **Windows 통합**: Windows API 직접 접근
- **낮은 메모리 사용**: Electron 대비 10배 이상 효율적
- **빠른 시작 시간**: < 1초

### 1.2 기술 구성
- **Language**: C++17
- **Framework**: MFC (Microsoft Foundation Classes)
- **Compiler**: Visual Studio 2022 (v143)
- **Windows SDK**: 10.0.22621.0 이상
- **Build System**: MSBuild
- **Runtime**: 정적 링크 (멀티스레드 /MT)

## 2. 아키텍처 개요

```
┌─────────────────────────────────────────────────────────┐
│                    CLetsZoomApp                         │
│              (CWinApp - 메인 애플리케이션)                │
└────────────────────┬────────────────────────────────────┘
                     │
    ┌────────────────┼────────────────┐
    │                │                │
┌───▼──────┐  ┌─────▼──────┐  ┌─────▼─────┐
│  Hotkey  │  │  Settings  │  │   Tray    │
│ Manager  │  │  Manager   │  │  Manager  │
└──────────┘  └────────────┘  └───────────┘
                     │
    ┌────────────────┼────────────────┐
    │                │                │
┌───▼──────┐  ┌─────▼──────┐  ┌─────▼─────┐
│   Zoom   │  │  Drawing   │  │ Screenshot│
│  Overlay │  │   Overlay  │  │  Capture  │
│  Window  │  │   Window   │  │           │
└──────────┘  └────────────┘  └───────────┘
```

## 3. 핵심 클래스 구조

### 3.1 CLetsZoomApp (CWinApp)
```cpp
class CLetsZoomApp : public CWinApp {
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

private:
    CHotkeyManager* m_pHotkeyMgr;
    CSettingsManager* m_pSettingsMgr;
    CTrayManager* m_pTrayMgr;

    // 오버레이 창들
    CZoomOverlayWnd* m_pZoomWnd;
    CDrawingOverlayWnd* m_pDrawWnd;
};
```

### 3.2 CHotkeyManager
```cpp
class CHotkeyManager {
public:
    BOOL RegisterHotkey(int id, UINT modifiers, UINT vk);
    BOOL UnregisterHotkey(int id);
    void HandleHotkey(int id);

private:
    std::map<int, HOTKEY_INFO> m_hotkeys;
    HWND m_hWndReceiver; // 메시지를 받을 숨겨진 창
};
```

### 3.3 CZoomOverlayWnd (전체화면 오버레이)
```cpp
class CZoomOverlayWnd : public CWnd {
public:
    void Show(int zoomLevel = 200);
    void Hide();
    void UpdateZoomLevel(int delta);
    void Pan(int dx, int dy);

protected:
    afx_msg void OnPaint();
    afx_msg void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
    HDC m_hdcScreen;      // 화면 캡처 DC
    HBITMAP m_hbmScreen;  // 화면 비트맵
    int m_nZoomLevel;     // 100 ~ 2000 (1.0x ~ 20.0x)
    CPoint m_ptCenter;    // 확대 중심점

    void CaptureScreen();
    void RenderZoomedView(CDC* pDC);
};
```

### 3.4 CDrawingOverlayWnd (그리기 오버레이)
```cpp
class CDrawingOverlayWnd : public CWnd {
public:
    void Show();
    void Hide();
    void SetDrawTool(DrawTool tool);
    void SetColor(COLORREF color);
    void SetThickness(int thickness);
    void Undo();
    void Redo();
    void Clear();

protected:
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
    std::vector<CStroke*> m_strokes;
    std::stack<CStroke*> m_undoStack;
    DrawTool m_currentTool;
    COLORREF m_color;
    int m_thickness;

    void DrawStroke(CDC* pDC, const CStroke* pStroke);
};
```

### 3.5 CScreenCapture
```cpp
class CScreenCapture {
public:
    static HBITMAP CaptureFullScreen();
    static HBITMAP CaptureRegion(const CRect& rect);
    static BOOL SaveToFile(HBITMAP hbm, LPCTSTR lpszPath);
    static BOOL CopyToClipboard(HBITMAP hbm);

private:
    static BOOL SaveAsPNG(HBITMAP hbm, LPCTSTR lpszPath);
    static BOOL SaveAsJPG(HBITMAP hbm, LPCTSTR lpszPath);
};
```

### 3.6 CSettingsManager
```cpp
class CSettingsManager {
public:
    BOOL LoadSettings();
    BOOL SaveSettings();
    const SETTINGS& GetSettings() const;
    void SetSettings(const SETTINGS& settings);

private:
    SETTINGS m_settings;
    CString m_strConfigPath; // %APPDATA%\LetsZoom\config.ini

    void SetDefaults();
    BOOL ReadFromRegistry();
    BOOL WriteToRegistry();
};
```

### 3.7 CTrayManager
```cpp
class CTrayManager {
public:
    BOOL Initialize(HWND hWnd);
    void Shutdown();
    void ShowNotification(LPCTSTR lpszMsg);

protected:
    afx_msg LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);

private:
    NOTIFYICONDATA m_nid;
    CMenu m_menu;
};
```

## 4. 데이터 구조

### 4.1 설정 구조체
```cpp
struct SETTINGS {
    // 일반
    BOOL bLaunchOnStartup;
    BOOL bShowTrayIcon;
    BOOL bShowNotifications;

    // 확대
    int nDefaultZoomLevel;      // 100 ~ 2000
    int nAnimationDuration;     // 밀리초
    BOOL bSmoothScrolling;

    // 그리기
    COLORREF clrDefault;
    int nDefaultThickness;
    DrawTool toolDefault;

    // 단축키
    HOTKEY hkZoom;              // Ctrl+1
    HOTKEY hkDraw;              // Ctrl+2
    HOTKEY hkZoomAndDraw;       // Ctrl+3
    HOTKEY hkScreenshot;        // Ctrl+4

    // 스크린샷
    CString strSaveFolder;
    ImageFormat imgFormat;      // PNG, JPG
    BOOL bAutoFilename;
};

enum class DrawTool {
    Pen,
    Line,
    Arrow,
    Rectangle,
    Ellipse,
    Text,
    Highlighter,
    Eraser
};

struct CStroke {
    DrawTool tool;
    std::vector<CPoint> points;
    COLORREF color;
    int thickness;
    BYTE opacity;
    CString text; // 텍스트 도구용
};
```

## 5. 핵심 기능 구현

### 5.1 화면 확대 (Magnification)
**방법 1: BitBlt + StretchBlt (권장)**
```cpp
void CZoomOverlayWnd::RenderZoomedView(CDC* pDC) {
    // 1. 화면 전체 캡처
    HDC hdcScreen = ::GetDC(NULL);
    HDC hdcMem = ::CreateCompatibleDC(hdcScreen);

    // 2. 확대할 영역 계산
    CRect rcSource = CalculateSourceRect();

    // 3. 확대 렌더링
    ::StretchBlt(
        pDC->m_hDC,
        0, 0, m_rcClient.Width(), m_rcClient.Height(),
        hdcMem,
        rcSource.left, rcSource.top, rcSource.Width(), rcSource.Height(),
        SRCCOPY
    );

    ::DeleteDC(hdcMem);
    ::ReleaseDC(NULL, hdcScreen);
}
```

**방법 2: Magnification API (고급)**
```cpp
// Windows Magnification API 사용
#include <magnification.h>
#pragma comment(lib, "Magnification.lib")

BOOL CZoomOverlayWnd::InitMagnification() {
    MagInitialize();
    m_hWndMag = CreateWindow(
        WC_MAGNIFIER, TEXT("MagnifierWindow"),
        WS_CHILD | WS_VISIBLE,
        0, 0, width, height,
        m_hWnd, NULL, GetModuleHandle(NULL), NULL
    );

    MagSetWindowFilterList(m_hWndMag, MW_FILTERMODE_EXCLUDE, 0, NULL);
    return TRUE;
}
```

### 5.2 전역 단축키
```cpp
BOOL CHotkeyManager::RegisterHotkey(int id, UINT modifiers, UINT vk) {
    // RegisterHotKey는 전역 단축키를 등록합니다
    if (!::RegisterHotKey(m_hWndReceiver, id, modifiers, vk)) {
        DWORD dwErr = GetLastError();
        // 충돌 처리
        return FALSE;
    }

    HOTKEY_INFO info = {id, modifiers, vk};
    m_hotkeys[id] = info;
    return TRUE;
}

// 메시지 핸들러
LRESULT CMessageWnd::OnHotkey(WPARAM wParam, LPARAM lParam) {
    int id = (int)wParam;
    theApp.m_pHotkeyMgr->HandleHotkey(id);
    return 0;
}
```

### 5.3 투명 오버레이 창
```cpp
BOOL CZoomOverlayWnd::Create() {
    // 레이어드 윈도우로 생성 (WS_EX_LAYERED)
    DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT;

    if (!CreateEx(
        dwExStyle,
        AfxRegisterWndClass(0),
        _T("ZoomOverlay"),
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL))
    {
        return FALSE;
    }

    // 투명도 설정 (0 ~ 255)
    ::SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);

    return TRUE;
}
```

### 5.4 화면 캡처
```cpp
HBITMAP CScreenCapture::CaptureFullScreen() {
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbm = CreateCompatibleBitmap(hdcScreen, nScreenWidth, nScreenHeight);

    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);

    BitBlt(hdcMem, 0, 0, nScreenWidth, nScreenHeight, hdcScreen, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    return hbm;
}

BOOL CScreenCapture::SaveAsPNG(HBITMAP hbm, LPCTSTR lpszPath) {
    // GDI+를 사용하여 PNG로 저장
    Gdiplus::Bitmap bitmap(hbm, NULL);

    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);

    return bitmap.Save(lpszPath, &pngClsid, NULL) == Gdiplus::Ok;
}
```

### 5.5 그리기 구현
```cpp
void CDrawingOverlayWnd::OnLButtonDown(UINT nFlags, CPoint point) {
    m_bDrawing = TRUE;

    CStroke* pStroke = new CStroke();
    pStroke->tool = m_currentTool;
    pStroke->color = m_color;
    pStroke->thickness = m_thickness;
    pStroke->points.push_back(point);

    m_currentStroke = pStroke;
}

void CDrawingOverlayWnd::OnMouseMove(UINT nFlags, CPoint point) {
    if (!m_bDrawing) return;

    m_currentStroke->points.push_back(point);

    // 증분 렌더링 (전체 다시 그리지 않음)
    CClientDC dc(this);
    DrawLastSegment(&dc, m_currentStroke);
}

void CDrawingOverlayWnd::DrawStroke(CDC* pDC, const CStroke* pStroke) {
    CPen pen(PS_SOLID, pStroke->thickness, pStroke->color);
    CPen* pOldPen = pDC->SelectObject(&pen);

    switch (pStroke->tool) {
        case DrawTool::Pen:
            DrawPenStroke(pDC, pStroke);
            break;
        case DrawTool::Line:
            DrawLineStroke(pDC, pStroke);
            break;
        case DrawTool::Arrow:
            DrawArrowStroke(pDC, pStroke);
            break;
        case DrawTool::Rectangle:
            DrawRectangleStroke(pDC, pStroke);
            break;
        // ... 기타 도구들
    }

    pDC->SelectObject(pOldPen);
}
```

## 6. 성능 최적화

### 6.1 더블 버퍼링
```cpp
void CZoomOverlayWnd::OnPaint() {
    CPaintDC dc(this);

    // 더블 버퍼링
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);

    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

    // 오프스크린에 렌더링
    RenderZoomedView(&memDC);

    // 화면에 복사
    dc.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldBitmap);
}
```

### 6.2 부분 렌더링
```cpp
void CDrawingOverlayWnd::InvalidateStrokeRect(const CStroke* pStroke) {
    CRect rcInvalid = CalculateStrokeBounds(pStroke);
    rcInvalid.InflateRect(10, 10); // 여유 공간
    InvalidateRect(&rcInvalid);
}
```

### 6.3 메모리 관리
```cpp
// 스마트 포인터 사용
class CAutoDeletePtr {
public:
    ~CAutoDeletePtr() {
        for (auto* p : m_strokes) {
            delete p;
        }
    }
private:
    std::vector<CStroke*> m_strokes;
};
```

## 7. 용량 최적화 (5MB 이하 목표)

### 7.1 컴파일 옵션
```
- Configuration: Release
- Platform Toolset: v143
- Character Set: Unicode
- Runtime Library: Multi-threaded (/MT) - 정적 링크
- Optimization: Minimize Size (/O1)
- Link Time Code Generation: Yes
- Remove Unreferenced Code: Yes
```

### 7.2 링커 옵션
```
/SUBSYSTEM:WINDOWS
/OPT:REF
/OPT:ICF
/LTCG
/DYNAMICBASE
/NXCOMPAT
```

### 7.3 제외할 기능
- MFC 정적 링크 대신 MFC 공유 DLL 사용 (선택 사항)
- 불필요한 Windows 기능 제외
- 리소스 압축 (UPX 등)

### 7.4 예상 크기
```
코드 섹션:           ~1.5 MB
리소스 (아이콘 등):   ~0.3 MB
MFC 정적 링크:       ~2.5 MB (또는 공유 DLL 사용 시 ~0.5 MB)
GDI+ 및 기타:        ~0.5 MB
------------------------
총 예상 크기:        ~3.8 MB (압축 전)
UPX 압축 후:         ~2.0 MB
```

## 8. 프로젝트 구조

```
LetsZoom/
├── src/
│   ├── LetsZoom.cpp              // CWinApp
│   ├── LetsZoom.h
│   ├── MainFrm.cpp               // 숨겨진 메인 프레임
│   ├── MainFrm.h
│   ├── Managers/
│   │   ├── HotkeyManager.cpp
│   │   ├── HotkeyManager.h
│   │   ├── SettingsManager.cpp
│   │   ├── SettingsManager.h
│   │   ├── TrayManager.cpp
│   │   └── TrayManager.h
│   ├── Overlays/
│   │   ├── ZoomOverlayWnd.cpp
│   │   ├── ZoomOverlayWnd.h
│   │   ├── DrawingOverlayWnd.cpp
│   │   └── DrawingOverlayWnd.h
│   ├── Capture/
│   │   ├── ScreenCapture.cpp
│   │   └── ScreenCapture.h
│   ├── Dialogs/
│   │   ├── SettingsDlg.cpp
│   │   └── SettingsDlg.h
│   └── Utils/
│       ├── Common.h
│       └── GdiPlus.cpp
├── res/
│   ├── LetsZoom.ico
│   ├── LetsZoom.rc
│   └── resource.h
├── docs/
│   ├── PRD.md
│   ├── ARCHITECTURE_MFC.md
│   └── IMPLEMENTATION_PLAN_MFC.md
└── LetsZoom.sln                  // Visual Studio 솔루션
```

## 9. 빌드 및 배포

### 9.1 빌드 요구사항
- Visual Studio 2022 (Community 이상)
- Windows SDK 10.0.22621.0
- C++17 지원

### 9.2 빌드 명령
```cmd
# Release 빌드
msbuild LetsZoom.sln /p:Configuration=Release /p:Platform=x64

# 압축 (선택 사항)
upx --best --lzma LetsZoom.exe
```

### 9.3 배포 형태
1. **단일 실행 파일**: LetsZoom.exe (< 5MB)
2. **설치 프로그램**: Inno Setup 또는 WiX
3. **포터블 버전**: ZIP 파일

### 9.4 디지털 서명
```cmd
signtool sign /f certificate.pfx /p password /t http://timestamp.digicert.com LetsZoom.exe
```

## 10. 개발 로드맵

### Phase 1: 프로젝트 셋업 (2-3일)
- Visual Studio 프로젝트 생성
- MFC 애플리케이션 기본 구조
- 트레이 아이콘 및 메뉴

### Phase 2: 핵심 기능 (1주)
- 전역 단축키 등록
- 화면 확대 오버레이
- 기본 그리기 도구 (펜, 선)

### Phase 3: 고급 기능 (1주)
- 고급 그리기 도구 (화살표, 도형, 텍스트)
- Undo/Redo 스택
- 설정 저장/불러오기

### Phase 4: UI 개선 (3-4일)
- 설정 대화상자
- 도구 팔레트
- 키보드 단축키 커스터마이징

### Phase 5: 테스트 및 최적화 (3-4일)
- 메모리 누수 체크 (Valgrind, Dr. Memory)
- 성능 프로파일링
- 다양한 해상도/DPI 테스트
- 용량 최적화

### Phase 6: 배포 준비 (2-3일)
- 설치 프로그램 제작
- 사용자 매뉴얼
- 라이선스 및 저작권
- GitHub 릴리스

## 11. 주요 도전 과제 및 해결책

### 11.1 고DPI 지원
```cpp
// DPI 인식 설정
void CLetsZoomApp::InitInstance() {
    SetProcessDPIAware();

    // 또는 매니페스트에 추가:
    // <dpiAware>True/PM</dpiAware>
}
```

### 11.2 멀티모니터 지원
```cpp
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor,
                               LPRECT lprcMonitor, LPARAM dwData) {
    MONITORINFOEX mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);

    // 각 모니터별 처리
    return TRUE;
}

EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
```

### 11.3 메모리 효율
- 화면 캡처 후 즉시 이전 비트맵 해제
- 그리기 스트로크 개수 제한 (예: 최대 1000개)
- 타이머 기반 자동 메모리 정리

### 11.4 반응성 유지
- UI 스레드와 렌더링 스레드 분리
- 비동기 파일 저장
- 화면 캡처를 백그라운드 스레드에서 수행

## 12. 보안 고려사항

### 12.1 권한 최소화
- 관리자 권한 불필요
- UIPI (User Interface Privilege Isolation) 고려

### 12.2 안전한 파일 저장
```cpp
BOOL SafeSaveFile(LPCTSTR lpszPath, const void* pData, size_t size) {
    // 경로 검증
    if (!PathIsValid(lpszPath)) return FALSE;

    // 덮어쓰기 확인
    if (PathFileExists(lpszPath)) {
        if (MessageBox(NULL, _T("파일을 덮어쓰시겠습니까?"),
                       _T("확인"), MB_YESNO) != IDYES) {
            return FALSE;
        }
    }

    // 파일 쓰기
    return WriteFile(...);
}
```

## 13. 성능 목표

| 항목 | 목표 | 측정 방법 |
|------|------|-----------|
| 실행 파일 크기 | < 5MB | Release 빌드 확인 |
| 시작 시간 | < 500ms | 프로파일러 |
| 메모리 (대기) | < 10MB | Task Manager |
| 메모리 (활성) | < 50MB | Task Manager |
| 확대 응답 | < 100ms | 타이머 측정 |
| 그리기 FPS | > 60fps | 프레임 카운터 |
| CPU (대기) | < 1% | Task Manager |
| CPU (활성) | < 10% | Task Manager |

## 14. 테스트 전략

### 14.1 단위 테스트
- Google Test 프레임워크
- 핵심 로직 테스트 (설정, 단축키 관리)

### 14.2 통합 테스트
- 단축키 → 오버레이 창 생성
- 그리기 → 저장 → 불러오기

### 14.3 성능 테스트
- 다양한 화면 해상도 (FHD, QHD, 4K)
- 장시간 실행 (메모리 누수 체크)
- CPU/메모리 프로파일링

### 14.4 호환성 테스트
- Windows 10 (1809, 21H2)
- Windows 11
- 다양한 DPI 설정 (100%, 125%, 150%, 200%)

## 15. 참고 자료

- [MFC Desktop Applications](https://docs.microsoft.com/en-us/cpp/mfc/mfc-desktop-applications)
- [Windows Magnification API](https://docs.microsoft.com/en-us/windows/win32/api/_magapi/)
- [GDI+ Programming](https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-gdi-start)
- [RegisterHotKey Function](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey)
