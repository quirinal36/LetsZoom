# LetsZoom - Modern Design Guide (MFC)

## 1. 디자인 철학

### 1.1 핵심 원칙
- **미니멀리즘**: 불필요한 요소 제거, 깔끔한 인터페이스
- **직관성**: 사용자가 학습 없이 즉시 사용 가능
- **현대성**: Windows 11 스타일의 플랫 디자인
- **세련됨**: 부드러운 애니메이션과 고급스러운 색상
- **비침투성**: 사용 중 방해하지 않는 최소한의 UI

### 1.2 참고 디자인
- **Windows 11 Design Language**: Fluent Design System
- **Microsoft PowerToys**: 현대적인 유틸리티 UI
- **ZoomIt**: 기능 참고 (디자인은 현대화)
- **Figma/Sketch**: 투명 오버레이 UI

## 2. 색상 시스템

### 2.1 Primary 색상 팔레트
```cpp
// Modern Color Scheme
namespace Colors {
    // Backgrounds
    const COLORREF BG_DARK       = RGB(32, 32, 32);      // #202020
    const COLORREF BG_PANEL      = RGB(45, 45, 45);      // #2D2D2D
    const COLORREF BG_HOVER      = RGB(60, 60, 60);      // #3C3C3C
    const COLORREF BG_SELECTED   = RGB(70, 70, 70);      // #464646

    // Accents (Windows 11 Blue)
    const COLORREF ACCENT_BLUE   = RGB(0, 120, 212);     // #0078D4
    const COLORREF ACCENT_HOVER  = RGB(0, 103, 192);     // #0067C0
    const COLORREF ACCENT_LIGHT  = RGB(76, 160, 224);    // #4CA0E0

    // Text
    const COLORREF TEXT_PRIMARY  = RGB(255, 255, 255);   // #FFFFFF
    const COLORREF TEXT_SECONDARY= RGB(200, 200, 200);   // #C8C8C8
    const COLORREF TEXT_DISABLED = RGB(130, 130, 130);   // #828282

    // Borders
    const COLORREF BORDER_NORMAL = RGB(80, 80, 80);      // #505050
    const COLORREF BORDER_FOCUS  = RGB(0, 120, 212);     // #0078D4

    // Drawing Colors (Vibrant)
    const COLORREF DRAW_RED      = RGB(255, 77, 77);     // #FF4D4D
    const COLORREF DRAW_ORANGE   = RGB(255, 153, 51);    // #FF9933
    const COLORREF DRAW_YELLOW   = RGB(255, 220, 51);    // #FFDC33
    const COLORREF DRAW_GREEN    = RGB(77, 255, 136);    // #4DFF88
    const COLORREF DRAW_CYAN     = RGB(77, 210, 255);    // #4DD2FF
    const COLORREF DRAW_BLUE     = RGB(77, 136, 255);    // #4D88FF
    const COLORREF DRAW_PURPLE   = RGB(187, 107, 217);   // #BB6BD9
    const COLORREF DRAW_PINK     = RGB(255, 107, 170);   // #FF6BAA
}
```

### 2.2 투명도 레벨
```cpp
namespace Alpha {
    const BYTE OPAQUE       = 255;  // 100%
    const BYTE HIGH         = 230;  // 90%
    const BYTE MEDIUM       = 180;  // 70%
    const BYTE LOW          = 128;  // 50%
    const BYTE VERY_LOW     = 64;   // 25%
    const BYTE TRANSPARENT  = 0;    // 0%
}
```

## 3. 타이포그래피

### 3.1 폰트 시스템
```cpp
class CFontManager {
public:
    static void Initialize() {
        // Primary Font: Segoe UI (Windows 11 기본)
        m_fontNormal.CreateFont(
            14, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            _T("Segoe UI")
        );

        // Headings
        m_fontHeading.CreateFont(
            18, 0, 0, 0, FW_SEMIBOLD,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            _T("Segoe UI")
        );

        // Small Text
        m_fontSmall.CreateFont(
            12, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            _T("Segoe UI")
        );
    }

    static CFont* GetNormalFont() { return &m_fontNormal; }
    static CFont* GetHeadingFont() { return &m_fontHeading; }
    static CFont* GetSmallFont() { return &m_fontSmall; }

private:
    static CFont m_fontNormal;
    static CFont m_fontHeading;
    static CFont m_fontSmall;
};
```

### 3.2 텍스트 스타일
- **제목**: Segoe UI Semibold 18pt, #FFFFFF
- **본문**: Segoe UI Regular 14pt, #FFFFFF
- **보조**: Segoe UI Regular 12pt, #C8C8C8

## 4. 컴포넌트 디자인

### 4.1 플로팅 툴바 (Zoom/Draw 모드)

```
┌─────────────────────────────────────────┐
│  ◉ ─ ↗ ▭ ○ T 🖍  │  🎨  2  4  8  │  ↶ ↷ ✕  │
└─────────────────────────────────────────┘
   도구           색상 두께       실행취소
```

**구현**:
```cpp
class CFloatingToolbar : public CWnd {
public:
    void Create(CWnd* pParent);

protected:
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
    struct ToolButton {
        CRect rect;
        int id;
        LPCTSTR icon;
        COLORREF color;
        BOOL bHovered;
        BOOL bSelected;
    };

    std::vector<ToolButton> m_buttons;
    int m_nHoveredBtn;
    int m_nSelectedBtn;

    void DrawButton(CDC* pDC, const ToolButton& btn);
    void DrawRoundRect(CDC* pDC, CRect rect, int radius, COLORREF color);
};

void CFloatingToolbar::OnPaint() {
    CPaintDC dc(this);

    CRect rcClient;
    GetClientRect(&rcClient);

    // Acrylic 배경 (반투명 블러)
    DrawAcrylicBackground(&dc, rcClient);

    // 버튼들
    for (const auto& btn : m_buttons) {
        DrawButton(&dc, btn);
    }
}

void CFloatingToolbar::DrawButton(CDC* pDC, const ToolButton& btn) {
    // 배경
    COLORREF bgColor = btn.bSelected ? Colors::ACCENT_BLUE :
                       btn.bHovered  ? Colors::BG_HOVER :
                                       Colors::BG_PANEL;

    DrawRoundRect(pDC, btn.rect, 8, bgColor);

    // 아이콘 (Segoe MDL2 Assets)
    CFont* pOldFont = pDC->SelectObject(m_pIconFont);
    pDC->SetTextColor(btn.bSelected ? Colors::TEXT_PRIMARY : Colors::TEXT_SECONDARY);
    pDC->SetBkMode(TRANSPARENT);
    pDC->DrawText(btn.icon, -1, const_cast<CRect*>(&btn.rect),
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    pDC->SelectObject(pOldFont);
}

void CFloatingToolbar::DrawRoundRect(CDC* pDC, CRect rect, int radius, COLORREF color) {
    // GDI+로 둥근 사각형
    Graphics graphics(pDC->m_hDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));

    GraphicsPath path;
    path.AddArc(rect.left, rect.top, radius*2, radius*2, 180, 90);
    path.AddArc(rect.right-radius*2, rect.top, radius*2, radius*2, 270, 90);
    path.AddArc(rect.right-radius*2, rect.bottom-radius*2, radius*2, radius*2, 0, 90);
    path.AddArc(rect.left, rect.bottom-radius*2, radius*2, radius*2, 90, 90);
    path.CloseFigure();

    graphics.FillPath(&brush, &path);
}
```

### 4.2 색상 피커

```
┌──────────────────────┐
│  ●  ●  ●  ●  ●  ●  ●  │  → 기본 색상 팔레트
│                      │
│  ┌────────────────┐  │
│  │   사용자 정의   │  │  → 클릭 시 색상 선택
│  └────────────────┘  │
└──────────────────────┘
```

**구현**:
```cpp
class CModernColorPicker : public CWnd {
public:
    void ShowAt(CPoint pt);
    COLORREF GetSelectedColor() const { return m_selectedColor; }

protected:
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
    struct ColorSwatch {
        CRect rect;
        COLORREF color;
        BOOL bHovered;
    };

    std::vector<ColorSwatch> m_swatches;
    COLORREF m_selectedColor;

    void DrawSwatch(CDC* pDC, const ColorSwatch& swatch);
};

void CModernColorPicker::OnPaint() {
    CPaintDC dc(this);

    // 반투명 어두운 배경
    Graphics graphics(dc.m_hDC);

    SolidBrush bgBrush(Color(230, 32, 32, 32));
    Pen borderPen(Color(255, 80, 80, 80), 1);

    RectF rcBg(0, 0, m_rcClient.Width(), m_rcClient.Height());
    graphics.FillRectangle(&bgBrush, rcBg);
    graphics.DrawRectangle(&borderPen, rcBg);

    // 색상 견본들
    for (const auto& swatch : m_swatches) {
        DrawSwatch(&dc, swatch);
    }
}

void CModernColorPicker::DrawSwatch(CDC* pDC, const ColorSwatch& swatch) {
    Graphics graphics(pDC->m_hDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    // 원형 색상 견본
    SolidBrush brush(Color(255,
        GetRValue(swatch.color),
        GetGValue(swatch.color),
        GetBValue(swatch.color)));

    int size = swatch.bHovered ? 32 : 28;
    int offset = (32 - size) / 2;

    graphics.FillEllipse(&brush,
        swatch.rect.left + offset,
        swatch.rect.top + offset,
        size, size);

    // 선택된 색상 표시 (흰색 링)
    if (swatch.color == m_selectedColor) {
        Pen whitePen(Color(255, 255, 255, 255), 2);
        graphics.DrawEllipse(&whitePen,
            swatch.rect.left + offset - 2,
            swatch.rect.top + offset - 2,
            size + 4, size + 4);
    }
}
```

### 4.3 설정 다이얼로그

**Windows 11 스타일 설정 UI**:

```
┌─────────────────────────────────────────────┐
│  ☰ LetsZoom 설정                     ─  ☐  ✕ │  ← 타이틀바
├─────────────────────────────────────────────┤
│                                             │
│  ┌───────────┐  ┌───────────────────────┐  │
│  │  일반     │  │                       │  │
│  │           │  │   시작 시 자동 실행    │  │
│  │  확대     │  │   [ON]                │  │
│  │           │  │                       │  │
│  │  그리기   │  │   트레이에 상주        │  │
│  │           │  │   [ON]                │  │
│  │  단축키   │  │                       │  │
│  │           │  │   알림 표시           │  │
│  │  스크린샷 │  │   [ON]                │  │
│  │           │  │                       │  │
│  └───────────┘  └───────────────────────┘  │
│                                             │
│                           ┌──────┐ ┌──────┐│
│                           │ 취소 │ │ 확인 ││
│                           └──────┘ └──────┘│
└─────────────────────────────────────────────┘
```

**구현**:
```cpp
class CModernSettingsDlg : public CDialogEx {
public:
    CModernSettingsDlg(CWnd* pParent = nullptr);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

private:
    // 커스텀 컨트롤들
    CModernTabCtrl m_tabCtrl;
    CModernToggleButton m_btnAutoStart;
    CModernToggleButton m_btnTrayIcon;
    CModernButton m_btnOK;
    CModernButton m_btnCancel;

    void InitializeControls();
    void ApplyModernStyle();
};

// 현대적인 토글 버튼
class CModernToggleButton : public CButton {
public:
    void SetChecked(BOOL bChecked);
    BOOL IsChecked() const { return m_bChecked; }

protected:
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
    BOOL m_bChecked;
    BOOL m_bHovered;
    int m_nAnimProgress; // 0-100

    void DrawToggle(CDC* pDC);
    void AnimateToggle();
};

void CModernToggleButton::OnPaint() {
    CPaintDC dc(this);

    CRect rcClient;
    GetClientRect(&rcClient);

    Graphics graphics(dc.m_hDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    // 배경 트랙
    int trackWidth = 44;
    int trackHeight = 24;
    int thumbSize = 18;

    CRect rcTrack(
        rcClient.left,
        rcClient.top + (rcClient.Height() - trackHeight) / 2,
        rcClient.left + trackWidth,
        rcClient.top + (rcClient.Height() - trackHeight) / 2 + trackHeight
    );

    COLORREF trackColor = m_bChecked ? Colors::ACCENT_BLUE : Colors::BG_HOVER;
    SolidBrush trackBrush(Color(255,
        GetRValue(trackColor),
        GetGValue(trackColor),
        GetBValue(trackColor)));

    graphics.FillRoundedRectangle(&trackBrush,
        RectF(rcTrack.left, rcTrack.top, rcTrack.Width(), rcTrack.Height()),
        trackHeight / 2, trackHeight / 2);

    // Thumb (슬라이더)
    int thumbX = m_bChecked ?
        rcTrack.right - thumbSize - 3 :
        rcTrack.left + 3;

    SolidBrush thumbBrush(Color(255, 255, 255, 255));
    graphics.FillEllipse(&thumbBrush,
        thumbX,
        rcTrack.top + 3,
        thumbSize,
        thumbSize);
}

// 현대적인 버튼
class CModernButton : public CButton {
public:
    enum class Style {
        Primary,   // 파란색 배경
        Secondary, // 회색 배경
        Outline    // 테두리만
    };

    void SetStyle(Style style);

protected:
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();

private:
    Style m_style;
    BOOL m_bHovered;
    BOOL m_bPressed;

    void DrawButton(CDC* pDC);
};

void CModernButton::OnPaint() {
    CPaintDC dc(this);

    CRect rcClient;
    GetClientRect(&rcClient);

    Graphics graphics(dc.m_hDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

    // 배경색 결정
    COLORREF bgColor;
    COLORREF textColor = Colors::TEXT_PRIMARY;

    switch (m_style) {
        case Style::Primary:
            bgColor = m_bPressed ? Colors::ACCENT_HOVER :
                     m_bHovered ? Colors::ACCENT_LIGHT :
                                  Colors::ACCENT_BLUE;
            break;
        case Style::Secondary:
            bgColor = m_bPressed ? Colors::BG_SELECTED :
                     m_bHovered ? Colors::BG_HOVER :
                                  Colors::BG_PANEL;
            break;
        case Style::Outline:
            bgColor = m_bHovered ? Colors::BG_HOVER :
                                   RGB(0, 0, 0); // 투명
            textColor = Colors::ACCENT_BLUE;
            break;
    }

    // 둥근 사각형 배경
    SolidBrush bgBrush(Color(255,
        GetRValue(bgColor),
        GetGValue(bgColor),
        GetBValue(bgColor)));

    GraphicsPath path;
    int radius = 4;
    AddRoundedRectPath(path, rcClient, radius);
    graphics.FillPath(&bgBrush, &path);

    // 외곽선 (Outline 스타일)
    if (m_style == Style::Outline) {
        Pen borderPen(Color(255,
            GetRValue(Colors::ACCENT_BLUE),
            GetGValue(Colors::ACCENT_BLUE),
            GetBValue(Colors::ACCENT_BLUE)), 1);
        graphics.DrawPath(&borderPen, &path);
    }

    // 텍스트
    CString strText;
    GetWindowText(strText);

    Font font(L"Segoe UI", 10, FontStyleRegular);
    SolidBrush textBrush(Color(255,
        GetRValue(textColor),
        GetGValue(textColor),
        GetBValue(textColor)));

    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    RectF rcText(rcClient.left, rcClient.top,
                 rcClient.Width(), rcClient.Height());
    graphics.DrawString(strText, -1, &font, rcText, &format, &textBrush);
}
```

## 5. 애니메이션

### 5.1 페이드 인/아웃
```cpp
class CAnimationHelper {
public:
    static void FadeIn(CWnd* pWnd, int duration = 200) {
        BYTE alpha = 0;
        int steps = duration / 16; // 60fps
        int delta = 255 / steps;

        for (int i = 0; i < steps; ++i) {
            alpha = min(255, alpha + delta);
            ::SetLayeredWindowAttributes(pWnd->m_hWnd, 0, alpha, LWA_ALPHA);
            Sleep(16);
        }

        ::SetLayeredWindowAttributes(pWnd->m_hWnd, 0, 255, LWA_ALPHA);
    }

    static void FadeOut(CWnd* pWnd, int duration = 200) {
        BYTE alpha = 255;
        int steps = duration / 16;
        int delta = 255 / steps;

        for (int i = 0; i < steps; ++i) {
            alpha = max(0, alpha - delta);
            ::SetLayeredWindowAttributes(pWnd->m_hWnd, 0, alpha, LWA_ALPHA);
            Sleep(16);
        }
    }

    static void SlideIn(CWnd* pWnd, SlideDirection dir, int duration = 300) {
        // 슬라이드 애니메이션
        CRect rcFinal;
        pWnd->GetWindowRect(&rcFinal);

        CRect rcStart = rcFinal;
        switch (dir) {
            case SlideDirection::FromTop:
                rcStart.OffsetRect(0, -rcFinal.Height());
                break;
            case SlideDirection::FromBottom:
                rcStart.OffsetRect(0, rcFinal.Height());
                break;
            case SlideDirection::FromLeft:
                rcStart.OffsetRect(-rcFinal.Width(), 0);
                break;
            case SlideDirection::FromRight:
                rcStart.OffsetRect(rcFinal.Width(), 0);
                break;
        }

        int steps = duration / 16;
        for (int i = 0; i <= steps; ++i) {
            float t = (float)i / steps;
            t = EaseOutCubic(t); // Easing function

            CRect rcCurrent;
            rcCurrent.left = Lerp(rcStart.left, rcFinal.left, t);
            rcCurrent.top = Lerp(rcStart.top, rcFinal.top, t);
            rcCurrent.right = Lerp(rcStart.right, rcFinal.right, t);
            rcCurrent.bottom = Lerp(rcStart.bottom, rcFinal.bottom, t);

            pWnd->MoveWindow(&rcCurrent);
            Sleep(16);
        }
    }

private:
    static float EaseOutCubic(float t) {
        return 1 - pow(1 - t, 3);
    }

    static int Lerp(int a, int b, float t) {
        return a + (int)((b - a) * t);
    }
};
```

### 5.2 확대 모드 전환 애니메이션
```cpp
void CZoomOverlayWnd::Show(int nZoomLevel) {
    // 1. 화면 캡처
    CaptureScreen();

    // 2. 창 생성 (완전 투명)
    ShowWindow(SW_SHOW);
    ::SetLayeredWindowAttributes(m_hWnd, 0, 0, LWA_ALPHA);

    m_nZoomLevel = 100; // 시작은 확대 없음
    m_ptCenter = GetCursorPos();

    // 3. 페이드 인 + 줌 애니메이션
    const int DURATION = 300; // ms
    const int STEPS = DURATION / 16;

    for (int i = 0; i <= STEPS; ++i) {
        float t = (float)i / STEPS;
        t = EaseOutCubic(t);

        // 투명도 증가
        BYTE alpha = (BYTE)(255 * t);
        ::SetLayeredWindowAttributes(m_hWnd, 0, alpha, LWA_ALPHA);

        // 줌 레벨 증가
        m_nZoomLevel = 100 + (int)((nZoomLevel - 100) * t);

        Invalidate(FALSE);
        UpdateWindow();
        Sleep(16);
    }

    m_nZoomLevel = nZoomLevel;
}
```

## 6. 시각적 피드백

### 6.1 Zoom 모드 인디케이터
```cpp
void CZoomOverlayWnd::DrawIndicator(CDC* pDC) {
    // 화면 좌측 상단에 작은 인디케이터
    CRect rcIndicator(20, 20, 150, 60);

    Graphics graphics(pDC->m_hDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    // 반투명 어두운 배경
    SolidBrush bgBrush(Color(200, 32, 32, 32));
    graphics.FillRoundedRectangle(&bgBrush,
        RectF(rcIndicator.left, rcIndicator.top,
              rcIndicator.Width(), rcIndicator.Height()),
        8, 8);

    // 텍스트: "확대 200%"
    Font font(L"Segoe UI", 12, FontStyleBold);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    CString strText;
    strText.Format(_T("🔍 확대 %d%%"), m_nZoomLevel);

    RectF rcText(rcIndicator.left, rcIndicator.top,
                 rcIndicator.Width(), rcIndicator.Height());
    graphics.DrawString(strText, -1, &font, rcText, &format, &textBrush);

    // ESC 힌트
    Font hintFont(L"Segoe UI", 9, FontStyleRegular);
    SolidBrush hintBrush(Color(200, 200, 200, 200));

    CRect rcHint = rcIndicator;
    rcHint.top = rcIndicator.bottom + 5;
    rcHint.bottom = rcHint.top + 20;

    RectF rcHintText(rcHint.left, rcHint.top,
                     rcHint.Width(), rcHint.Height());
    graphics.DrawString(L"ESC로 종료", -1, &hintFont,
                       rcHintText, &format, &hintBrush);
}
```

### 6.2 십자선 (Crosshair)
```cpp
void CZoomOverlayWnd::DrawCrosshair(CDC* pDC) {
    CRect rcClient;
    GetClientRect(&rcClient);

    Graphics graphics(pDC->m_hDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    int cx = rcClient.Width() / 2;
    int cy = rcClient.Height() / 2;
    int size = 20;
    int gap = 5;

    // 흰색 십자선 (그림자 효과)
    Pen shadowPen(Color(128, 0, 0, 0), 3);
    graphics.DrawLine(&shadowPen, cx - size, cy, cx - gap, cy);
    graphics.DrawLine(&shadowPen, cx + gap, cy, cx + size, cy);
    graphics.DrawLine(&shadowPen, cx, cy - size, cx, cy - gap);
    graphics.DrawLine(&shadowPen, cx, cy + gap, cx, cy + size);

    Pen crosshairPen(Color(255, 255, 255, 255), 2);
    graphics.DrawLine(&crosshairPen, cx - size, cy, cx - gap, cy);
    graphics.DrawLine(&crosshairPen, cx + gap, cy, cx + size, cy);
    graphics.DrawLine(&crosshairPen, cx, cy - size, cx, cy - gap);
    graphics.DrawLine(&crosshairPen, cx, cy + gap, cx, cy + size);

    // 중심점 원
    SolidBrush centerBrush(Color(180, 0, 120, 212));
    graphics.FillEllipse(&centerBrush, cx - 3, cy - 3, 6, 6);
}
```

### 6.3 그리기 커서
```cpp
void CDrawingOverlayWnd::UpdateCursor() {
    // 도구에 따라 커스텀 커서 생성
    switch (m_currentTool) {
        case DrawTool::Pen:
        case DrawTool::Highlighter:
            SetCrosshairCursor();
            break;
        case DrawTool::Line:
        case DrawTool::Arrow:
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            break;
        case DrawTool::Eraser:
            SetEraserCursor();
            break;
        case DrawTool::Text:
            SetCursor(LoadCursor(NULL, IDC_IBEAM));
            break;
    }
}

void CDrawingOverlayWnd::SetCrosshairCursor() {
    // 펜 두께를 나타내는 원형 커서 생성
    int size = m_thickness * 2 + 4;
    HBITMAP hBitmap = CreateTransparentBitmap(size, size);

    CDC dc;
    dc.CreateCompatibleDC(NULL);
    HBITMAP hOldBmp = (HBITMAP)dc.SelectObject(hBitmap);

    Graphics graphics(dc.m_hDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    // 원 그리기
    Pen pen(Color(255,
        GetRValue(m_color),
        GetGValue(m_color),
        GetBValue(m_color)), 2);
    graphics.DrawEllipse(&pen, 2, 2, size - 4, size - 4);

    dc.SelectObject(hOldBmp);

    ICONINFO iconInfo = {0};
    iconInfo.fIcon = FALSE;
    iconInfo.xHotspot = size / 2;
    iconInfo.yHotspot = size / 2;
    iconInfo.hbmMask = hBitmap;
    iconInfo.hbmColor = hBitmap;

    HCURSOR hCursor = CreateIconIndirect(&iconInfo);
    SetCursor(hCursor);

    DeleteObject(hBitmap);
}
```

## 7. Acrylic/Blur 효과 (Windows 10+)

### 7.1 DWM Blur
```cpp
void CFloatingToolbar::EnableAcrylicBlur() {
    if (!IsWindows10OrGreater()) return;

    // DWM Blur Behind
    DWM_BLURBEHIND bb = {0};
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.fEnable = TRUE;
    bb.hRgnBlur = CreateRectRgn(0, 0, -1, -1);

    DwmEnableBlurBehindWindow(m_hWnd, &bb);
    DeleteObject(bb.hRgnBlur);

    // Accent Policy (Windows 10 1809+)
    struct ACCENTPOLICY {
        int nAccentState;
        int nFlags;
        int nColor;
        int nAnimationId;
    };

    struct WINCOMPATTRDATA {
        int nAttribute;
        PVOID pData;
        ULONG ulDataSize;
    };

    typedef BOOL (WINAPI *pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);

    HMODULE hUser32 = GetModuleHandle(_T("user32.dll"));
    pSetWindowCompositionAttribute SetWindowCompositionAttribute =
        (pSetWindowCompositionAttribute)GetProcAddress(hUser32,
        "SetWindowCompositionAttribute");

    if (SetWindowCompositionAttribute) {
        ACCENTPOLICY policy = {0};
        policy.nAccentState = 3; // ACCENT_ENABLE_BLURBEHIND
        policy.nFlags = 2;
        policy.nColor = 0x88000000; // ABGR

        WINCOMPATTRDATA data = {0};
        data.nAttribute = 19; // WCA_ACCENT_POLICY
        data.pData = &policy;
        data.ulDataSize = sizeof(policy);

        SetWindowCompositionAttribute(m_hWnd, &data);
    }
}
```

## 8. 아이콘 시스템

### 8.1 Segoe MDL2 Assets
```cpp
namespace Icons {
    // Segoe MDL2 Assets 폰트 사용
    const wchar_t* PEN          = L"\uE76D";  // ✎
    const wchar_t* LINE         = L"\uE8BC";  // ─
    const wchar_t* ARROW        = L"\uE72A";  // ↗
    const wchar_t* RECTANGLE    = L"\uE91F";  // ▭
    const wchar_t* CIRCLE       = L"\uF785";  // ○
    const wchar_t* TEXT         = L"\uE8D2";  // T
    const wchar_t* HIGHLIGHTER  = L"\uE76E";  // 🖍
    const wchar_t* ERASER       = L"\uE75C";  // ⌫
    const wchar_t* COLOR        = L"\uE790";  // 🎨
    const wchar_t* THICKNESS    = L"\uE7FB";  // ─
    const wchar_t* UNDO         = L"\uE7A7";  // ↶
    const wchar_t* REDO         = L"\uE7A6";  // ↷
    const wchar_t* CLEAR        = L"\uE894";  // ✕
    const wchar_t* SETTINGS     = L"\uE713";  // ⚙
    const wchar_t* ZOOM_IN      = L"\uE8A3";  // 🔍+
    const wchar_t* ZOOM_OUT     = L"\uE71F";  // 🔍-
    const wchar_t* SCREENSHOT   = L"\uE7B2";  // 📷
}

// 아이콘 폰트 생성
CFont* CreateIconFont(int size = 16) {
    CFont* pFont = new CFont();
    pFont->CreateFont(
        size, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        _T("Segoe MDL2 Assets")
    );
    return pFont;
}
```

## 9. 반응형 레이아웃

### 9.1 DPI 인식
```cpp
class CDPIHelper {
public:
    static void Initialize(HWND hWnd) {
        m_hWnd = hWnd;
        m_nDPI = GetDpiForWindow(hWnd);
        m_fScale = m_nDPI / 96.0f;
    }

    static int Scale(int value) {
        return (int)(value * m_fScale);
    }

    static CRect Scale(const CRect& rc) {
        return CRect(
            Scale(rc.left),
            Scale(rc.top),
            Scale(rc.right),
            Scale(rc.bottom)
        );
    }

    static float GetScale() { return m_fScale; }
    static int GetDPI() { return m_nDPI; }

private:
    static HWND m_hWnd;
    static int m_nDPI;
    static float m_fScale;
};
```

## 10. 완성된 UI 예시

### 10.1 전체 화면 확대 모드
```
┌─────────────────────────────────────────────────────────┐
│                                                         │
│   ┌──────────────┐                                     │
│   │ 🔍 확대 200%  │  ← 상태 인디케이터 (반투명)         │
│   │ ESC로 종료    │                                     │
│   └──────────────┘                                     │
│                                                         │
│                                                         │
│                         +                               │  ← 십자선
│                                                         │
│                                                         │
│   확대된 화면 내용...                                    │
│                                                         │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### 10.2 그리기 모드
```
┌─────────────────────────────────────────────────────────┐
│  투명 배경에 그리기...                                   │
│                                                         │
│  ┌───────────────────────────────────────────────┐     │
│  │  ◉ ─ ↗ ▭ ○ T 🖍  │  ●●●●  2 4 8  │  ↶ ↷ ✕  │     │  ← 플로팅 툴바
│  └───────────────────────────────────────────────┘     │
│     도구            색상  두께      실행취소            │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

## 11. 구현 체크리스트

### Phase 1: 기본 디자인 시스템
- [ ] 색상 팔레트 정의
- [ ] 폰트 시스템 구축
- [ ] GDI+ 초기화
- [ ] 둥근 모서리 헬퍼 함수

### Phase 2: 커스텀 컨트롤
- [ ] CModernButton
- [ ] CModernToggleButton
- [ ] CModernColorPicker
- [ ] CFloatingToolbar

### Phase 3: 애니메이션
- [ ] FadeIn/FadeOut
- [ ] SlideIn/Out
- [ ] Zoom 전환 애니메이션
- [ ] Easing 함수

### Phase 4: 시각 효과
- [ ] Acrylic/Blur 배경
- [ ] 드롭 섀도우
- [ ] 호버 효과
- [ ] 포커스 표시

### Phase 5: 아이콘
- [ ] Segoe MDL2 Assets 폰트
- [ ] 아이콘 헬퍼 함수
- [ ] 커스텀 커서

### Phase 6: 통합
- [ ] 모든 다이얼로그에 적용
- [ ] 오버레이 창 스타일링
- [ ] 트레이 아이콘 메뉴

이 디자인 가이드를 따라 구현하면 ZoomIt보다 훨씬 현대적이고 세련된 UI를 가진 LetsZoom을 만들 수 있습니다!
