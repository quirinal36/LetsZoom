# 빌드 가이드

## 1. 빌드 도구 설치

### 방법 1: CMake (권장)

#### Windows - MSYS2 사용
```bash
# MSYS2 터미널에서
pacman -S mingw-w64-x86_64-cmake
```

#### Windows - Chocolatey 사용
```bash
choco install cmake
```

#### Windows - 수동 설치
1. https://cmake.org/download/ 접속
2. "Windows x64 Installer" 다운로드
3. 설치 시 "Add CMake to the system PATH" 선택

#### 설치 확인
```bash
cmake --version
```

### 방법 2: Make만 사용 (CMake 없이)
- MinGW-w64가 설치되어 있으면 `make` 명령 사용 가능
- `Makefile` 제공 (아래 참조)

## 2. 빌드 방법

### 방법 A: CMake + MinGW (권장)

#### Debug 빌드
```bash
# 1. CMake 구성
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug

# 2. 컴파일
cmake --build build

# 3. 실행
./build/LetsZoom.exe
```

#### Release 빌드 (최적화)
```bash
# 1. CMake 구성
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# 2. 컴파일
cmake --build build

# 3. 실행
./build/LetsZoom.exe
```

#### 빌드 정리
```bash
rm -rf build
```

### 방법 B: Make만 사용 (CMake 없이)

#### Debug 빌드
```bash
make debug
```

#### Release 빌드
```bash
make release
```

#### 정리
```bash
make clean
```

#### 실행
```bash
make run
```

### 방법 C: VS Code 통합 빌드

#### 빌드 단축키
- `Ctrl+Shift+B`: Release 빌드 (기본)
- `Ctrl+Shift+P` → "Tasks: Run Build Task"

#### 디버깅
- `F5`: 디버깅 시작
- `Ctrl+F5`: 디버깅 없이 실행

## 3. 빌드 옵션

### CMake 옵션
```bash
# 빌드 타입 선택
-DCMAKE_BUILD_TYPE=Debug|Release

# 컴파일러 선택
-DCMAKE_C_COMPILER=gcc
-DCMAKE_CXX_COMPILER=g++

# 생성기 선택
-G "MinGW Makefiles"
-G "Ninja"
-G "Visual Studio 17 2022"
```

### 컴파일러 최적화 플래그
#### Debug
- `-g`: 디버그 심볼
- `-Wall -Wextra`: 모든 경고
- `-mconsole`: 콘솔 표시

#### Release
- `-Os`: 크기 최적화
- `-ffunction-sections -fdata-sections`: 섹션 분리
- `-Wl,--gc-sections`: 사용하지 않는 코드 제거
- `-Wl,--strip-all`: 심볼 제거
- `-static`: 정적 링크 (런타임 DLL 불필요)

## 4. 예상 빌드 결과

### Debug 빌드
```
실행 파일: build/LetsZoom.exe
크기: ~5-10 MB (디버그 심볼 포함)
```

### Release 빌드
```
실행 파일: build/LetsZoom.exe
크기: ~2-3 MB (최적화됨)
```

### UPX 압축 후 (선택 사항)
```bash
upx --best --lzma build/LetsZoom.exe
크기: ~1-1.5 MB
```

## 5. 빌드 문제 해결

### "cmake: command not found"
**해결**: CMake 설치 후 PATH 환경 변수 확인
```bash
# Windows
where cmake

# MSYS2
which cmake
```

### "gcc: command not found"
**해결**: MinGW-w64 설치 확인
```bash
gcc --version
```

### "cannot find -luser32"
**해결**: MinGW-w64가 제대로 설치되었는지 확인
```bash
# MSYS2에서
pacman -S mingw-w64-x86_64-toolchain
```

### 한글 깨짐
**해결**: 소스 파일을 UTF-8 (BOM 없음)으로 저장

### 빌드는 성공하지만 실행 안 됨
**해결**:
1. 필요한 DLL이 없는지 확인
2. `Dependency Walker` 사용하여 의존성 확인
3. `-static` 링크 옵션 사용

## 6. 빌드 스크립트 (선택 사항)

### build.sh (Git Bash / MSYS2)
```bash
#!/bin/bash

# Release 빌드
build_release() {
    echo "Building Release..."
    cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    echo "Build completed: build/LetsZoom.exe"
    ls -lh build/LetsZoom.exe
}

# Debug 빌드
build_debug() {
    echo "Building Debug..."
    cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
    cmake --build build
    echo "Build completed: build/LetsZoom.exe"
}

# 정리
clean() {
    echo "Cleaning build directory..."
    rm -rf build
}

# 메인
case "$1" in
    release)
        build_release
        ;;
    debug)
        build_debug
        ;;
    clean)
        clean
        ;;
    *)
        echo "Usage: $0 {release|debug|clean}"
        exit 1
        ;;
esac
```

사용법:
```bash
chmod +x build.sh
./build.sh release
./build.sh debug
./build.sh clean
```

## 7. CI/CD (GitHub Actions)

나중에 GitHub Actions로 자동 빌드를 설정할 예정입니다.

## 8. 다음 단계

빌드가 성공하면 다음 Issue로 진행:
- Issue #3: 기본 윈도우 및 메시지 루프 구현
