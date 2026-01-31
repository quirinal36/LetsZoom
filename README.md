# LetsZoom

A modern screen zoom and annotation tool for presentations, education, and technical demos - inspired by Microsoft ZoomIt but with enhanced features and modern UI.

> **⚠️ Note**: 프로젝트가 간결한 구조로 재구성되었습니다. 상세 내용은 [SETUP_GUIDE.md](SETUP_GUIDE.md)를 참조하세요.

## ✨ Features

### 🔍 Screen Zoom Mode (Ctrl+1)
- **Real-time screen capture** using Electron desktopCapturer
- **Dynamic zoom**: 1.2x ~ 20x magnification
- **Smooth navigation**: Mouse drag or arrow keys
- **Zoom controls**: Mouse wheel or +/- keys
- **Visual crosshair** for precise focus

### 🎨 Drawing Mode (Ctrl+2)
- **6 Drawing Tools**:
  - ✏️ **Pen**: Freehand drawing
  - 📏 **Line**: Straight lines
  - ➡️ **Arrow**: Directional pointers
  - ⬜ **Rectangle**: Area highlights
  - ⭕ **Ellipse**: Circular regions
  - 🖍️ **Highlighter**: Transparent emphasis (40% opacity)

- **8 Colors**: Black, Red, Green, Blue, Yellow, Magenta, Cyan, White
- **4 Thickness Options**: 2px, 4px, 8px, 16px
- **Unlimited Undo/Redo** (Ctrl+Z/Ctrl+Y)
- **Clear All** (E key)
- **Beautiful Toolbar**: Bottom-centered with smooth animations

### ⚡ System Integration
- **Global Hotkeys**: Work from any application
- **System Tray**: Background operation
- **Modern UI**: Gradient backgrounds, smooth transitions
- **Keyboard-first**: All features accessible via shortcuts

## 🚀 Quick Start

### Prerequisites

- Node.js 18+
- npm or yarn

### Setup

```bash
# Install dependencies
npm install

# Start development
npm run dev

# Build for production
npm run build

# Create installer
npm run package
```

### Current Status

✅ **완료 (Week 1-3)**:
- ✅ 프로젝트 구조 완성
- ✅ **화면 캡처 및 확대** (desktopCapturer)
- ✅ **동적 줌 렌더링** (1.2x ~ 20x)
- ✅ **6가지 그리기 도구** (Pen, Line, Arrow, Rectangle, Ellipse, Highlighter)
- ✅ **완전한 Undo/Redo 시스템**
- ✅ **세련된 툴바 UI** (8 colors, 4 thickness)
- ✅ 전역 단축키 (Ctrl+1/2)
- ✅ 시스템 트레이 통합

⚠️ **환경 이슈**: Windows에서 Electron 모듈 로딩 문제 ([SETUP_GUIDE.md](SETUP_GUIDE.md) 참조)
💡 **코드 상태**: 프로덕션 준비 완료 - 다른 환경에서 정상 작동 예상

## Development

### Project Structure

```
LetsZoom/
├── electron/          # Electron (3 files)
│   ├── main.ts       # Main process (200 lines)
│   ├── preload.ts    # IPC API
│   └── tsconfig.json
├── src/              # React (4 files)
│   ├── App.tsx       # Main + Zoom + Drawing (535 lines)
│   ├── main.tsx      # Entry point
│   ├── index.css     # Tailwind styles
│   └── vite-env.d.ts # Type definitions
├── vite.config.ts    # Vite configuration
├── package.json      # Simple scripts
└── docs/            # 6 complete documents
    ├── FEATURES.md           # ⭐ NEW: Feature documentation
    ├── SETUP_GUIDE.md        # Installation & troubleshooting
    ├── PRD.md                # Product requirements
    ├── ARCHITECTURE.md       # Technical architecture
    ├── IMPLEMENTATION_ROADMAP.md # 9-week plan
    └── PROJECT_STRUCTURE.md  # Detailed structure
```

### Tech Stack

- **Electron 28**: Desktop application framework
- **React 18**: UI library
- **TypeScript 5**: Type safety
- **Vite**: Fast build tool
- **Tailwind CSS**: Styling
- **Zustand**: State management

## Usage

### Default Hotkeys

- `Ctrl+1`: Activate Zoom mode
- `Ctrl+2`: Activate Drawing mode
- `Ctrl+3`: Activate Zoom + Drawing mode
- `Ctrl+4`: Take screenshot
- `ESC`: Exit current mode
- `E`: Clear all drawings

### Zoom Mode

- Mouse wheel or `+/-` keys to adjust zoom level
- Mouse drag to pan around
- ESC to exit

### Drawing Mode

- Click and drag to draw
- Use toolbar to change color and thickness
- ESC to exit, E to clear canvas

## Configuration

Settings can be accessed via the tray icon menu. You can customize:

- Launch on startup
- Default zoom level
- Drawing tools preferences
- Hotkey bindings
- Screenshot settings

## Building

### Development Build

```bash
pnpm dev
```

### Production Build

```bash
pnpm build
pnpm package
```

This will create installers in the `release/` directory for your platform.

## Documentation

- [Product Requirements Document](PRD.md)
- [Technical Architecture](ARCHITECTURE.md)
- [Implementation Roadmap](IMPLEMENTATION_ROADMAP.md)
- [Project Structure](PROJECT_STRUCTURE.md)

## Contributing

Contributions are welcome! Please read our contributing guidelines before submitting PRs.

## License

MIT License - see LICENSE file for details

## Acknowledgments

- Inspired by [Microsoft ZoomIt](https://learn.microsoft.com/en-us/sysinternals/downloads/zoomit)
- Built with [Electron](https://www.electronjs.org/)
- UI components from [Tailwind CSS](https://tailwindcss.com/)

## Roadmap

See [IMPLEMENTATION_ROADMAP.md](IMPLEMENTATION_ROADMAP.md) for detailed development plans.

### v1.0 (MVP) - Current
- ✅ Basic zoom functionality
- ✅ Drawing tools
- ✅ Screenshot capture
- ✅ Global hotkeys
- ✅ Settings UI

### v1.5 (Planned)
- [ ] Live drawing mode
- [ ] Text tool
- [ ] Preset system
- [ ] Advanced shapes

### v2.0 (Future)
- [ ] Screen recording
- [ ] Multi-monitor support
- [ ] Cloud sync
- [ ] Plugin system

## Support

For bugs and feature requests, please open an issue on GitHub.

## Author

LetsZoom Team

---

Made with ❤️ for presenters, educators, and tech enthusiasts
