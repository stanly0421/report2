# Whisper 語音轉錄整合指南

## 概述

此音樂播放器整合了 OpenAI Whisper 模型，可在播放本地音樂時即時進行語音轉錄，並將轉錄結果顯示在字幕區域。

## 安裝 Whisper

### 系統需求
- Python 3.8 或更高版本
- FFmpeg
- 至少 1GB RAM（根據模型大小而定）

### Ubuntu/Debian
```bash
# 安裝 Python 和 pip
sudo apt-get update
sudo apt-get install python3 python3-pip

# 安裝 FFmpeg
sudo apt-get install ffmpeg

# 安裝 Whisper
pip3 install openai-whisper
```

### macOS
```bash
# 使用 Homebrew
brew install python ffmpeg

# 安裝 Whisper
pip3 install openai-whisper
```

### Windows
1. 從 [python.org](https://www.python.org) 下載並安裝 Python
2. 從 [ffmpeg.org](https://ffmpeg.org) 下載並安裝 FFmpeg
3. 開啟命令提示字元，執行:
```bash
pip install openai-whisper
```

## 使用方法

### 基本使用
1. 確保已安裝 Whisper（見上方安裝說明）
2. 執行音樂播放器
3. 播放本地音樂檔案
4. 轉錄過程會自動開始
5. 轉錄文字會即時顯示在字幕區域

### 自訂 Whisper 腳本

預設的 `whisper_transcribe.py` 腳本使用 `base` 模型。你可以根據需求修改：

```python
# 可用的模型 (從小到大，速度從快到慢):
# - tiny    (最快，精度較低，~1GB RAM)
# - base    (平衡，~1GB RAM)
# - small   (較好，~2GB RAM)
# - medium  (很好，~5GB RAM)
# - large   (最佳，~10GB RAM)

model = whisper.load_model("base")  # 修改這裡
```

### 支援的語言

Whisper 支援多種語言。預設腳本設定為中文 (`zh`)：

```python
result = model.transcribe(audio_file_path, language="zh")
```

若要自動偵測語言，移除 `language` 參數：

```python
result = model.transcribe(audio_file_path)
```

支援的語言代碼：
- `zh`: 中文
- `en`: 英文
- `ja`: 日文
- `ko`: 韓文
- `es`: 西班牙文
- 等等...（完整列表見 Whisper 文件）

## 效能調整

### 選擇合適的模型

| 模型   | 記憶體需求 | 速度 | 精度 | 適用場景 |
|--------|-----------|------|------|---------|
| tiny   | ~1GB      | 最快 | 低   | 即時預覽 |
| base   | ~1GB      | 快   | 中   | 一般使用 |
| small  | ~2GB      | 中   | 高   | 高品質需求 |
| medium | ~5GB      | 慢   | 很高 | 專業用途 |
| large  | ~10GB     | 最慢 | 最高 | 最佳品質 |

### 加速轉錄

如果有 NVIDIA GPU，可以使用 CUDA 加速：

```bash
# 安裝 CUDA 版本
pip3 uninstall openai-whisper
pip3 install openai-whisper torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu118
```

## 疑難排解

### 問題: 顯示「錯誤: 未安裝 Whisper 模組」

**解決方案**:
```bash
pip3 install openai-whisper
```

### 問題: 顯示「需要 FFmpeg」

**解決方案**:
- Ubuntu/Debian: `sudo apt install ffmpeg`
- macOS: `brew install ffmpeg`
- Windows: 從 [ffmpeg.org](https://ffmpeg.org) 下載

### 問題: 轉錄速度很慢

**解決方案**:
1. 使用較小的模型（tiny 或 base）
2. 確保有足夠的 RAM
3. 考慮使用 GPU 加速（需要 NVIDIA GPU）

### 問題: 轉錄結果不準確

**解決方案**:
1. 使用較大的模型（medium 或 large）
2. 確保音頻品質良好
3. 設定正確的語言參數

### 問題: 找不到 whisper_transcribe.py

**解決方案**:
確保 `whisper_transcribe.py` 檔案與執行檔在同一目錄，或修改程式碼中的路徑。

## 進階功能

### 輸出格式自訂

修改 `whisper_transcribe.py` 以自訂輸出格式：

```python
# 只輸出純文字
print(result["text"])

# 包含時間戳記
for segment in result["segments"]:
    print(f"[{segment['start']:.2f}s] {segment['text']}")

# JSON 格式
import json
print(json.dumps(result, ensure_ascii=False))
```

### 儲存轉錄結果

在腳本中加入儲存功能：

```python
# 儲存為 TXT
with open("transcript.txt", "w", encoding="utf-8") as f:
    f.write(result["text"])

# 儲存為 SRT 字幕
def write_srt(segments, filename):
    with open(filename, "w", encoding="utf-8") as f:
        for i, segment in enumerate(segments, 1):
            start = format_timestamp(segment["start"])
            end = format_timestamp(segment["end"])
            text = segment["text"].strip()
            f.write(f"{i}\n{start} --> {end}\n{text}\n\n")

write_srt(result["segments"], "transcript.srt")
```

## 停用 Whisper

如果不需要語音轉錄功能：
1. 不安裝 Whisper（應用程式會自動跳過轉錄）
2. 或註解掉 `widget.cpp` 中的 `startWhisperTranscription()` 呼叫

應用程式在沒有 Whisper 的情況下仍可正常運作，只是不會顯示字幕。

## 資源連結

- [Whisper GitHub](https://github.com/openai/whisper)
- [Whisper 文件](https://github.com/openai/whisper#available-models-and-languages)
- [FFmpeg 官網](https://ffmpeg.org)
- [Python 官網](https://www.python.org)

## 授權與版權

Whisper 是 OpenAI 開發的開源專案，採用 MIT 授權。使用 Whisper 時請遵守其授權條款。
