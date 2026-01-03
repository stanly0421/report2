# Testing Guide / 測試指南

## Prerequisites / 前置需求

### Required Software / 必要軟體
- Qt 5.12+ or Qt 6.0+ with Qt Creator
- C++17 compatible compiler
- Qt Multimedia module

### Test Files Needed / 需要的測試檔案
1. **Audio Files / 音訊檔案**:
   - Sample MP3 file
   - Sample WAV file
   - Sample FLAC file (optional)
   - Sample AAC file (optional)

2. **Lyrics File / 歌詞檔案**:
   - Use provided `sample_lyrics.lrc`
   - Or create your own LRC format file

## Build Instructions / 編譯說明

### Method 1: Qt Creator
1. Open Qt Creator
2. File → Open File or Project
3. Navigate to `report/report.pro`
4. Select a Kit (Qt5 or Qt6)
5. Click "Configure Project"
6. Build → Build Project "report"
7. Run → Run

### Method 2: Command Line / 命令列
```bash
cd report
qmake report.pro
make
./report  # Linux/Mac
# or
report.exe  # Windows
```

## Test Cases / 測試案例

### TC-01: Basic Playback / 基本播放
**Steps / 步驟:**
1. Click "Add Songs" button
2. Select one or more audio files
3. Verify files appear in playlist
4. Click "Play" button
5. Verify music plays

**Expected Result / 預期結果:**
- ✓ Songs added to playlist
- ✓ Music starts playing
- ✓ Progress bar moves
- ✓ Time display updates (00:00 → xx:xx)

### TC-02: Playlist Management / 播放清單管理
**Steps / 步驟:**
1. Add 3+ songs to playlist
2. Select a song in the middle
3. Click "Move Up" button
4. Verify song moved up one position
5. Click "Move Down" button twice
6. Verify song moved down
7. Click "Remove" button
8. Verify song removed from list

**Expected Result / 預期結果:**
- ✓ Songs reorder correctly
- ✓ Songs can be removed
- ✓ Playlist count updates

### TC-03: Playback Controls / 播放控制
**Steps / 步驟:**
1. Add and play a song
2. Wait 5 seconds
3. Click "Pause" button
4. Verify playback paused
5. Click "Play" button
6. Verify playback resumes from same position
7. Click "Stop" button
8. Verify playback stops and resets to 00:00

**Expected Result / 預期結果:**
- ✓ Pause works correctly
- ✓ Resume continues from pause point
- ✓ Stop resets position

### TC-04: Next/Previous Navigation / 上下首切換
**Steps / 步驟:**
1. Add 3 songs to playlist
2. Play first song
3. Click "Next" button
4. Verify second song plays
5. Click "Previous" button
6. Verify first song plays again
7. Click "Previous" again
8. Verify last song plays (loop behavior)

**Expected Result / 預期結果:**
- ✓ Next button advances to next song
- ✓ Previous button goes to previous song
- ✓ Looping works at boundaries

### TC-05: Progress Slider / 進度條
**Steps / 步驟:**
1. Play a song
2. Wait for playback to start
3. Drag progress slider to middle
4. Verify playback jumps to that position
5. Verify time display updates correctly

**Expected Result / 預期結果:**
- ✓ Slider moves during playback
- ✓ Dragging slider seeks correctly
- ✓ Time display accurate

### TC-06: Volume Control / 音量控制
**Steps / 步驟:**
1. Play a song
2. Adjust volume slider to 25
3. Verify volume changes and display shows "25"
4. Adjust to 75
5. Verify volume increases and display shows "75"
6. Adjust to 0
7. Verify audio muted

**Expected Result / 預期結果:**
- ✓ Volume changes with slider
- ✓ Display shows correct value
- ✓ Volume = 0 mutes audio

### TC-07: Lyrics Loading / 歌詞載入
**Steps / 步驟:**
1. Click "Load Lyrics" button
2. Select `sample_lyrics.lrc` file
3. Verify lyrics appear in right panel
4. Verify all lyrics lines visible

**Expected Result / 預期結果:**
- ✓ Lyrics file loads successfully
- ✓ All lines displayed in list
- ✓ No parsing errors

### TC-08: Lyrics Synchronization / 歌詞同步
**Steps / 步驟:**
1. Load lyrics file
2. Play corresponding song
3. Watch lyrics panel during playback
4. Verify current line highlights (blue background)
5. Verify auto-scroll to current line

**Expected Result / 預期結果:**
- ✓ Lyrics highlight at correct time
- ✓ Highlight color: light blue
- ✓ Auto-scroll works
- ✓ Timing is accurate

### TC-09: Click-to-Seek Lyrics / 點擊歌詞跳轉
**Steps / 步驟:**
1. Load lyrics and play song
2. Click on a lyrics line in the middle
3. Verify playback jumps to that time
4. Verify that line becomes highlighted
5. Click on first line
6. Verify playback jumps to beginning

**Expected Result / 預期結果:**
- ✓ Clicking jumps to correct time
- ✓ Highlight updates immediately
- ✓ Works for all lyrics lines

### TC-10: Multi-Format Support / 多格式支援
**Steps / 步驟:**
1. Add MP3 file → verify it plays
2. Add WAV file → verify it plays
3. Add FLAC file → verify it plays
4. Add AAC file → verify it plays

**Expected Result / 預期結果:**
- ✓ All supported formats play correctly
- ✓ No format-specific issues

### TC-11: Double-Click Playlist / 雙擊播放清單
**Steps / 步驟:**
1. Add 3+ songs
2. Double-click on third song
3. Verify third song starts playing
4. Verify playlist highlights third song

**Expected Result / 預期結果:**
- ✓ Double-click plays selected song
- ✓ Visual feedback correct

### TC-12: Auto-Advance / 自動播放下一首
**Steps / 步驟:**
1. Add 2 short songs
2. Play first song
3. Wait for song to finish
4. Verify second song automatically starts

**Expected Result / 預期結果:**
- ✓ Automatically plays next song
- ✓ No gap between songs
- ✓ Display updates correctly

### TC-13: Playlist Loop / 播放清單循環
**Steps / 步驟:**
1. Add 2 songs
2. Play second song
3. Click "Next" button
4. Verify first song plays (loop to beginning)

**Expected Result / 預期結果:**
- ✓ Loops from last to first
- ✓ Continuous playback

### TC-14: Clear Lyrics / 清除歌詞
**Steps / 步驟:**
1. Load lyrics file
2. Click "Clear Lyrics" button
3. Verify lyrics panel is empty

**Expected Result / 預期結果:**
- ✓ All lyrics removed
- ✓ Panel is empty

### TC-15: Current Song Display / 當前歌曲顯示
**Steps / 步驟:**
1. Add songs with different names
2. Play each song
3. Verify "Now playing: [filename]" displays correctly
4. Stop playback
5. Verify display shows "No song playing"

**Expected Result / 預期結果:**
- ✓ Correct filename displayed
- ✓ Updates on song change
- ✓ Correct message when stopped

## Performance Tests / 效能測試

### PT-01: Large Playlist
**Steps:**
1. Add 100+ songs
2. Verify no lag in UI
3. Test reordering
4. Test playback

**Expected:**
- ✓ UI remains responsive
- ✓ Operations complete quickly

### PT-02: Long Lyrics File
**Steps:**
1. Load lyrics file with 100+ lines
2. Play and verify synchronization
3. Test scroll performance

**Expected:**
- ✓ Scrolling smooth
- ✓ No performance degradation

## Compatibility Tests / 相容性測試

### CT-01: Qt5 Build
**Steps:**
1. Build with Qt 5.12+
2. Run all test cases
3. Verify all features work

### CT-02: Qt6 Build
**Steps:**
1. Build with Qt 6.0+
2. Run all test cases
3. Verify all features work

## Bug Reporting / 錯誤回報

If you find any issues, please report with:
- Qt version
- Operating system
- Steps to reproduce
- Expected vs actual behavior
- Any error messages

## Test Results Template / 測試結果範本

```
Test Date: ____________________
Qt Version: ___________________
OS: ___________________________

Test Case | Result | Notes
----------|--------|-------
TC-01     | PASS   |
TC-02     | PASS   |
TC-03     | PASS   |
...       | ...    | ...

Overall Status: PASS / FAIL
Comments: _____________________
```

## Known Limitations / 已知限制

1. Only LRC format lyrics supported
2. No playlist save/load functionality
3. No equalizer
4. No visualization
5. No album art display

## Future Test Cases / 未來測試案例

When new features are added:
- [ ] Playlist save/load
- [ ] Random playback mode
- [ ] Repeat one mode
- [ ] Equalizer settings
- [ ] Theme customization

---

**Testing completed successfully indicates the implementation is production-ready.**
**測試全部通過表示實作已達到生產環境標準。**
