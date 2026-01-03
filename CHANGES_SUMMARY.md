# 變更摘要 - 移除 WebEngine 依賴

## 概述

本次更新成功將 Qt WebEngineWidgets 依賴從專案中移除，使用標準 Qt 組件替代。這使得應用程式更輕量、更易於建置，並且可以在任何標準 Qt 安裝中運行。

## 主要變更

### 1. 核心組件替換

| 之前 | 現在 | 說明 |
|------|------|------|
| `QWebEngineView` | `QTextBrowser` | 顯示 HTML 內容和可點擊連結 |
| 嵌入式 YouTube 播放器 | 瀏覽器連結 | 在系統瀏覽器中開啟 YouTube |
| Qt WebEngineWidgets 模組 | 標準 Qt Widgets | 無需額外依賴 |

### 2. 檔案變更清單

#### 程式碼檔案
- **widget.h**: 
  - 移除 `#include <QWebEngineView>`
  - 新增 `#include <QTextBrowser>`
  - 新增 `#include <QDesktopServices>`
  - 將 `QWebEngineView* videoWebView` 改為 `QTextBrowser* videoDisplayArea`
  - 新增輔助函數聲明

- **widget.cpp**:
  - 更新所有 `videoWebView` 引用為 `videoDisplayArea`
  - 實作新的輔助函數：
    - `generateWelcomeHTML()` - 歡迎畫面
    - `generateYouTubeDisplayHTML()` - YouTube 顯示
    - `generateLocalMusicHTML()` - 本地音樂顯示
    - `updateVideoLabels()` - 更新標籤
  - 新增共用 CSS 樣式常數 `BASE_HTML_STYLE`
  - YouTube 連結改為在瀏覽器中開啟

#### 建置配置
- **CMakeLists.txt**:
  - 移除 `WebEngineWidgets` 從 `find_package`
  - 移除 `Qt${QT_VERSION_MAJOR}::WebEngineWidgets` 從 `target_link_libraries`

- **last-report.pro**:
  - 從 `QT` 變數移除 `webenginewidgets`

#### 文件
- **README.md**: 
  - 更新系統需求（移除 WebEngine）
  - 更新安裝說明（移除 WebEngine 套件）
  - 更新技術實現說明
  - 更新使用說明

- **MIGRATION_NOTES.md**: 新增完整的遷移指南

- **CHANGES_SUMMARY.md**: 本檔案，提供變更摘要

### 3. 功能變更

#### YouTube 影片播放
**之前**:
```cpp
QString embedUrl = QString("https://www.youtube.com/embed/%1?autoplay=1").arg(videoId);
videoWebView->setUrl(QUrl(embedUrl));
```

**現在**:
```cpp
videoDisplayArea->setHtml(generateYouTubeDisplayHTML(title, channel, videoId));
// 使用者點擊連結 -> 在瀏覽器中開啟
```

#### 本地音樂播放
**保持不變**: 使用 `QMediaPlayer` 播放，功能完全正常

### 4. 程式碼品質改進

#### 消除重複程式碼
1. **HTML 生成**: 提取為獨立函數
   - `generateYouTubeDisplayHTML()`
   - `generateLocalMusicHTML()`
   - `generateWelcomeHTML()`

2. **CSS 樣式**: 提取共用樣式
   - `BASE_HTML_STYLE` 常數
   - 在所有 HTML 生成函數中重用

3. **標籤更新**: 提取為輔助函數
   - `updateVideoLabels()`

#### 程式碼行數減少
- 移除約 100+ 行重複的 HTML/CSS 程式碼
- 新增約 70 行輔助函數
- 淨減少約 30 行程式碼
- 提高程式碼可維護性

## 優點

### 對開發者
1. ✅ **更簡單的建置**: 不需要安裝 WebEngine
2. ✅ **更快的編譯**: WebEngine 模組很大
3. ✅ **更好的相容性**: 標準 Qt 安裝即可
4. ✅ **更易於維護**: 程式碼更簡潔

### 對使用者
1. ✅ **更小的安裝包**: 不包含 WebEngine
2. ✅ **更快的啟動**: 不載入 WebEngine
3. ✅ **更穩定**: 使用成熟的系統瀏覽器
4. ✅ **更好的性能**: YouTube 在瀏覽器中播放效能更好

## 影響

### 正面影響
- 相容性提升：可在任何有 Qt Multimedia 的系統上運行
- 依賴減少：從 6 個 Qt 模組減少到 5 個
- 檔案大小：可執行檔大小減少（不鏈接 WebEngine）

### 負面影響
- 使用者體驗：YouTube 需要在瀏覽器中開啟（增加一個步驟）
- 整合度：不再是「一站式」應用程式

## 測試建議

### 功能測試
- [ ] YouTube 連結顯示正確
- [ ] 點擊連結在瀏覽器中開啟
- [ ] 本地音樂正常播放
- [ ] 播放清單功能正常
- [ ] UI 顯示正確
- [ ] 歡迎畫面顯示正確

### 建置測試
- [ ] CMake 建置成功
- [ ] qmake 建置成功
- [ ] 在 Linux 上建置
- [ ] 在 Windows 上建置
- [ ] 在 macOS 上建置

## 版本資訊

- **變更日期**: 2026-01-03
- **PR 分支**: `copilot/remove-webenginewidgets-dependency`
- **影響版本**: v2.0+
- **向後相容性**: 不相容（需要重新編譯）

## 提交歷史

1. `c18bb05` - Replace QWebEngineView with QTextBrowser to remove webenginewidgets dependency
2. `b8746da` - Update documentation to reflect WebEngine removal
3. `e278c6a` - Refactor: Extract helper functions to reduce code duplication
4. `199b050` - Further refactoring: Extract common CSS and welcome HTML

## 下一步

建議使用者：
1. 重新編譯應用程式
2. 測試所有功能
3. 如遇問題請回報
4. 考慮是否需要進一步的功能改進

## 技術細節

### QTextBrowser 特性
- 支援 HTML 4.01 子集
- 支援 CSS 樣式
- 自動處理連結點擊
- 輕量級（<100KB vs WebEngine 50+MB）

### 瀏覽器整合
使用 `QTextBrowser::setOpenExternalLinks(true)` 自動處理連結：
```cpp
videoDisplayArea->setOpenExternalLinks(true);
// 點擊連結 -> QDesktopServices::openUrl() -> 系統預設瀏覽器
```

## 相關資源

- [Qt QTextBrowser 文件](https://doc.qt.io/qt-6/qtextbrowser.html)
- [Qt QDesktopServices 文件](https://doc.qt.io/qt-6/qdesktopservices.html)
- [專案 README](README.md)
- [遷移指南](MIGRATION_NOTES.md)

## 聯絡資訊

如有問題或建議，請在 GitHub 上提交 Issue。
