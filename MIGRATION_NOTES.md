# WebEngine 移除遷移說明

## 變更概要

為了提高程式的可移植性和減少依賴，本專案已將 Qt WebEngineWidgets 模組移除，改用標準 Qt 組件。

## 主要變更

### 1. 類別替換

**之前**: 使用 `QWebEngineView` 顯示嵌入式 YouTube 影片
```cpp
QWebEngineView* videoWebView;
```

**現在**: 使用 `QTextBrowser` 顯示 HTML 內容和可點擊連結
```cpp
QTextBrowser* videoDisplayArea;
```

### 2. 功能變更

#### YouTube 影片播放

**之前**:
- 影片在應用程式內嵌入播放
- 使用 YouTube 嵌入式播放器 API
- 需要 Qt WebEngine 模組

**現在**:
- 顯示 YouTube 連結
- 點擊連結在系統預設瀏覽器中開啟
- 不需要額外的 Qt 模組

#### 本地音樂播放

**保持不變**:
- 使用 QMediaPlayer 播放本地音樂檔案
- 所有播放控制功能正常運作
- Whisper 語音轉錄功能正常運作

### 3. 依賴變更

#### 移除的依賴
- Qt WebEngineWidgets
- Qt WebEngineCore (間接依賴)

#### 新增的依賴
- QDesktopServices (標準 Qt 模組，用於開啟瀏覽器)
- QTextBrowser (標準 Qt 模組，已包含在 Qt Widgets 中)

### 4. 建置配置變更

#### CMakeLists.txt
```cmake
# 移除
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    ...
    WebEngineWidgets  # 已移除
)

target_link_libraries(last-report PRIVATE
    ...
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets  # 已移除
)
```

#### last-report.pro
```qmake
# 之前
QT += core gui multimedia multimediawidgets webenginewidgets

# 現在
QT += core gui multimedia multimediawidgets
```

## 使用者體驗變更

### YouTube 影片播放流程

**之前**:
1. 貼上 YouTube 連結
2. 點擊「播放」
3. 影片在應用程式內播放

**現在**:
1. 貼上 YouTube 連結
2. 點擊「播放」
3. 顯示影片資訊和連結
4. 點擊連結在瀏覽器中播放

### 優點

1. **更輕量**: 不需要安裝龐大的 WebEngine 模組
2. **更廣泛的相容性**: 可在任何標準 Qt 安裝中運行
3. **更簡單的建置**: 減少編譯時間和依賴問題
4. **更好的性能**: 使用系統瀏覽器播放，不佔用應用程式資源

### 缺點

1. **需要額外操作**: 需要點擊連結開啟瀏覽器
2. **不是無縫體驗**: 影片在瀏覽器中播放，而非應用程式內

## 升級指南

如果您之前使用過包含 WebEngine 的版本：

1. **重新編譯**: 使用更新後的建置配置
2. **移除 WebEngine**: 不再需要安裝 Qt WebEngine 套件
3. **調整使用習慣**: YouTube 影片將在瀏覽器中開啟

## 技術細節

### QTextBrowser 功能

- 支援 HTML 內容顯示
- 支援 CSS 樣式
- 自動處理連結點擊 (`setOpenExternalLinks(true)`)
- 輕量級，無需額外依賴

### 瀏覽器開啟機制

使用 Qt 的 `QTextBrowser::setOpenExternalLinks(true)`，當使用者點擊連結時：
1. 自動呼叫 `QDesktopServices::openUrl()`
2. 使用系統預設瀏覽器開啟 URL
3. 支援所有主流作業系統（Windows、macOS、Linux）

## 測試建議

在更新後，建議測試以下功能：

- [ ] 貼上 YouTube 連結並顯示
- [ ] 點擊連結在瀏覽器中開啟
- [ ] 本地音樂檔案播放
- [ ] 播放清單管理
- [ ] 拖放排序功能
- [ ] Whisper 語音轉錄（如果已安裝）

## 常見問題

### Q: 為什麼移除 WebEngine？
A: WebEngine 模組體積大、依賴多，且不是所有 Qt 安裝都包含。使用標準組件可提高相容性。

### Q: 能否恢復嵌入式播放？
A: 可以，但需要重新加入 WebEngine 依賴。對於大多數使用者，在瀏覽器中播放更加穩定和流暢。

### Q: 本地音樂播放受影響嗎？
A: 完全不受影響。所有本地音樂播放功能保持不變。

### Q: 在沒有瀏覽器的系統上會怎樣？
A: QDesktopServices 會嘗試使用系統預設的 URL 處理程式。如果無法開啟，會顯示錯誤訊息。

## 版本資訊

- 變更日期: 2026-01-03
- 影響版本: v2.0+
- 相容性: 向後不相容（建置配置變更）

## 相關檔案

- `widget.h`: 類別定義更新
- `widget.cpp`: 實作更新
- `CMakeLists.txt`: CMake 建置配置更新
- `last-report.pro`: qmake 建置配置更新
- `README.md`: 文件更新
