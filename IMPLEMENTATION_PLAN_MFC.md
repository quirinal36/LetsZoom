# LetsZoom - C++ MFC 구현 계획

## 1. 개발 환경 설정

### 1.1 필수 도구
```
✓ Visual Studio 2022 (Community 이상)
✓ Desktop development with C++ workload
✓ Windows 10 SDK (10.0.22621.0)
✓ MFC and ATL support
```

### 1.2 프로젝트 생성 단계
1. Visual Studio 2022 실행
2. "새 프로젝트 만들기"
3. "MFC 애플리케이션" 선택
4. 프로젝트 이름: LetsZoom
5. 애플리케이션 형식: **단일 문서**
6. 프로젝트 스타일: MFC 표준
7. 고급 기능:
   - [ ] 인쇄 및 인쇄 미리 보기 지원 제거
   - [x] ActiveX 컨트롤 사용 안 함
   - [x] 공통 컨트롤 매니페스트

### 1.3 프로젝트 설정

#### Release 구성 (용량 최적화)
```
Configuration Properties > General
- Platform Toolset: Visual Studio 2022 (v143)
- Character Set: Use Unicode Character Set
- MFC 사용: Use MFC in a Shared DLL (or Static Library)

C/C++ > General
- Additional Include Directories: $(ProjectDir)src

C/C++ > Optimization
- Optimization: Minimize Size (/O1)
- Inline Function Expansion: Only __inline
- Enable Intrinsic Functions: Yes
- Favor Size Or Speed: Favor Small Code (/Os)
- Whole Program Optimization: Yes

C/C++ > Code Generation
- Runtime Library: Multi-threaded (/MT)
- Security Check: Disable Security Check (/GS-)

C/C++ > Language
- C++ Language Standard: ISO C++17 Standard

Linker > General
- Enable Incremental Linking: No

Linker > Optimization
- References: Yes (/OPT:REF)
- Enable COMDAT Folding: Yes (/OPT:ICF)
- Link Time Code Generation: Use Link Time Code Generation

Linker > Advanced
- Entry Point: wWinMainCRTStartup

Linker > System
- SubSystem: Windows (/SUBSYSTEM:WINDOWS)
```

## 2. 단계별 구현 계획

### Phase 1: 기본 구조 (Day 1-2)

#### Task 1.1: 프로젝트 생성 및 설정
```cpp
// LetsZoomApp.h
class CLetsZoomApp : public CWinApp {
public:
    CLetsZoomApp();
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};
```

**체크리스트**:
- [ ] Visual Studio 프로젝트 생성
- [ ] Release 빌드 설정 구성
- [ ] 기본 리소스 파일 (아이콘, 메뉴)
- [ ] 버전 정보 추가

#### Task 1.2: 숨겨진 메인 창
```cpp
// MainFrm.h
class CMainFrame : public CFrameWnd {
public:
    CMainFrame();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg LRESULT OnHotkey(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};
```

**구현**:
```cpp
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
    // 창을 화면 밖에 생성 (숨김)
    cs.x = -32000;
    cs.y = -32000;
    cs.cx = 1;
    cs.cy = 1;
    cs.style = WS_OVERLAPPED; // 최소화

    return CFrameWnd::PreCreateWindow(cs);
}
```

**체크리스트**:
- [ ] 메인 프레임 창 생성 (숨김 상태)
- [ ] WM_HOTKEY 메시지 핸들러
- [ ] WM_CLOSE 최소화 처리

#### Task 1.3: 트레이 아이콘
```cpp
// TrayManager.h
class CTrayManager {
public:
    CTrayManager();
    ~CTrayManager();

    BOOL Initialize(HWND hWnd, UINT uCallbackMsg);
    void Shutdown();
    void ShowNotification(LPCTSTR lpszMsg, LPCTSTR lpszTitle);
    void UpdateIcon(HICON hIcon);
    void UpdateTooltip(LPCTSTR lpszTooltip);

private:
    NOTIFYICONDATA m_nid;
    HWND m_hWnd;
    UINT m_uCallbackMsg;
    CMenu m_contextMenu;

    void CreateContextMenu();
};
```

**구현**:
```cpp
BOOL CTrayManager::Initialize(HWND hWnd, UINT uCallbackMsg) {
    m_hWnd = hWnd;
    m_uCallbackMsg = uCallbackMsg;

    ZeroMemory(&m_nid, sizeof(m_nid));
    m_nid.cbSize = sizeof(NOTIFYICONDATA);
    m_nid.hWnd = hWnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = uCallbackMsg;
    m_nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    _tcscpy_s(m_nid.szTip, _T("LetsZoom"));

    return Shell_NotifyIcon(NIM_ADD, &m_nid);
}

void CTrayManager::CreateContextMenu() {
    m_contextMenu.CreatePopupMenu();
    m_contextMenu.AppendMenu(MF_STRING, ID_SETTINGS, _T("설정(&S)"));
    m_contextMenu.AppendMenu(MF_SEPARATOR);
    m_contextMenu.AppendMenu(MF_STRING, ID_APP_EXIT, _T("종료(&X)"));
}
```

**체크리스트**:
- [ ] Shell_NotifyIcon 구현
- [ ] 트레이 아이콘 우클릭 메뉴
- [ ] 알림 기능 (balloon tip)

---

### Phase 2: 전역 단축키 (Day 3)

#### Task 2.1: HotkeyManager 구현
```cpp
// HotkeyManager.h
#define WM_HOTKEY_PRESSED (WM_USER + 100)

enum HotkeyID {
    HOTKEY_ZOOM = 1,
    HOTKEY_DRAW = 2,
    HOTKEY_ZOOM_AND_DRAW = 3,
    HOTKEY_SCREENSHOT = 4
};

class CHotkeyManager {
public:
    CHotkeyManager();
    ~CHotkeyManager();

    BOOL Initialize(HWND hWndReceiver);
    void Shutdown();

    BOOL RegisterAllHotkeys(const SETTINGS& settings);
    BOOL RegisterHotkey(int id, UINT modifiers, UINT vk);
    BOOL UnregisterHotkey(int id);
    void UnregisterAllHotkeys();

    static CString FormatHotkey(UINT modifiers, UINT vk);
    static BOOL ParseHotkey(LPCTSTR lpszHotkey, UINT& modifiers, UINT& vk);

private:
    HWND m_hWndReceiver;
    std::map<int, std::pair<UINT, UINT>> m_registeredHotkeys; // id -> (modifiers, vk)
};
```

**구현**:
```cpp
BOOL CHotkeyManager::RegisterHotkey(int id, UINT modifiers, UINT vk) {
    // 이미 등록된 단축키 해제
    UnregisterHotkey(id);

    // 새 단축키 등록
    if (!::RegisterHotKey(m_hWndReceiver, id, modifiers, vk)) {
        DWORD dwErr = GetLastError();
        CString strMsg;
        strMsg.Format(_T("단축키 등록 실패: 0x%08X"), dwErr);
        AfxMessageBox(strMsg, MB_ICONERROR);
        return FALSE;
    }

    m_registeredHotkeys[id] = std::make_pair(modifiers, vk);
    return TRUE;
}

// MainFrm.cpp
LRESULT CMainFrame::OnHotkey(WPARAM wParam, LPARAM lParam) {
    int nHotkeyID = (int)wParam;

    switch (nHotkeyID) {
        case HOTKEY_ZOOM:
            OnZoomMode();
            break;
        case HOTKEY_DRAW:
            OnDrawMode();
            break;
        case HOTKEY_ZOOM_AND_DRAW:
            OnZoomAndDrawMode();
            break;
        case HOTKEY_SCREENSHOT:
            OnScreenshot();
            break;
    }

    return 0;
}
```

**체크리스트**:
- [ ] RegisterHotKey API 구현
- [ ] 단축키 충돌 처리
- [ ] 단축키 문자열 파싱 (예: "Ctrl+Shift+Z")
- [ ] 메시지 핸들러 연결

---

### Phase 3: 설정 관리 (Day 3-4)

#### Task 3.1: SettingsManager 구현
```cpp
// SettingsManager.h
struct HOTKEY_SETTING {
    UINT modifiers; // MOD_CONTROL, MOD_SHIFT, MOD_ALT, MOD_WIN
    UINT vk;        // Virtual key code
};

struct SETTINGS {
    // 일반
    BOOL bLaunchOnStartup;
    BOOL bShowTrayIcon;
    BOOL bShowNotifications;

    // 확대
    int nDefaultZoomLevel;      // 100 ~ 2000 (1.0x ~ 20.0x)
    int nAnimationDuration;     // 밀리초 (0 ~ 1000)
    BOOL bSmoothScrolling;

    // 그리기
    COLORREF clrDefault;
    int nDefaultThickness;      // 2, 4, 8, 16
    DrawTool toolDefault;

    // 단축키
    HOTKEY_SETTING hkZoom;
    HOTKEY_SETTING hkDraw;
    HOTKEY_SETTING hkZoomAndDraw;
    HOTKEY_SETTING hkScreenshot;

    // 스크린샷
    CString strSaveFolder;
    int nImageFormat;           // 0=PNG, 1=JPG
    BOOL bAutoFilename;
    CString strFilenamePattern;
};

class CSettingsManager {
public:
    CSettingsManager();

    BOOL LoadSettings();
    BOOL SaveSettings();

    const SETTINGS& GetSettings() const { return m_settings; }
    void SetSettings(const SETTINGS& settings);

private:
    SETTINGS m_settings;
    CString m_strConfigPath;

    void SetDefaults();
    BOOL LoadFromFile();
    BOOL SaveToFile();
};
```

**구현 (INI 파일 사용)**:
```cpp
BOOL CSettingsManager::LoadFromFile() {
    // %APPDATA%\LetsZoom\config.ini
    TCHAR szPath[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
    m_strConfigPath.Format(_T("%s\\LetsZoom\\config.ini"), szPath);

    // 파일이 없으면 기본값 사용
    if (!PathFileExists(m_strConfigPath)) {
        SetDefaults();
        return TRUE;
    }

    // 일반 설정
    m_settings.bLaunchOnStartup = GetPrivateProfileInt(
        _T("General"), _T("LaunchOnStartup"), TRUE, m_strConfigPath);

    // 확대 설정
    m_settings.nDefaultZoomLevel = GetPrivateProfileInt(
        _T("Zoom"), _T("DefaultZoomLevel"), 200, m_strConfigPath);

    // 단축키 설정
    TCHAR szHotkey[32];
    GetPrivateProfileString(_T("Hotkeys"), _T("Zoom"),
        _T("Ctrl+1"), szHotkey, 32, m_strConfigPath);
    ParseHotkey(szHotkey, m_settings.hkZoom);

    // ... 기타 설정

    return TRUE;
}

BOOL CSettingsManager::SaveToFile() {
    // 디렉토리 생성
    CString strDir = m_strConfigPath.Left(m_strConfigPath.ReverseFind('\\'));
    SHCreateDirectoryEx(NULL, strDir, NULL);

    // 일반 설정
    WritePrivateProfileString(_T("General"), _T("LaunchOnStartup"),
        m_settings.bLaunchOnStartup ? _T("1") : _T("0"), m_strConfigPath);

    // 확대 설정
    CString strValue;
    strValue.Format(_T("%d"), m_settings.nDefaultZoomLevel);
    WritePrivateProfileString(_T("Zoom"), _T("DefaultZoomLevel"),
        strValue, m_strConfigPath);

    // ... 기타 설정

    return TRUE;
}
```

**체크리스트**:
- [ ] SETTINGS 구조체 정의
- [ ] INI 파일 읽기/쓰기
- [ ] 기본값 설정
- [ ] %APPDATA%\LetsZoom 폴더 생성

---

### Phase 4: 화면 확대 (Day 5-7)

#### Task 4.1: ZoomOverlayWnd 구조
```cpp
// ZoomOverlayWnd.h
class CZoomOverlayWnd : public CWnd {
public:
    CZoomOverlayWnd();
    virtual ~CZoomOverlayWnd();

    BOOL Create();
    void Show(int nZoomLevel = 200);
    void Hide();

    void SetZoomLevel(int nLevel);
    void AdjustZoomLevel(int nDelta);
    void Pan(int dx, int dy);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    DECLARE_MESSAGE_MAP()

private:
    // 화면 캡처
    HDC m_hdcScreen;
    HBITMAP m_hbmScreen;
    int m_nScreenWidth;
    int m_nScreenHeight;

    // 확대 상태
    int m_nZoomLevel;       // 100 ~ 2000
    CPoint m_ptCenter;      // 화면 좌표계
    BOOL m_bPanning;
    CPoint m_ptLastMouse;

    // 렌더링
    CDC m_dcMem;            // 메모리 DC
    CBitmap m_bmpMem;       // 더블 버퍼링

    void CaptureScreen();
    void ReleaseScreenCapture();
    void RenderZoomedView(CDC* pDC);
    CRect CalculateSourceRect();
};
```

#### Task 4.2: 화면 캡처 구현
```cpp
void CZoomOverlayWnd::CaptureScreen() {
    // 화면 크기
    m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    // 화면 DC 가져오기
    m_hdcScreen = ::GetDC(NULL);
    HDC hdcMem = ::CreateCompatibleDC(m_hdcScreen);

    // 비트맵 생성
    m_hbmScreen = ::CreateCompatibleBitmap(m_hdcScreen, m_nScreenWidth, m_nScreenHeight);
    HBITMAP hbmOld = (HBITMAP)::SelectObject(hdcMem, m_hbmScreen);

    // 화면 복사
    ::BitBlt(hdcMem, 0, 0, m_nScreenWidth, m_nScreenHeight,
             m_hdcScreen, 0, 0, SRCCOPY);

    ::SelectObject(hdcMem, hbmOld);
    ::DeleteDC(hdcMem);
}

void CZoomOverlayWnd::ReleaseScreenCapture() {
    if (m_hbmScreen) {
        ::DeleteObject(m_hbmScreen);
        m_hbmScreen = NULL;
    }
    if (m_hdcScreen) {
        ::ReleaseDC(NULL, m_hdcScreen);
        m_hdcScreen = NULL;
    }
}
```

#### Task 4.3: 확대 렌더링
```cpp
void CZoomOverlayWnd::OnPaint() {
    CPaintDC dc(this);

    if (!m_hbmScreen) return;

    // 클라이언트 영역
    CRect rcClient;
    GetClientRect(&rcClient);

    // 메모리 DC에 렌더링 (더블 버퍼링)
    CDC dcMem;
    dcMem.CreateCompatibleDC(&dc);
    CBitmap bmpMem;
    bmpMem.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
    CBitmap* pOldBmp = dcMem.SelectObject(&bmpMem);

    // 배경 검정
    dcMem.FillSolidRect(&rcClient, RGB(0, 0, 0));

    // 확대된 화면 렌더링
    RenderZoomedView(&dcMem);

    // 화면에 복사
    dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);

    dcMem.SelectObject(pOldBmp);
}

void CZoomOverlayWnd::RenderZoomedView(CDC* pDC) {
    // 확대할 소스 영역 계산
    CRect rcSource = CalculateSourceRect();

    // 클라이언트 영역
    CRect rcClient;
    GetClientRect(&rcClient);

    // 화면 비트맵에서 확대하여 렌더링
    CDC dcScreen;
    dcScreen.CreateCompatibleDC(pDC);
    HBITMAP hbmOld = (HBITMAP)::SelectObject(dcScreen, m_hbmScreen);

    // StretchBlt로 확대
    pDC->SetStretchBltMode(HALFTONE);
    pDC->StretchBlt(
        0, 0, rcClient.Width(), rcClient.Height(),
        &dcScreen,
        rcSource.left, rcSource.top, rcSource.Width(), rcSource.Height(),
        SRCCOPY
    );

    ::SelectObject(dcScreen, hbmOld);

    // 십자선 그리기 (중심 표시)
    DrawCrosshair(pDC);
}

CRect CZoomOverlayWnd::CalculateSourceRect() {
    CRect rcClient;
    GetClientRect(&rcClient);

    // 확대 비율 (1.0 = 100%)
    double fZoom = m_nZoomLevel / 100.0;

    // 소스 영역 크기
    int nSrcWidth = (int)(rcClient.Width() / fZoom);
    int nSrcHeight = (int)(rcClient.Height() / fZoom);

    // 중심점 기준으로 소스 영역 계산
    CRect rcSource(
        m_ptCenter.x - nSrcWidth / 2,
        m_ptCenter.y - nSrcHeight / 2,
        m_ptCenter.x + nSrcWidth / 2,
        m_ptCenter.y + nSrcHeight / 2
    );

    // 화면 경계 클리핑
    rcSource.left = max(0, rcSource.left);
    rcSource.top = max(0, rcSource.top);
    rcSource.right = min(m_nScreenWidth, rcSource.right);
    rcSource.bottom = min(m_nScreenHeight, rcSource.bottom);

    return rcSource;
}
```

#### Task 4.4: 마우스 상호작용
```cpp
void CZoomOverlayWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
    // 줌 레벨 조정
    int nDelta = (zDelta > 0) ? 10 : -10;
    AdjustZoomLevel(nDelta);

    Invalidate(FALSE);
}

void CZoomOverlayWnd::OnLButtonDown(UINT nFlags, CPoint point) {
    // 패닝 시작
    m_bPanning = TRUE;
    m_ptLastMouse = point;
    SetCapture();
}

void CZoomOverlayWnd::OnMouseMove(UINT nFlags, CPoint point) {
    if (!m_bPanning) return;

    // 이동 거리 계산
    int dx = point.x - m_ptLastMouse.x;
    int dy = point.y - m_ptLastMouse.y;

    // 확대 비율에 따라 이동 속도 조정
    double fZoom = m_nZoomLevel / 100.0;
    m_ptCenter.x -= (int)(dx / fZoom);
    m_ptCenter.y -= (int)(dy / fZoom);

    // 화면 경계 체크
    m_ptCenter.x = max(0, min(m_nScreenWidth, m_ptCenter.x));
    m_ptCenter.y = max(0, min(m_nScreenHeight, m_ptCenter.y));

    m_ptLastMouse = point;
    Invalidate(FALSE);
}

void CZoomOverlayWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    switch (nChar) {
        case VK_ESCAPE:
            Hide();
            break;
        case VK_ADD:      // + 키
        case VK_OEM_PLUS: // = 키
            AdjustZoomLevel(10);
            Invalidate(FALSE);
            break;
        case VK_SUBTRACT: // - 키
        case VK_OEM_MINUS:
            AdjustZoomLevel(-10);
            Invalidate(FALSE);
            break;
        case VK_UP:
            Pan(0, -10);
            break;
        case VK_DOWN:
            Pan(0, 10);
            break;
        case VK_LEFT:
            Pan(-10, 0);
            break;
        case VK_RIGHT:
            Pan(10, 0);
            break;
    }
}
```

**체크리스트**:
- [ ] 전체화면 오버레이 창 생성
- [ ] 화면 캡처 (BitBlt)
- [ ] 확대 렌더링 (StretchBlt)
- [ ] 마우스 휠로 줌 조절
- [ ] 마우스 드래그로 패닝
- [ ] ESC 키로 종료

---

### Phase 5: 그리기 기능 (Day 8-10)

#### Task 5.1: DrawingOverlayWnd 구조
```cpp
// DrawingOverlayWnd.h
enum class DrawTool {
    None = 0,
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

    CStroke() : tool(DrawTool::Pen), color(RGB(255, 0, 0)),
                thickness(4), opacity(255) {}
};

class CDrawingOverlayWnd : public CWnd {
public:
    CDrawingOverlayWnd();
    virtual ~CDrawingOverlayWnd();

    BOOL Create();
    void Show();
    void Hide();

    // 도구 설정
    void SetTool(DrawTool tool);
    void SetColor(COLORREF color);
    void SetThickness(int thickness);
    void SetOpacity(BYTE opacity);

    // 편집
    void Undo();
    void Redo();
    void Clear();

protected:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    DECLARE_MESSAGE_MAP()

private:
    // 그리기 상태
    BOOL m_bDrawing;
    DrawTool m_currentTool;
    COLORREF m_color;
    int m_thickness;
    BYTE m_opacity;

    // 스트로크
    std::vector<CStroke*> m_strokes;
    std::vector<CStroke*> m_redoStack;
    CStroke* m_currentStroke;

    // 렌더링
    CDC m_dcMem;
    CBitmap m_bmpMem;

    void DrawStroke(CDC* pDC, const CStroke* pStroke);
    void DrawPen(CDC* pDC, const CStroke* pStroke);
    void DrawLine(CDC* pDC, const CStroke* pStroke);
    void DrawArrow(CDC* pDC, const CStroke* pStroke);
    void DrawRectangle(CDC* pDC, const CStroke* pStroke);
    void DrawEllipse(CDC* pDC, const CStroke* pStroke);
    void DrawText(CDC* pDC, const CStroke* pStroke);

    void ReleaseStrokes();
};
```

#### Task 5.2: 투명 오버레이 창 생성
```cpp
BOOL CDrawingOverlayWnd::Create() {
    // 레이어드 윈도우 스타일
    DWORD dwExStyle = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT;

    // 전체화면 크기
    int nWidth = GetSystemMetrics(SM_CXSCREEN);
    int nHeight = GetSystemMetrics(SM_CYSCREEN);

    // 윈도우 클래스 등록
    LPCTSTR lpszClass = AfxRegisterWndClass(
        CS_DBLCLKS,
        LoadCursor(NULL, IDC_CROSS),
        NULL,
        NULL
    );

    // 창 생성
    if (!CreateEx(dwExStyle, lpszClass, _T("DrawingOverlay"),
                  WS_POPUP | WS_VISIBLE,
                  0, 0, nWidth, nHeight,
                  NULL, NULL)) {
        return FALSE;
    }

    // 완전 투명 (255 = 불투명)
    ::SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);

    // 더블 버퍼링 준비
    CClientDC dc(this);
    m_dcMem.CreateCompatibleDC(&dc);
    m_bmpMem.CreateCompatibleBitmap(&dc, nWidth, nHeight);
    m_dcMem.SelectObject(&m_bmpMem);

    // 배경 투명
    m_dcMem.FillSolidRect(0, 0, nWidth, nHeight, RGB(0, 0, 0));

    return TRUE;
}
```

#### Task 5.3: 그리기 구현
```cpp
void CDrawingOverlayWnd::OnLButtonDown(UINT nFlags, CPoint point) {
    m_bDrawing = TRUE;

    m_currentStroke = new CStroke();
    m_currentStroke->tool = m_currentTool;
    m_currentStroke->color = m_color;
    m_currentStroke->thickness = m_thickness;
    m_currentStroke->opacity = m_opacity;
    m_currentStroke->points.push_back(point);

    SetCapture();
}

void CDrawingOverlayWnd::OnMouseMove(UINT nFlags, CPoint point) {
    if (!m_bDrawing) return;

    m_currentStroke->points.push_back(point);

    // 실시간 렌더링 (펜 도구의 경우)
    if (m_currentTool == DrawTool::Pen) {
        CClientDC dc(this);
        DrawStroke(&dc, m_currentStroke);
    } else {
        // 다른 도구는 전체 다시 그리기
        Invalidate(FALSE);
    }
}

void CDrawingOverlayWnd::OnLButtonUp(UINT nFlags, CPoint point) {
    if (!m_bDrawing) return;

    m_bDrawing = FALSE;
    ReleaseCapture();

    m_currentStroke->points.push_back(point);
    m_strokes.push_back(m_currentStroke);
    m_currentStroke = NULL;

    // Redo 스택 클리어
    for (auto* p : m_redoStack) delete p;
    m_redoStack.clear();

    Invalidate(FALSE);
}

void CDrawingOverlayWnd::OnPaint() {
    CPaintDC dc(this);

    // 배경 투명
    CRect rcClient;
    GetClientRect(&rcClient);

    // 모든 스트로크 그리기
    for (const auto* pStroke : m_strokes) {
        DrawStroke(&dc, pStroke);
    }

    // 현재 그리는 중인 스트로크
    if (m_bDrawing && m_currentStroke) {
        DrawStroke(&dc, m_currentStroke);
    }
}

void CDrawingOverlayWnd::DrawStroke(CDC* pDC, const CStroke* pStroke) {
    switch (pStroke->tool) {
        case DrawTool::Pen:
            DrawPen(pDC, pStroke);
            break;
        case DrawTool::Line:
            DrawLine(pDC, pStroke);
            break;
        case DrawTool::Arrow:
            DrawArrow(pDC, pStroke);
            break;
        case DrawTool::Rectangle:
            DrawRectangle(pDC, pStroke);
            break;
        case DrawTool::Ellipse:
            DrawEllipse(pDC, pStroke);
            break;
    }
}

void CDrawingOverlayWnd::DrawPen(CDC* pDC, const CStroke* pStroke) {
    if (pStroke->points.size() < 2) return;

    CPen pen(PS_SOLID, pStroke->thickness, pStroke->color);
    CPen* pOldPen = pDC->SelectObject(&pen);

    pDC->MoveTo(pStroke->points[0]);
    for (size_t i = 1; i < pStroke->points.size(); ++i) {
        pDC->LineTo(pStroke->points[i]);
    }

    pDC->SelectObject(pOldPen);
}

void CDrawingOverlayWnd::DrawLine(CDC* pDC, const CStroke* pStroke) {
    if (pStroke->points.size() < 2) return;

    CPen pen(PS_SOLID, pStroke->thickness, pStroke->color);
    CPen* pOldPen = pDC->SelectObject(&pen);

    CPoint ptStart = pStroke->points.front();
    CPoint ptEnd = pStroke->points.back();

    pDC->MoveTo(ptStart);
    pDC->LineTo(ptEnd);

    pDC->SelectObject(pOldPen);
}

void CDrawingOverlayWnd::DrawArrow(CDC* pDC, const CStroke* pStroke) {
    if (pStroke->points.size() < 2) return;

    CPoint ptStart = pStroke->points.front();
    CPoint ptEnd = pStroke->points.back();

    // 직선 그리기
    DrawLine(pDC, pStroke);

    // 화살표 머리 그리기
    double angle = atan2(ptEnd.y - ptStart.y, ptEnd.x - ptStart.x);
    int arrowSize = pStroke->thickness * 3;

    CPoint pt1, pt2;
    pt1.x = ptEnd.x - (int)(arrowSize * cos(angle - M_PI / 6));
    pt1.y = ptEnd.y - (int)(arrowSize * sin(angle - M_PI / 6));
    pt2.x = ptEnd.x - (int)(arrowSize * cos(angle + M_PI / 6));
    pt2.y = ptEnd.y - (int)(arrowSize * sin(angle + M_PI / 6));

    CPen pen(PS_SOLID, pStroke->thickness, pStroke->color);
    CBrush brush(pStroke->color);
    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    CPoint pts[] = {ptEnd, pt1, pt2};
    pDC->Polygon(pts, 3);

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
}
```

#### Task 5.4: Undo/Redo
```cpp
void CDrawingOverlayWnd::Undo() {
    if (m_strokes.empty()) return;

    CStroke* pStroke = m_strokes.back();
    m_strokes.pop_back();
    m_redoStack.push_back(pStroke);

    Invalidate();
}

void CDrawingOverlayWnd::Redo() {
    if (m_redoStack.empty()) return;

    CStroke* pStroke = m_redoStack.back();
    m_redoStack.pop_back();
    m_strokes.push_back(pStroke);

    Invalidate();
}

void CDrawingOverlayWnd::Clear() {
    for (auto* p : m_strokes) delete p;
    m_strokes.clear();

    for (auto* p : m_redoStack) delete p;
    m_redoStack.clear();

    Invalidate();
}
```

**체크리스트**:
- [ ] 투명 오버레이 창 (WS_EX_LAYERED)
- [ ] 펜 도구 구현
- [ ] 직선, 화살표 도구
- [ ] 사각형, 타원 도구
- [ ] Undo/Redo 스택
- [ ] 색상 및 두께 변경

---

### Phase 6: 스크린샷 (Day 11)

#### Task 6.1: ScreenCapture 클래스
```cpp
// ScreenCapture.cpp
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class CScreenCapture {
public:
    static BOOL Initialize() {
        GdiplusStartupInput gdiplusStartupInput;
        return GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL) == Ok;
    }

    static void Shutdown() {
        GdiplusShutdown(m_gdiplusToken);
    }

    static HBITMAP CaptureFullScreen() {
        // ... (이전 구현과 동일)
    }

    static BOOL SaveToFile(HBITMAP hbm, LPCTSTR lpszPath, int nFormat) {
        Bitmap bitmap(hbm, NULL);

        CLSID clsid;
        if (nFormat == 0) { // PNG
            GetEncoderClsid(L"image/png", &clsid);
        } else { // JPG
            GetEncoderClsid(L"image/jpeg", &clsid);
        }

        return bitmap.Save(lpszPath, &clsid, NULL) == Ok;
    }

    static BOOL CopyToClipboard(HBITMAP hbm) {
        if (!OpenClipboard(NULL)) return FALSE;

        EmptyClipboard();
        SetClipboardData(CF_BITMAP, hbm);
        CloseClipboard();

        return TRUE;
    }

private:
    static ULONG_PTR m_gdiplusToken;

    static BOOL GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
        UINT num = 0, size = 0;
        GetImageEncodersSize(&num, &size);
        if (size == 0) return FALSE;

        ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)malloc(size);
        GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT i = 0; i < num; ++i) {
            if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
                *pClsid = pImageCodecInfo[i].Clsid;
                free(pImageCodecInfo);
                return TRUE;
            }
        }

        free(pImageCodecInfo);
        return FALSE;
    }
};

ULONG_PTR CScreenCapture::m_gdiplusToken = 0;
```

**체크리스트**:
- [ ] GDI+ 초기화
- [ ] PNG 저장
- [ ] JPG 저장
- [ ] 클립보드 복사

---

### Phase 7: 설정 UI (Day 12-13)

#### Task 7.1: 설정 다이얼로그
```cpp
// SettingsDlg.h
class CSettingsDlg : public CDialogEx {
public:
    CSettingsDlg(CWnd* pParent = nullptr);

    enum { IDD = IDD_SETTINGS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedBrowse();

    DECLARE_MESSAGE_MAP()

private:
    SETTINGS m_settings;

    // 일반 탭
    BOOL m_bLaunchOnStartup;
    BOOL m_bShowTrayIcon;

    // 확대 탭
    int m_nZoomLevel;
    int m_nAnimationDuration;

    // 단축키 탭
    CHotKeyCtrl m_hkZoom;
    CHotKeyCtrl m_hkDraw;

    void LoadSettings();
    void SaveSettings();
};
```

**체크리스트**:
- [ ] 탭 컨트롤 (일반, 확대, 그리기, 단축키)
- [ ] CHotKeyCtrl로 단축키 입력
- [ ] 색상 선택 다이얼로그
- [ ] 폴더 선택 다이얼로그

---

### Phase 8: 테스트 및 최적화 (Day 14)

#### Task 8.1: 메모리 누수 체크
```cpp
// Debug 빌드에서
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 애플리케이션 종료 시
_CrtDumpMemoryLeaks();
```

#### Task 8.2: 성능 측정
- Task Manager로 메모리/CPU 사용량 확인
- 다양한 해상도에서 테스트 (1920x1080, 2560x1440, 3840x2160)
- 확대/그리기 응답 시간 측정

#### Task 8.3: 빌드 크기 확인
```cmd
# Release 빌드 후
dir /s Release\LetsZoom.exe

# UPX 압축 (선택 사항)
upx --best --lzma Release\LetsZoom.exe
```

**체크리스트**:
- [ ] 메모리 누수 없음
- [ ] 실행 파일 < 5MB
- [ ] 메모리 사용량 < 50MB (활성 시)
- [ ] CPU < 10% (활성 시)

---

## 3. 빌드 및 배포

### 3.1 Release 빌드
```cmd
# Visual Studio Developer Command Prompt
msbuild LetsZoom.sln /p:Configuration=Release /p:Platform=x64 /t:Rebuild
```

### 3.2 설치 프로그램 (Inno Setup)
```pascal
[Setup]
AppName=LetsZoom
AppVersion=1.0.0
DefaultDirName={pf}\LetsZoom
DefaultGroupName=LetsZoom
OutputDir=Output
OutputBaseFilename=LetsZoom_Setup
Compression=lzma2
SolidCompression=yes

[Files]
Source: "Release\LetsZoom.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\LetsZoom"; Filename: "{app}\LetsZoom.exe"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\LetsZoom.exe"; Description: "Launch LetsZoom"; Flags: nowait postinstall skipifsilent
```

---

## 4. 예상 일정

| Phase | 작업 | 예상 시간 |
|-------|------|-----------|
| 1 | 프로젝트 셋업 + 디자인 시스템 | 2-3일 |
| 2 | 전역 단축키 | 1일 |
| 3 | 설정 관리 | 1-2일 |
| 4 | 화면 확대 + 애니메이션 | 3-4일 |
| 5 | 그리기 기능 + 플로팅 툴바 | 4-5일 |
| 6 | 스크린샷 | 1일 |
| 7 | 설정 UI (현대적 디자인) | 2-3일 |
| 8 | 테스트 및 최적화 | 1-2일 |
| **총계** | | **15-21일** |

**참고**: 현대적인 디자인 구현으로 3-5일 추가 소요 예상

---

## 5. 다음 단계

1. Visual Studio 2022 설치 및 환경 설정
2. 프로젝트 생성 및 기본 구조 작성
3. Phase 1부터 순차적으로 구현

준비가 되면 "Phase 1 시작"이라고 말씀해주시면 함께 구현을 시작하겠습니다!
