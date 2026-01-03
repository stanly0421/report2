# 播放清單管理器 (Playlist Manager)

這是一個使用 Qt6 C++ 開發的簡單播放清單管理器。

## 功能特色

### 1. 播放清單管理
- 創建和管理多個播放清單
- 支援刪除播放清單
- 自動保存播放清單狀態

## 操作說明

### 播放清單管理
- **新增播放清單**: 創建新的播放清單
- **刪除播放清單**: 刪除當前播放清單（至少保留一個）

## 編譯和運行

### 系統需求
- Qt 6.x 或更高版本
- C++17 編譯器

### 安裝依賴 (Ubuntu/Debian)
```bash
sudo apt-get install qt6-base-dev
```

### 編譯
```bash
qmake6 last-report.pro
make
```

### 運行
```bash
./last-report
```

## 技術實現

### 核心類別
- **QListWidget**: 顯示播放清單
- **QComboBox**: 播放清單選擇器

### 資料持久化
- 使用簡單的文本格式儲存播放清單
- 儲存位置：`QStandardPaths::AppDataLocation`
- 自動載入上次的播放清單

## 授權聲明

本專案為開源專案。

祝您使用愉快！