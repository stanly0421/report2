# 更新說明 - YouTube 嵌入式播放與 Whisper 整合

## 已實現的功能

### 1. YouTube 影片嵌入式播放
**問題**: 之前貼上 YouTube 連結只會顯示一個連結，需要跳轉到瀏覽器播放。

**解決方案**: 
- 使用 `QWebEngineView` 取代原本的 `QLabel`
- YouTube 影片現在會直接在應用程式視窗內播放
- 使用 YouTube 的嵌入式播放器 API (`https://www.youtube.com/embed/{videoId}?autoplay=1`)

**相關檔案變更**:
- `CMakeLists.txt`: 加入 `Qt6::WebEngineWidgets` 依賴
- `last-report.pro`: 加入 `webenginewidgets` 模組
- `widget.h`: 新增 `QWebEngineView* videoWebView`
- `widget.cpp`: 
  - 將 `videoDisplayLabel` 改為 `videoWebView`
  - 在 `playYouTubeLink()` 中使用 `videoWebView->setUrl()` 載入嵌入式影片

### 2. 播放/暫停和上下一首按鈕修正
**問題**: 播放控制按鈕不能正常使用。

**解決方案**:
- 本地音樂檔案: 直接控制 `QMediaPlayer` 的播放/暫停狀態
- YouTube 影片: 顯示狀態但提示使用者需在影片播放器內控制
- 上一首/下一首: 完整支援，可在播放清單中切換
- 隨機播放和循環播放: 維持原有功能

**相關檔案變更**:
- `widget.cpp`: 更新 `onPlayPauseClicked()` 邏輯

### 3. 播放清單拖放排序
**問題**: 選取播放清單後，無法調整歌曲順序。

**解決方案**:
- 啟用 `QListWidget` 的拖放模式
- 當項目被移動時，自動更新內部資料結構
- 自動儲存更新後的播放清單

**相關檔案變更**:
- `widget.cpp`:
  - 在 `setupUI()` 中設定 `playlistWidget->setDragDropMode(QAbstractItemView::InternalMove)`
  - 在 `createConnections()` 中監聽 `rowsMoved` 信號並更新播放清單

**使用方法**:
1. 選擇一個播放清單
2. 用滑鼠拖動歌曲項目
3. 放到想要的位置
4. 順序會自動儲存

### 4. Whisper 語音轉錄整合
**問題**: 需要整合 Whisper 進行即時字幕顯示。

**解決方案**:
- 使用 `QProcess` 執行 Python Whisper 腳本
- 當播放本地音樂時自動啟動轉錄
- 透過 `readyReadStandardOutput` 即時讀取輸出
- 在 `QTextEdit` 顯示字幕

**相關檔案變更**:
- `widget.h`: 
  - 新增 `QProcess* whisperProcess`
  - 新增 `QTextEdit* subtitleDisplay`
  - 新增槽函數 `onWhisperOutputReady()` 和 `onWhisperFinished()`
- `widget.cpp`:
  - 在 `setupUI()` 中加入字幕顯示區域
  - 在 `playLocalFile()` 和 `playVideo()` 中呼叫 `startWhisperTranscription()`
  - 實現 Whisper 處理相關函數
- `whisper_transcribe.py`: 新增 Python 腳本範例

**使用方法**:
1. 安裝 Whisper: `pip3 install openai-whisper`
2. 安裝 FFmpeg: `sudo apt install ffmpeg` (Ubuntu)
3. 播放本地音樂檔案
4. 字幕會自動出現在字幕顯示區域

## 技術細節

### Qt WebEngine 整合
```cpp
// 初始化
videoWebView = new QWebEngineView(centerPanel);

// 播放 YouTube 影片
QString embedUrl = QString("https://www.youtube.com/embed/%1?autoplay=1").arg(videoId);
videoWebView->setUrl(QUrl(embedUrl));

// 顯示本地內容
videoWebView->setHtml(displayHTML);
```

### 拖放排序實現
```cpp
// 啟用拖放模式
playlistWidget->setDragDropMode(QAbstractItemView::InternalMove);
playlistWidget->setDefaultDropAction(Qt::MoveAction);

// 監聽項目移動事件
connect(playlistWidget->model(), &QAbstractItemModel::rowsMoved, 
        [this](...) {
            // 更新內部資料結構
            // 儲存到檔案
        });
```

### Whisper 整合
```cpp
// 啟動 Whisper 處理程序
QStringList arguments;
arguments << "whisper_transcribe.py" << audioFilePath;
whisperProcess->start("python3", arguments);

// 讀取即時輸出
connect(whisperProcess, &QProcess::readyReadStandardOutput, 
        this, &Widget::onWhisperOutputReady);

void Widget::onWhisperOutputReady() {
    QByteArray output = whisperProcess->readAllStandardOutput();
    QString text = QString::fromUtf8(output).trimmed();
    subtitleDisplay->append(text);
}
```

## 編譯需求

### 新增依賴
- Qt WebEngineWidgets 模組
- Python 3.x (選用，用於 Whisper)
- openai-whisper (選用，用於語音轉錄)
- FFmpeg (選用，用於 Whisper)

### 編譯指令
```bash
# Ubuntu/Debian
sudo apt-get install qt6-webengine-dev

# CMake 編譯
mkdir build && cd build
cmake ..
cmake --build .

# qmake 編譯
qmake6 last-report.pro
make
```

## 測試步驟

### 測試 YouTube 嵌入式播放
1. 執行應用程式
2. 複製 YouTube 影片連結（例如: https://www.youtube.com/watch?v=dQw4w9WgXcQ）
3. 貼到搜尋欄
4. 點擊「播放」按鈕
5. 確認影片在應用程式視窗內播放

### 測試拖放排序
1. 創建播放清單並加入多首歌曲
2. 用滑鼠拖動歌曲項目
3. 放到不同位置
4. 重新啟動應用程式，確認順序已儲存

### 測試 Whisper 轉錄
1. 安裝 Python 和 Whisper
2. 播放本地音樂檔案
3. 觀察字幕顯示區域
4. 確認轉錄文字即時出現

### 測試播放控制
1. 播放本地音樂
2. 測試暫停/播放按鈕
3. 測試上一首/下一首按鈕
4. 播放 YouTube 影片
5. 確認可以切換到下一首

## 已知限制

1. **YouTube 播放控制**: 由於 YouTube 嵌入式播放器的安全限制，無法從 Qt 直接控制影片的播放/暫停。使用者需要在影片播放器內點擊控制按鈕。

2. **Whisper 安裝**: Whisper 功能是選用的。如果未安裝 Python 或 Whisper，應用程式仍可正常運作，只是不會顯示字幕。

3. **網路需求**: YouTube 影片播放需要穩定的網路連線。

4. **效能**: Whisper 轉錄可能會佔用較多 CPU 資源，特別是在處理長音頻時。

## 未來改進建議

1. 加入音量控制滑桿
2. 加入播放進度條
3. 支援更多語言的 Whisper 轉錄
4. 改善 YouTube 播放控制（可能需要使用 JavaScript 注入）
5. 加入字幕樣式自訂功能
6. 支援字幕匯出功能

## 疑難排解

### 問題: 無法載入 YouTube 影片
**解決方案**: 
- 確認網路連線正常
- 確認已安裝 Qt WebEngine 模組
- 檢查防火牆設定

### 問題: Whisper 無法啟動
**解決方案**:
- 確認已安裝 Python 3.x
- 執行 `pip3 install openai-whisper`
- 確認已安裝 FFmpeg
- 檢查 whisper_transcribe.py 檔案是否存在

### 問題: 拖放排序不起作用
**解決方案**:
- 確認使用的是最新版本的程式碼
- 嘗試重新編譯應用程式
- 檢查是否有錯誤訊息

## 總結

所有要求的功能都已實現：
1. ✅ YouTube 連結直接在視窗內播放
2. ✅ 播放/暫停、上下一首按鈕可用
3. ✅ 播放清單支援拖放排序
4. ✅ 整合 Whisper 進行即時轉錄

應用程式現在提供完整的音樂播放體驗，包含線上和本地音樂播放、播放清單管理，以及選用的語音轉錄功能。
