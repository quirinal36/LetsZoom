cmake_minimum_required(VERSION 3.15)
project(LetsZoom C CXX)

# C/C++ 표준 설정
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows 전용 플래그
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

# 소스 파일
set(SOURCES
    src/main.c
    src/tray.c
    src/tray.h
    src/hotkey.c
    src/hotkey.h
    src/settings.c
    src/settings.h
    src/zoom_overlay.c
    src/zoom_overlay.h
    src/drawing_overlay.c
    src/drawing_overlay.h
)

# 실행 파일 생성 (WIN32 = GUI 애플리케이션)
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Windows 라이브러리 링크
if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        user32      # 윈도우 관리
        gdi32       # GDI 그리기
        gdiplus     # GDI+ (PNG/JPG)
        comctl32    # 공통 컨트롤
        shell32     # 셸 API (트레이 아이콘)
        shlwapi     # 셸 유틸리티
        ole32       # OLE/COM
        uuid        # UUID
    )
endif()

# 컴파일 옵션
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Debug 빌드
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall                   # 모든 경고 활성화
        -Wextra                 # 추가 경고
        -g                      # 디버그 심볼
    )
    if(WIN32)
        target_link_options(${PROJECT_NAME} PRIVATE
            -mconsole           # 콘솔 표시 (디버그용)
        )
    endif()
else()
    # Release 빌드 - 크기 최적화
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Os                     # 크기 최적화
        -ffunction-sections     # 함수별 섹션 분리
        -fdata-sections         # 데이터별 섹션 분리
        -fno-exceptions         # 예외 비활성화 (C에서 불필요)
        -fno-rtti               # RTTI 비활성화
    )

    if(WIN32)
        target_link_options(${PROJECT_NAME} PRIVATE
            -mwindows               # GUI 애플리케이션 (콘솔 없음)
            -Wl,--gc-sections       # 사용하지 않는 섹션 제거
            -Wl,--strip-all         # 모든 심볼 제거
            -static                 # 정적 링크 (런타임 DLL 불필요)
            -static-libgcc          # libgcc 정적 링크
            -static-libstdc++       # libstdc++ 정적 링크
        )
    endif()
endif()

# 출력 디렉토리 설정
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}
)

# 빌드 정보 출력
message(STATUS "=================================")
message(STATUS "LetsZoom Build Configuration")
message(STATUS "=================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "CXX Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "=================================")
