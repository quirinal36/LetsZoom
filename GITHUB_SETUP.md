# GitHub 저장소 설정 가이드

## 1. GitHub 저장소 생성

### 방법 1: GitHub 웹사이트
1. https://github.com/new 접속
2. Repository name: `LetsZoom`
3. Description: `경량 화면 확대 및 주석 도구 (Win32 API)`
4. Public 또는 Private 선택
5. **README, .gitignore, License 추가 안 함** (이미 로컬에 있음)
6. Create repository 클릭

### 방법 2: GitHub CLI (설치 필요)
```bash
# GitHub CLI 설치 (Windows)
winget install --id GitHub.cli

# 로그인
gh auth login

# 저장소 생성
gh repo create LetsZoom --public --source=. --remote=origin --push
```

## 2. 원격 저장소 연결 (웹사이트로 생성한 경우)

```bash
cd "c:\Users\Albert\Documents\GitHub\LetsZoom"

# 원격 저장소 추가 (YOUR_USERNAME을 본인 GitHub 사용자명으로 변경)
git remote add origin https://github.com/YOUR_USERNAME/LetsZoom.git

# 메인 브랜치 이름 설정
git branch -M main

# 푸시
git push -u origin main
```

## 3. Milestones 생성

### GitHub 웹에서 생성
1. 저장소 페이지 → Issues 탭
2. Milestones 클릭
3. New milestone 클릭
4. 아래 정보 입력

### Milestone 1: Phase 1 - 프로젝트 셋업 및 개발 환경
- **Title**: `Phase 1: 프로젝트 셋업`
- **Due date**: (시작일로부터 2-3일)
- **Description**:
  ```
  개발 환경 설정 및 기본 프로젝트 구조 생성

  - VS Code 개발 환경 설정
  - MinGW-w64 또는 MSVC 설치
  - CMake 빌드 시스템 구성
  - 기본 윈도우 생성 및 메시지 루프
  - 트레이 아이콘 구현
  ```

### Milestone 2: Phase 2 - 전역 단축키 및 설정
- **Title**: `Phase 2: 전역 단축키 및 설정`
- **Due date**: (Phase 1 완료 후 1-2일)
- **Description**:
  ```
  전역 단축키 시스템 및 설정 관리 구현

  - RegisterHotKey API로 전역 단축키 등록
  - 설정 저장/불러오기 (INI 파일)
  - 단축키 충돌 처리
  ```

### Milestone 3: Phase 3 - 화면 확대 기능
- **Title**: `Phase 3: 화면 확대 기능`
- **Due date**: (Phase 2 완료 후 3-4일)
- **Description**:
  ```
  화면 확대 오버레이 구현

  - 전체화면 레이어드 윈도우
  - 화면 캡처 (BitBlt)
  - 확대 렌더링 (StretchBlt)
  - 마우스 휠로 줌 조절
  - 마우스 드래그로 패닝
  - 십자선 표시
  - 페이드 인/아웃 애니메이션
  ```

### Milestone 4: Phase 4 - 그리기 기능
- **Title**: `Phase 4: 그리기 기능`
- **Due date**: (Phase 3 완료 후 4-5일)
- **Description**:
  ```
  그리기 오버레이 및 도구 구현

  - 투명 오버레이 윈도우
  - GDI+ 그리기 엔진
  - 펜, 직선, 화살표, 도형 도구
  - 색상 및 두께 설정
  - Undo/Redo 스택
  - 플로팅 툴바 UI
  ```

### Milestone 5: Phase 5 - 스크린샷 및 고급 기능
- **Title**: `Phase 5: 스크린샷 및 고급 기능`
- **Due date**: (Phase 4 완료 후 2-3일)
- **Description**:
  ```
  스크린샷 캡처 및 설정 UI

  - 스크린샷 캡처 (전체/영역)
  - PNG/JPG 저장 (GDI+)
  - 클립보드 복사
  - 설정 다이얼로그 (현대적 디자인)
  ```

### Milestone 6: Phase 6 - 테스트 및 배포
- **Title**: `Phase 6: 테스트 및 배포`
- **Due date**: (Phase 5 완료 후 1-2일)
- **Description**:
  ```
  테스트, 최적화 및 배포 준비

  - 메모리 누수 체크
  - 성능 프로파일링
  - 용량 최적화 (< 3MB)
  - 다양한 해상도/DPI 테스트
  - 설치 프로그램 제작
  - 문서화
  ```

## 4. Issues 생성

각 Milestone에 대한 Issue를 생성합니다. 아래 템플릿을 사용하세요.

### Phase 1 Issues

#### Issue #1: VS Code 개발 환경 설정
- **Title**: `[Phase 1] VS Code 개발 환경 설정`
- **Labels**: `setup`, `documentation`
- **Milestone**: `Phase 1: 프로젝트 셋업`
- **Description**:
  ```markdown
  ## 목표
  VS Code에서 C/C++ 개발 환경 설정

  ## 작업 내용
  - [ ] VS Code C/C++ 확장 설치
  - [ ] MinGW-w64 또는 MSVC 설치
  - [ ] c_cpp_properties.json 설정
  - [ ] tasks.json (빌드 태스크) 설정
  - [ ] launch.json (디버그 설정) 설정

  ## 참고 문서
  - VS_CODE_SETUP.md (생성 예정)
  ```

#### Issue #2: CMake 빌드 시스템 구성
- **Title**: `[Phase 1] CMake 빌드 시스템 구성`
- **Labels**: `setup`, `build`
- **Milestone**: `Phase 1: 프로젝트 셋업`
- **Description**:
  ```markdown
  ## 목표
  CMake를 사용한 빌드 시스템 구성

  ## 작업 내용
  - [ ] CMakeLists.txt 생성
  - [ ] 소스 파일 구조 정의
  - [ ] Windows 라이브러리 링크 설정
  - [ ] Release 빌드 최적화 옵션
  - [ ] 빌드 테스트

  ## 예상 크기
  - Debug: ~5MB
  - Release: ~2-3MB
  ```

#### Issue #3: 기본 윈도우 및 메시지 루프
- **Title**: `[Phase 1] 기본 윈도우 및 메시지 루프 구현`
- **Labels**: `core`, `implementation`
- **Milestone**: `Phase 1: 프로젝트 셋업`
- **Description**:
  ```markdown
  ## 목표
  WinMain 진입점과 숨겨진 메인 윈도우 생성

  ## 작업 내용
  - [ ] src/main.c 생성
  - [ ] WinMain 함수 구현
  - [ ] 숨겨진 윈도우 생성 (메시지 수신용)
  - [ ] 메시지 루프 구현
  - [ ] WM_DESTROY 핸들러

  ## 참고
  - ARCHITECTURE_WIN32.md 섹션 3.1, 3.2
  ```

#### Issue #4: 트레이 아이콘 구현
- **Title**: `[Phase 1] 트레이 아이콘 및 메뉴 구현`
- **Labels**: `ui`, `implementation`
- **Milestone**: `Phase 1: 프로젝트 셋업`
- **Description**:
  ```markdown
  ## 목표
  시스템 트레이 아이콘 및 우클릭 메뉴

  ## 작업 내용
  - [ ] src/tray.c/h 생성
  - [ ] Shell_NotifyIcon으로 트레이 아이콘 추가
  - [ ] 우클릭 메뉴 구현 (설정, 종료)
  - [ ] 트레이 아이콘 클릭 이벤트
  - [ ] 알림 기능 (Balloon tip)

  ## 참고
  - ARCHITECTURE_WIN32.md 섹션 3.4
  ```

### Phase 2 Issues

#### Issue #5: 전역 단축키 시스템
- **Title**: `[Phase 2] 전역 단축키 시스템 구현`
- **Labels**: `core`, `implementation`
- **Milestone**: `Phase 2: 전역 단축키 및 설정`
- **Description**:
  ```markdown
  ## 목표
  RegisterHotKey API를 사용한 전역 단축키

  ## 작업 내용
  - [ ] src/hotkey.c/h 생성
  - [ ] RegisterHotKey로 단축키 등록
  - [ ] WM_HOTKEY 메시지 핸들러
  - [ ] 단축키 충돌 처리
  - [ ] 기본 단축키 설정 (Ctrl+1~4)

  ## 기본 단축키
  - Ctrl+1: 확대 모드
  - Ctrl+2: 그리기 모드
  - Ctrl+3: 확대+그리기
  - Ctrl+4: 스크린샷

  ## 참고
  - ARCHITECTURE_WIN32.md 섹션 3.3
  ```

#### Issue #6: 설정 저장 및 불러오기
- **Title**: `[Phase 2] 설정 저장 및 불러오기 구현`
- **Labels**: `core`, `implementation`
- **Milestone**: `Phase 2: 전역 단축키 및 설정`
- **Description**:
  ```markdown
  ## 목표
  INI 파일을 사용한 설정 관리

  ## 작업 내용
  - [ ] src/settings.c/h 생성
  - [ ] Settings 구조체 정의
  - [ ] GetPrivateProfileInt/String으로 읽기
  - [ ] WritePrivateProfileString으로 쓰기
  - [ ] %APPDATA%\LetsZoom\config.ini
  - [ ] 기본값 설정

  ## 설정 항목
  - 일반: 자동 시작, 트레이 아이콘
  - 확대: 기본 배율, 애니메이션 속도
  - 그리기: 색상, 두께, 도구
  - 단축키: 모든 단축키 커스터마이징

  ## 참고
  - ARCHITECTURE_WIN32.md 섹션 3.8
  ```

### Phase 3 Issues

#### Issue #7: 확대 오버레이 윈도우
- **Title**: `[Phase 3] 확대 오버레이 윈도우 구현`
- **Labels**: `feature`, `implementation`
- **Milestone**: `Phase 3: 화면 확대 기능`
- **Description**:
  ```markdown
  ## 목표
  전체화면 확대 오버레이 윈도우

  ## 작업 내용
  - [ ] src/zoom_overlay.c/h 생성
  - [ ] 레이어드 윈도우 생성 (WS_EX_LAYERED)
  - [ ] 화면 캡처 (BitBlt)
  - [ ] 확대 렌더링 (StretchBlt)
  - [ ] 더블 버퍼링
  - [ ] 페이드 인/아웃 애니메이션

  ## 참고
  - ARCHITECTURE_WIN32.md 섹션 3.5
  ```

#### Issue #8: 줌 인터랙션 (휠, 패닝)
- **Title**: `[Phase 3] 줌 인터랙션 구현 (휠, 패닝, 키보드)`
- **Labels**: `feature`, `implementation`
- **Milestone**: `Phase 3: 화면 확대 기능`
- **Description**:
  ```markdown
  ## 목표
  확대 모드 상호작용

  ## 작업 내용
  - [ ] 마우스 휠로 줌 레벨 조절 (100~2000%)
  - [ ] 마우스 드래그로 패닝
  - [ ] 키보드 +/- 키로 줌
  - [ ] 화살표 키로 이동
  - [ ] ESC 키로 종료
  - [ ] 십자선 표시

  ## UI 요소
  - 좌측 상단 인디케이터 (확대 200%)
  - 중심 십자선
  - ESC 힌트
  ```

### Phase 4 Issues

#### Issue #9: 그리기 오버레이 윈도우
- **Title**: `[Phase 4] 그리기 오버레이 윈도우 구현`
- **Labels**: `feature`, `implementation`
- **Milestone**: `Phase 4: 그리기 기능`
- **Description**:
  ```markdown
  ## 목표
  투명 그리기 오버레이 윈도우

  ## 작업 내용
  - [ ] src/drawing_overlay.c/h 생성
  - [ ] 투명 레이어드 윈도우
  - [ ] GDI+ 초기화
  - [ ] 마우스 입력 처리
  - [ ] 스트로크 데이터 구조
  - [ ] 안티앨리어싱 렌더링

  ## 참고
  - ARCHITECTURE_WIN32.md 섹션 3.6
  ```

#### Issue #10: 그리기 도구 구현
- **Title**: `[Phase 4] 그리기 도구 구현 (펜, 선, 화살표, 도형)`
- **Labels**: `feature`, `implementation`
- **Milestone**: `Phase 4: 그리기 기능`
- **Description**:
  ```markdown
  ## 목표
  다양한 그리기 도구

  ## 작업 내용
  - [ ] 펜 도구 (자유 곡선)
  - [ ] 직선 도구
  - [ ] 화살표 도구
  - [ ] 사각형 도구
  - [ ] 원/타원 도구
  - [ ] 텍스트 도구 (선택 사항)
  - [ ] 하이라이터 (반투명)
  - [ ] 지우개

  ## 속성
  - 색상: 8가지 기본 + 커스텀
  - 두께: 2, 4, 8, 16px
  - 투명도: 0-255
  ```

#### Issue #11: Undo/Redo 스택
- **Title**: `[Phase 4] Undo/Redo 스택 구현`
- **Labels**: `feature`, `implementation`
- **Milestone**: `Phase 4: 그리기 기능`
- **Description**:
  ```markdown
  ## 목표
  그리기 취소/재실행 기능

  ## 작업 내용
  - [ ] Undo 스택 구현
  - [ ] Redo 스택 구현
  - [ ] Ctrl+Z로 Undo
  - [ ] Ctrl+Y로 Redo
  - [ ] E 키로 전체 지우기
  - [ ] 스택 크기 제한 (메모리 관리)
  ```

#### Issue #12: 플로팅 툴바 UI
- **Title**: `[Phase 4] 플로팅 툴바 UI 구현`
- **Labels**: `ui`, `implementation`
- **Milestone**: `Phase 4: 그리기 기능`
- **Description**:
  ```markdown
  ## 목표
  현대적인 플로팅 툴바

  ## 작업 내용
  - [ ] 독립 윈도우로 툴바 생성
  - [ ] 둥근 모서리 배경 (GDI+)
  - [ ] Acrylic/Blur 효과
  - [ ] 도구 버튼 (Segoe MDL2 아이콘)
  - [ ] 색상 피커
  - [ ] 두께 선택
  - [ ] 호버 효과

  ## 디자인
  - DESIGN_GUIDE_MFC.md 참고 (GDI+로 구현)
  ```

### Phase 5 Issues

#### Issue #13: 스크린샷 캡처
- **Title**: `[Phase 5] 스크린샷 캡처 구현`
- **Labels**: `feature`, `implementation`
- **Milestone**: `Phase 5: 스크린샷 및 고급 기능`
- **Description**:
  ```markdown
  ## 목표
  화면 캡처 및 저장

  ## 작업 내용
  - [ ] src/screenshot.c/h 생성
  - [ ] 전체 화면 캡처 (BitBlt)
  - [ ] 영역 선택 캡처
  - [ ] GDI+로 PNG 저장
  - [ ] GDI+로 JPG 저장
  - [ ] 클립보드 복사
  - [ ] 자동 파일명 (날짜/시간)

  ## 참고
  - ARCHITECTURE_WIN32.md 섹션 3.7
  ```

#### Issue #14: 설정 다이얼로그
- **Title**: `[Phase 5] 설정 다이얼로그 UI 구현`
- **Labels**: `ui`, `implementation`
- **Milestone**: `Phase 5: 스크린샷 및 고급 기능`
- **Description**:
  ```markdown
  ## 목표
  현대적인 설정 다이얼로그

  ## 작업 내용
  - [ ] 다이얼로그 윈도우 생성
  - [ ] 탭 컨트롤 (일반, 확대, 그리기, 단축키)
  - [ ] 커스텀 컨트롤 (토글 버튼, 색상 피커)
  - [ ] 설정 저장/불러오기 연동
  - [ ] 단축키 입력 (CHotKeyCtrl 대체)

  ## 디자인
  - Windows 11 스타일
  - DESIGN_GUIDE_MFC.md 참고
  ```

### Phase 6 Issues

#### Issue #15: 메모리 누수 체크 및 최적화
- **Title**: `[Phase 6] 메모리 누수 체크 및 최적화`
- **Labels**: `testing`, `optimization`
- **Milestone**: `Phase 6: 테스트 및 배포`
- **Description**:
  ```markdown
  ## 목표
  메모리 누수 없는 안정적인 프로그램

  ## 작업 내용
  - [ ] Dr. Memory 또는 Valgrind로 메모리 누수 체크
  - [ ] 모든 malloc/free 쌍 확인
  - [ ] GDI/GDI+ 객체 해제 확인
  - [ ] 윈도우 핸들 정리 확인
  - [ ] 장시간 실행 테스트

  ## 목표
  - 대기 중: < 5MB
  - 활성 중: < 30MB
  - 누수: 0 bytes
  ```

#### Issue #16: 성능 최적화 및 크기 최적화
- **Title**: `[Phase 6] 성능 최적화 및 실행 파일 크기 최적화`
- **Labels**: `optimization`
- **Milestone**: `Phase 6: 테스트 및 배포`
- **Description**:
  ```markdown
  ## 목표
  < 3MB 실행 파일, 최고 성능

  ## 작업 내용
  - [ ] CMake Release 빌드 최적화 (-Os)
  - [ ] 사용하지 않는 코드 제거
  - [ ] 정적 링크 최적화
  - [ ] 심볼 제거 (strip)
  - [ ] UPX 압축 (선택 사항)
  - [ ] 성능 프로파일링

  ## 목표
  - 실행 파일: < 3MB
  - 시작 시간: < 300ms
  - CPU (대기): < 0.5%
  - 확대 응답: < 100ms
  ```

#### Issue #17: 다양한 환경 테스트
- **Title**: `[Phase 6] 다양한 환경에서 테스트`
- **Labels**: `testing`
- **Milestone**: `Phase 6: 테스트 및 배포`
- **Description**:
  ```markdown
  ## 목표
  다양한 Windows 환경에서 안정성 검증

  ## 테스트 환경
  - [ ] Windows 10 (1809, 21H2)
  - [ ] Windows 11
  - [ ] 다양한 해상도 (FHD, QHD, 4K)
  - [ ] 다양한 DPI (100%, 125%, 150%, 200%)
  - [ ] 멀티모니터

  ## 테스트 시나리오
  - [ ] 확대 → 패닝 → 종료
  - [ ] 그리기 → Undo → Clear → 종료
  - [ ] 스크린샷 → 저장 → 클립보드
  - [ ] 설정 변경 → 저장 → 재시작 확인
  ```

#### Issue #18: 문서화 및 배포 준비
- **Title**: `[Phase 6] 문서화 및 배포 준비`
- **Labels**: `documentation`, `deployment`
- **Milestone**: `Phase 6: 테스트 및 배포`
- **Description**:
  ```markdown
  ## 목표
  사용자 문서 및 배포 패키지

  ## 작업 내용
  - [ ] README.md 업데이트 (사용법, 빌드 방법)
  - [ ] CHANGELOG.md 작성
  - [ ] 사용자 가이드 작성
  - [ ] 스크린샷/데모 GIF
  - [ ] Inno Setup 설치 프로그램
  - [ ] GitHub Release 준비

  ## 배포 형태
  - LetsZoom.exe (단일 실행 파일)
  - LetsZoom_Setup.exe (설치 프로그램)
  - LetsZoom_Portable.zip (포터블 버전)
  ```

## 5. 라벨 (Labels) 생성

GitHub 저장소 → Issues → Labels에서 생성:

- `setup` (초록색) - 환경 설정
- `core` (빨간색) - 핵심 기능
- `feature` (파란색) - 새 기능
- `ui` (보라색) - UI/UX
- `implementation` (노란색) - 구현 작업
- `testing` (주황색) - 테스트
- `optimization` (하늘색) - 최적화
- `documentation` (회색) - 문서화
- `build` (갈색) - 빌드 시스템
- `deployment` (분홍색) - 배포

## 6. 프로젝트 보드 (선택 사항)

GitHub Projects를 사용하여 칸반 보드 생성:

1. Projects 탭 → New project
2. Board 템플릿 선택
3. 컬럼:
   - 📋 **To Do**: 예정된 작업
   - 🚧 **In Progress**: 진행 중
   - ✅ **Done**: 완료

Issues를 드래그 앤 드롭으로 관리할 수 있습니다.

## 7. 자동화 스크립트 (gh CLI 설치 후)

```bash
# GitHub CLI 설치
winget install --id GitHub.cli

# 로그인
gh auth login

# Milestones 생성
gh api repos/OWNER/REPO/milestones -f title="Phase 1: 프로젝트 셋업" -f description="..."
gh api repos/OWNER/REPO/milestones -f title="Phase 2: 전역 단축키 및 설정" -f description="..."
# ... (나머지 Milestones)

# Issues 생성
gh issue create --title "[Phase 1] VS Code 개발 환경 설정" --body "..." --label "setup,documentation" --milestone "Phase 1: 프로젝트 셋업"
gh issue create --title "[Phase 1] CMake 빌드 시스템 구성" --body "..." --label "setup,build" --milestone "Phase 1: 프로젝트 셋업"
# ... (나머지 Issues)
```

## 다음 단계

1. ✅ Git 저장소 초기화 완료
2. ⬜ GitHub 저장소 생성
3. ⬜ 원격 저장소 연결 및 푸시
4. ⬜ Milestones 6개 생성
5. ⬜ Issues 18개 생성
6. ⬜ Labels 생성
7. ⬜ (선택) Project 보드 생성

준비가 되면 개발을 시작할 수 있습니다!
