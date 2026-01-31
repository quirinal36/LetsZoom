# VS Code 개발 환경 설정 가이드

## 1. 필수 소프트웨어 설치

### 1.1 Visual Studio Code
- https://code.visualstudio.com/ 다운로드 및 설치
- Windows 64-bit 버전 권장

### 1.2 MinGW-w64 (GCC 컴파일러)

#### 방법 1: MSYS2 사용 (권장)
```bash
# 1. MSYS2 다운로드
# https://www.msys2.org/ 접속
# msys2-x86_64-<버전>.exe 다운로드 및 설치

# 2. MSYS2 터미널 열기
# 시작 메뉴 → MSYS2 MSYS

# 3. 패키지 업데이트
pacman -Syu

# 4. MinGW-w64 툴체인 설치
pacman -S mingw-w64-x86_64-toolchain

# 5. CMake 설치
pacman -S mingw-w64-x86_64-cmake

# 6. GDB 디버거 설치
pacman -S mingw-w64-x86_64-gdb
```

#### 방법 2: WinLibs 사용
```bash
# 1. WinLibs 다운로드
# https://winlibs.com/ 접속
# "GCC 13.x + MinGW-w64 UCRT runtime" 다운로드 (zip)

# 2. 압축 해제
# C:\mingw64 에 압축 해제

# 3. 환경 변수 설정 (아래 참조)
```

### 1.3 환경 변수 설정

**시스템 환경 변수 PATH에 추가**:
```
C:\msys64\mingw64\bin
```
또는 (WinLibs 사용 시):
```
C:\mingw64\bin
```

**확인 방법**:
```bash
# PowerShell 또는 CMD 열기
gcc --version
cmake --version
gdb --version
```

## 2. VS Code 확장 설치

### 2.1 필수 확장
VS Code를 열고 **Extensions** (Ctrl+Shift+X)에서 설치:

1. **C/C++** (ms-vscode.cpptools)
   - Microsoft의 공식 C/C++ 확장
   - IntelliSense, 디버깅 지원

2. **C/C++ Extension Pack** (ms-vscode.cpptools-extension-pack)
   - C/C++ 개발에 필요한 확장 모음

3. **CMake** (twxs.cmake)
   - CMakeLists.txt 문법 하이라이팅

4. **CMake Tools** (ms-vscode.cmake-tools)
   - CMake 통합 빌드 도구

5. **Makefile Tools** (ms-vscode.makefile-tools)
   - Makefile 지원

### 2.2 자동 설치
프로젝트를 열면 VS Code가 자동으로 추천 확장을 설치할지 물어봅니다.
`.vscode/extensions.json`에 정의된 확장들이 자동으로 추천됩니다.

## 3. VS Code 설정 파일 확인

프로젝트 폴더의 `.vscode/` 디렉토리에 다음 파일들이 있어야 합니다:

### 3.1 c_cpp_properties.json
IntelliSense 설정 (컴파일러 경로, 포함 경로 등)
```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/src",
                "C:/mingw64/include/**"
            ],
            "compilerPath": "C:/mingw64/bin/gcc.exe",
            "cStandard": "c11",
            ...
        }
    ]
}
```

**주의**: `compilerPath`를 실제 MinGW-w64 설치 경로에 맞게 수정하세요.

### 3.2 tasks.json
빌드 태스크 정의
- `Build (Debug)`: CMake 구성 (Debug)
- `Build (Release)`: CMake 구성 (Release)
- `Compile (Debug)`: 컴파일 실행
- `Compile (Release)`: 컴파일 실행
- `Clean`: 빌드 폴더 삭제
- `Run`: 프로그램 실행

**빌드 단축키**:
- `Ctrl+Shift+B`: 기본 빌드 태스크 (Release)

### 3.3 launch.json
디버깅 설정
- `Debug (GDB)`: 일반 디버깅
- `Debug (Current File)`: 진입점에서 멈춤

**디버깅 단축키**:
- `F5`: 디버깅 시작
- `Ctrl+F5`: 디버깅 없이 실행
- `F9`: 중단점 토글
- `F10`: Step Over
- `F11`: Step Into

### 3.4 settings.json
프로젝트 설정
- 탭 크기: 4 spaces
- 인코딩: UTF-8
- 줄 끝: LF
- C 표준: C11
- C++ 표준: C++17

## 4. IntelliSense 구성 확인

### 4.1 컴파일러 경로 확인
```bash
# MinGW-w64 설치 경로 확인
where gcc
```

출력 예시:
```
C:\msys64\mingw64\bin\gcc.exe
```

### 4.2 c_cpp_properties.json 수정
위에서 확인한 경로를 `c_cpp_properties.json`의 `compilerPath`에 설정:
```json
"compilerPath": "C:/msys64/mingw64/bin/gcc.exe"
```

### 4.3 IntelliSense 재시작
- `Ctrl+Shift+P` → "C/C++: Restart IntelliSense"
- 또는 VS Code 재시작

## 5. 첫 빌드 테스트

### 5.1 프로젝트 열기
```bash
cd c:\Users\Albert\Documents\GitHub\LetsZoom
code .
```

### 5.2 빌드 테스트
1. `Ctrl+Shift+B` (빌드)
2. 또는 Terminal → Run Build Task

### 5.3 디버그 테스트
1. `F5` (디버깅 시작)
2. 처음 실행 시 launch.json 구성 선택
3. "Debug (GDB)" 선택

## 6. 문제 해결

### 6.1 "gcc를 찾을 수 없습니다"
**원인**: gcc가 PATH에 없음
**해결**:
1. MinGW-w64가 설치되었는지 확인
2. 환경 변수 PATH에 `C:\mingw64\bin` 추가
3. VS Code 재시작

### 6.2 "Windows.h를 찾을 수 없습니다"
**원인**: Windows SDK 경로 문제
**해결**:
1. MinGW-w64가 제대로 설치되었는지 확인
2. `c_cpp_properties.json`의 `includePath`에 다음 추가:
   ```json
   "C:/mingw64/x86_64-w64-mingw32/include/**"
   ```

### 6.3 IntelliSense가 작동하지 않음
**해결**:
1. C/C++ 확장이 설치되었는지 확인
2. `Ctrl+Shift+P` → "C/C++: Reset IntelliSense Database"
3. `c_cpp_properties.json`의 `compilerPath` 확인
4. VS Code 재시작

### 6.4 디버거가 시작되지 않음
**원인**: GDB 경로 문제
**해결**:
1. GDB 설치 확인: `gdb --version`
2. `launch.json`의 `miDebuggerPath` 수정:
   ```json
   "miDebuggerPath": "C:/msys64/mingw64/bin/gdb.exe"
   ```

### 6.5 한글 깨짐
**해결**:
1. 소스 파일을 UTF-8 (BOM 없음)으로 저장
2. `settings.json`에서 인코딩 확인:
   ```json
   "files.encoding": "utf8"
   ```

## 7. 추가 도구 (선택 사항)

### 7.1 Clang-Format (코드 포맷팅)
```bash
pacman -S mingw-w64-x86_64-clang-tools-extra
```

VS Code 설정:
```json
{
  "editor.formatOnSave": true,
  "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 100 }"
}
```

### 7.2 Static Analyzer
```bash
pacman -S mingw-w64-x86_64-cppcheck
```

### 7.3 Memory Leak Detector
```bash
pacman -S mingw-w64-x86_64-drmemory
```

## 8. 다음 단계

VS Code 환경 설정이 완료되었습니다!

다음은 **Issue #2: CMake 빌드 시스템 구성**으로 진행하세요.

## 9. 참고 자료

- [VS Code C/C++ 문서](https://code.visualstudio.com/docs/languages/cpp)
- [MinGW-w64 공식 사이트](https://www.mingw-w64.org/)
- [MSYS2 공식 사이트](https://www.msys2.org/)
- [CMake 문서](https://cmake.org/documentation/)
- [GDB 사용법](https://sourceware.org/gdb/documentation/)
