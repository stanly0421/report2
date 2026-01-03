# 更新日誌 (Changelog)

## [v2.0] - 2026-01-03

### 新增功能 ✨
- **播放進度條**: 添加 QSlider 控制播放進度
  - 即時顯示當前播放位置和總時長（mm:ss 格式）
  - 可拖動進度條跳轉到指定位置
  - 拖動時即時更新時間顯示

- **音量控制**: 添加 QSlider 調整音量
  - 支援 0-100% 音量調整
  - 音量圖標根據音量大小自動切換（🔇/🔈/🔉/🔊）
  - 即時調整，無需確認

### 改進功能 🎯
- **智能最愛按鈕**: 根據當前播放清單自動切換按鈕文字
  - 在「我的最愛」播放清單中顯示「💔 移除最愛」
  - 在其他播放清單中顯示「❤️ 加入最愛」
  - 根據歌曲是否已在最愛中智能顯示

- **簡潔播放清單顯示**: 
  - 移除播放清單項目的編號（如 "1. ", "2. " 等）
  - 只顯示歌曲名稱和藝術家/頻道
  - 介面更加簡潔美觀

- **精簡音樂資訊顯示**:
  - 移除"檔案:"標籤
  - 只顯示歌曲標題，減少視覺雜訊

### 程式碼清理 🧹
- 移除不需要的文檔檔案：
  - CHANGES.md
  - CHANGES_SUMMARY.md
  - CHANGES_SUMMARY_FINAL.md
  - IMPLEMENTATION_NOTES.md
  - MIGRATION_NOTES.md
  - PROJECT_STATUS.txt
  - QUICK_REFERENCE.md
  - SOLUTION_SUMMARY.txt
  - SOLUTION_SUMMARY_NEW.md
  - TESTING_GUIDE.md
  - UI_DESIGN.md
  - USER_GUIDE.md
  - VERIFICATION.md
  - WHISPER_GUIDE.md

- 移除不需要的腳本檔案：
  - check_webengine.bat
  - check_webengine.sh

### 技術實現 🔧
- 新增成員變數：
  - `progressSlider`: QSlider* - 播放進度條
  - `currentTimeLabel`: QLabel* - 當前時間標籤
  - `totalTimeLabel`: QLabel* - 總時長標籤
  - `volumeSlider`: QSlider* - 音量控制滑桿
  - `volumeLabel`: QLabel* - 音量圖標標籤
  - `isProgressSliderPressed`: bool - 追蹤進度條拖動狀態

- 新增槽函數：
  - `onProgressSliderPressed()`: 處理進度條按下事件
  - `onProgressSliderReleased()`: 處理進度條釋放事件
  - `onProgressSliderMoved(int)`: 處理進度條拖動事件
  - `onVolumeSliderChanged(int)`: 處理音量變化事件

- 更新槽函數：
  - `onMediaPlayerPositionChanged()`: 更新進度條和時間顯示
  - `onMediaPlayerDurationChanged()`: 設置進度條範圍和總時長
  - `onPlaylistChanged()`: 更新最愛按鈕文字
  - `playVideo()`: 智能設置最愛按鈕文字
  - `toggleFavoriteForVideo()`: 根據播放清單上下文更新按鈕
  - `updatePlaylistDisplay()`: 移除編號顯示
  - `generateLocalMusicHTML()`: 移除檔案標籤
  - `updateLocalMusicDisplay()`: 簡化顯示內容

### 文檔更新 📚
- 更新 README.md：
  - 新增「最新改進（v2.0）」章節
  - 更新「功能特色」說明
  - 更新「操作說明」增加進度條和音量控制說明
  - 更新「界面預覽」描述新增的控制元件
  - 移除「未來改進」中已完成的項目

## [v1.0] - 之前版本

### 主要功能
- 本地音樂播放支援
- YouTube 影片連結支援（歷史兼容）
- 播放清單管理
- Whisper 語音轉錄與字幕功能
- 拖放排序功能
- 我的最愛功能
- Spotify 風格 UI 設計
