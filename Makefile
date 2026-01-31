# LetsZoom Makefile
# CMake 없이 빌드하기 위한 대체 방법

# 컴파일러 및 플래그
CC = gcc
WINDRES = windres

# 소스 파일
SOURCES = src/main.c src/tray.c src/hotkey.c src/settings.c
OBJECTS = $(SOURCES:.c=.o)

# 타겟 이름
TARGET = LetsZoom.exe

# Windows 라이브러리
LIBS = -luser32 -lgdi32 -lgdiplus -lcomctl32 -lshell32 -lshlwapi -lole32 -luuid

# 공통 플래그
CFLAGS = -Wall -Wextra -DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN

# Debug 플래그
DEBUG_FLAGS = -g -mconsole

# Release 플래그 (크기 최적화)
RELEASE_FLAGS = -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti
RELEASE_LDFLAGS = -mwindows -Wl,--gc-sections -Wl,--strip-all -static -static-libgcc -static-libstdc++

# 기본 타겟
.PHONY: all clean debug release run help

all: release

# Debug 빌드
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)
	@echo "Debug build completed: $(TARGET)"
	@ls -lh $(TARGET) 2>/dev/null || dir $(TARGET)

# Release 빌드
release: CFLAGS += $(RELEASE_FLAGS)
release: LDFLAGS += $(RELEASE_LDFLAGS)
release: $(TARGET)
	@echo "Release build completed: $(TARGET)"
	@ls -lh $(TARGET) 2>/dev/null || dir $(TARGET)

# 링크
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# 컴파일
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 정리
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean completed"

# 실행
run: $(TARGET)
	./$(TARGET)

# 도움말
help:
	@echo "LetsZoom Makefile"
	@echo ""
	@echo "Usage:"
	@echo "  make              - Release 빌드 (기본)"
	@echo "  make release      - Release 빌드 (최적화)"
	@echo "  make debug        - Debug 빌드"
	@echo "  make clean        - 정리"
	@echo "  make run          - 실행"
	@echo "  make help         - 도움말"
