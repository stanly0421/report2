# 音樂播放器 (Music Player)

這是一個使用 Qt6 C++ 開發的音樂播放器，靈感來自 Spotify 的設計風格。支援本地音樂檔案播放和 YouTube 影片連結。

## 快速開始

1. **編譯**: 使用 CMake 或 qmake (詳見下方說明)
2. **使用**: 貼上 YouTube 連結或選擇本地音樂檔案開始播放

## 功能特色

### 1. 本地音樂播放
- 支援多種音樂格式：MP3、WAV、FLAC、M4A、OGG、AAC
- 使用檔案對話框選擇音樂檔案
- 內建音樂播放器，支援播放/暫停控制
- 自動顯示歌曲名稱（從檔案名提取）
- **即時字幕顯示**: 支援 Whisper 語音轉文字功能
- **可點擊時間戳**: 點擊字幕中的時間戳可直接跳轉到該位置播放

### 2. 播放清單管理
- 創建和管理多個播放清單
- 儲存本地音樂到播放清單（也支援 YouTube 連結的歷史數據）
- 支援從播放清單播放音樂
- 雙擊播放清單項目即可播放
- **拖放排序功能**: 可以用滑鼠拖放調整播放清單中歌曲的順序
- 點擊歌曲即可播放

### 3. 播放控制
- 播放/暫停按鈕（本地音樂支援實際控制）
- 上一首/下一首功能（完整支援）
- 隨機播放模式
- 循環播放模式
- 支援背景播放
- **時間軸跳轉**: 透過點擊字幕時間戳快速跳轉

### 4. Whisper 語音轉錄與字幕功能
- 使用 QProcess 執行 Python Whisper 腳本
- 即時讀取 Whisper 輸出並顯示字幕
- 自動更新字幕顯示
- 支援本地音樂檔案的語音轉文字
- **時間戳顯示**: 每段字幕都標示開始和結束時間
- **點擊跳轉**: 點擊任何時間戳即可跳轉到該時間點播放
- 字幕區域自動滾動顯示最新內容

### 5. 我的最愛功能
- 一鍵加入/移除最愛
- 自動創建「我的最愛」播放清單
- 快速訪問喜愛的音樂

### 6. Spotify 風格 UI
- 深色主題設計
- 現代化的介面佈局
- 綠色強調色 (#1DB954)
- 直覺的操作體驗
- 響應式設計

## 操作說明

### 播放本地音樂
1. 點擊頂部的「📁 載入音樂檔案」按鈕
2. 選擇音樂檔案
3. 音樂將自動開始播放
4. 使用播放控制按鈕控制播放
5. 字幕將自動顯示（需安裝 Whisper）
6. **點擊時間戳跳轉**: 點擊字幕中顯示的時間戳（如 [0.00s - 5.23s]）可直接跳到該時間點

### 播放控制
- **▶ 播放/⏸ 暫停**: 開始或暫停播放當前音樂
- **⏮ 上一首**: 切換到上一首音樂
- **⏭ 下一首**: 切換到下一首音樂
- **🔀 隨機**: 啟用隨機播放模式
- **🔁 循環**: 啟用循環播放模式
- **❤️ 加入最愛**: 將當前音樂加入最愛

### 字幕功能使用說明
1. **自動顯示**: 播放本地音樂時，Whisper 會自動開始轉錄並顯示字幕
2. **時間戳顯示**: 每段字幕都會顯示時間範圍，例如：[0.00s - 5.23s]
3. **點擊跳轉**: 點擊任何時間戳即可跳轉到該時間點播放
4. **即時更新**: 字幕會在轉錄過程中即時更新
5. **自動滾動**: 字幕區域會自動滾動到最新內容

### 播放清單管理
- **➕ 新增**: 創建新的播放清單
- **🗑️ 刪除**: 刪除當前播放清單（至少保留一個）
- **雙擊音樂**: 立即播放該音樂
- **拖放排序**: 用滑鼠拖動歌曲可重新排列播放順序

### 語音轉錄 (Whisper)
- 播放本地音樂時自動啟動轉錄
- 即時顯示語音轉文字結果，帶時間戳
- **點擊時間戳可跳轉**: 點擊字幕中的時間範圍可直接跳到該時間點播放
- 需要安裝 Python 和 Whisper 模組

## 編譯和運行

### 系統需求
- Qt 6.x 或更高版本（也支援 Qt 5.x）
- Qt Multimedia 模組
- C++17 編譯器
- 穩定的網路連線（播放 YouTube 影片時）
- Python 3.x 和 Whisper（選用，用於語音轉錄）

**注意**: 本專案不再需要 Qt WebEngineWidgets 模組，可以在任何標準 Qt 安裝中運行。

### 安裝依賴

#### Ubuntu/Debian
```bash
# 安裝 Qt 6 和必要模組
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-multimedia-dev

# 或安裝 Qt 5 (如果使用 Qt 5)
sudo apt-get install qtbase5-dev qtmultimedia5-dev

# 安裝 Python 和 Whisper (選用)
sudo apt-get install python3 python3-pip ffmpeg
pip3 install openai-whisper
```

#### Fedora/RHEL/CentOS
```bash
# Qt 6
sudo dnf install qt6-qtbase-devel qt6-qtmultimedia-devel

# 或 Qt 5
sudo dnf install qt5-qtbase-devel qt5-qtmultimedia-devel

# 安裝 Python 和 Whisper (選用)
sudo dnf install python3 python3-pip ffmpeg
pip3 install openai-whisper
```

#### macOS (使用 Homebrew)
```bash
brew install qt@6
# 或 brew install qt@5

# 安裝 Python 和 Whisper (選用)
brew install python ffmpeg
pip3 install openai-whisper
```

#### Windows
1. 下載並安裝 Qt from [qt.io](https://www.qt.io/download)
2. 在安裝時確保選擇以下組件：
   - Qt Multimedia
3. (選用) 安裝 Python 和 Whisper：
   - 下載 Python from [python.org](https://www.python.org)
   - 下載 FFmpeg from [ffmpeg.org](https://ffmpeg.org)
   - 執行: `pip install openai-whisper`

### 編譯

#### 使用 qmake (傳統方式)
```bash
# Qt 6
qmake6 last-report.pro
make

# Qt 5
qmake last-report.pro
make
```

#### 使用 CMake (推薦)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 運行
```bash
./last-report
```

## 技術實現

### 核心類別
- **QMediaPlayer**: 本地音樂播放器
- **QAudioOutput**: 音訊輸出控制
- **QTextBrowser**: HTML 內容顯示和連結開啟
- **QProcess**: 執行 Whisper 語音轉錄腳本
- **QFileDialog**: 檔案選擇對話框
- **QListWidget**: 顯示播放清單（支援拖放排序）
- **QComboBox**: 播放清單選擇器
- **QRegularExpression**: YouTube 連結解析
- **QDesktopServices**: 在瀏覽器中開啟 YouTube 連結

### YouTube 連結處理
- 使用正則表達式解析 YouTube URL
- 支援多種 URL 格式
- 生成標準 YouTube 觀看連結
- 使用 QTextBrowser 顯示可點擊連結
- 點擊連結在系統預設瀏覽器中開啟

### 本地音樂播放
- 使用 Qt Multimedia 框架
- 支援多種音訊格式
- 即時播放控制
- 整合 Whisper 語音轉錄

### 資料持久化
- 使用 JSON 格式儲存播放清單
- 儲存位置：`QStandardPaths::AppDataLocation`
- 自動載入上次的播放清單
- 保存音樂資訊、檔案路徑和最愛狀態

## 界面預覽

播放器界面包含：
- **頂部工具欄**: Logo 和本地音樂載入按鈕
- **左側面板**: 播放清單管理（支援拖放排序）
- **中央面板**: 
  - 音樂資訊顯示
  - 本地音樂資訊顯示區域
  - **字幕/轉錄文字顯示區域**: 帶可點擊時間戳，點擊可跳轉到該時間點
  - 播放控制按鈕

## 注意事項

### YouTube 影片播放
- YouTube 影片連結已從主界面移除
- 歷史播放清單中的 YouTube 連結數據仍會保留（向下兼容）
- 如需播放 YouTube 影片，請在播放清單中雙擊 YouTube 項目（如果存在）

### 本地音樂播放
- 支援常見音訊格式
- 檔案路徑會被儲存在播放清單中
- 確保音樂檔案位置不會移動
- 支援 Whisper 語音轉錄（需額外安裝）

### Whisper 語音轉錄與字幕功能
- 需要安裝 Python 3.x 和 openai-whisper 模組
- 需要安裝 FFmpeg
- 轉錄過程可能需要較長時間（取決於音頻長度）
- 轉錄結果會即時顯示在字幕區域，帶有時間戳
- **互動功能**: 點擊字幕中的時間戳（如 [0.00s - 5.23s]）可跳轉到該時間點
- 字幕區域會自動滾動顯示最新內容

## 未來改進

- 支援播放清單分享功能
- 添加音量控制滑桿
- 支援播放進度條
- 添加播放歷史記錄
- 支援深色/淺色主題切換
- 實作迷你播放器模式
- 支援更多音訊格式
- 添加等化器功能

## 疑難排解 (Troubleshooting)

### 常見問題

- **找不到 qmake**: 確認 Qt 已正確安裝並加入 PATH
- **CMake 找不到 Qt**: 設置 `CMAKE_PREFIX_PATH` 環境變數
- **缺少 DLL (Windows)**: 將 Qt bin 目錄加入 PATH 或複製所需 DLL
- **本地音樂無法播放**: 確認 Qt Multimedia 模組已正確安裝

祝您使用愉快！🎵