#!/usr/bin/env python3
"""
Whisper 語音轉錄腳本
此腳本使用 OpenAI Whisper 模型將音頻文件轉錄為文字
並即時輸出轉錄結果以供 Qt 應用程式讀取
"""

import sys
import os

def transcribe_audio(audio_file_path):
    """
    轉錄音頻文件
    
    Args:
        audio_file_path: 音頻文件的完整路徑
    """
    # 檢查文件是否存在
    if not os.path.exists(audio_file_path):
        print(f"錯誤: 找不到音頻文件: {audio_file_path}", file=sys.stderr)
        return
    
    print(f"正在載入音頻文件: {audio_file_path}")
    sys.stdout.flush()
    
    try:
        # 嘗試導入 Whisper
        import whisper
        
        print("正在載入 Whisper 模型...")
        sys.stdout.flush()
        
        # 載入模型 (使用 base 模型以獲得速度和精度的平衡)
        # 可選模型: tiny, base, small, medium, large
        model = whisper.load_model("base")
        
        print("開始轉錄...")
        sys.stdout.flush()
        
        # 轉錄音頻
        result = model.transcribe(audio_file_path, language="zh")
        
        # 輸出完整轉錄文字
        print("\n=== 完整轉錄 ===")
        print(result["text"])
        sys.stdout.flush()
        
        # 輸出分段文字
        print("\n=== 分段轉錄 ===")
        for segment in result["segments"]:
            timestamp = f"[{segment['start']:.2f}s - {segment['end']:.2f}s]"
            text = segment['text'].strip()
            print(f"{timestamp} {text}")
            sys.stdout.flush()
        
        print("\n轉錄完成!")
        sys.stdout.flush()
        
    except ImportError:
        print("錯誤: 未安裝 Whisper 模組", file=sys.stderr)
        print("請使用以下命令安裝: pip install openai-whisper", file=sys.stderr)
        print("\n注意: Whisper 需要 ffmpeg，請確保已安裝:", file=sys.stderr)
        print("  - Ubuntu/Debian: sudo apt install ffmpeg", file=sys.stderr)
        print("  - macOS: brew install ffmpeg", file=sys.stderr)
        print("  - Windows: 從 https://ffmpeg.org 下載", file=sys.stderr)
        sys.stdout.flush()
    except Exception as e:
        print(f"轉錄時發生錯誤: {str(e)}", file=sys.stderr)
        sys.stdout.flush()

def main():
    """主函數"""
    if len(sys.argv) < 2:
        print("使用方法: python3 whisper_transcribe.py <音頻文件路徑>")
        print("\n支援的音頻格式: mp3, wav, flac, m4a, ogg, aac")
        sys.exit(1)
    
    audio_file_path = sys.argv[1]
    transcribe_audio(audio_file_path)

if __name__ == "__main__":
    main()
