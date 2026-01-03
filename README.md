# Music Player Application

A full-featured Qt-based music player with playlist management, lyrics display, and comprehensive playback controls.

## Features

### 1. Playlist Management
- **Add Songs**: Add multiple audio files to the playlist using the "Add Songs" button
- **Remove Songs**: Remove selected songs from the playlist
- **Reorder Songs**: Move songs up or down in the playlist to adjust playback order
- **Double-click to Play**: Double-click any song in the playlist to start playing it

### 2. Playback Navigation
- **Previous Button**: Skip to the previous song in the playlist
- **Next Button**: Skip to the next song in the playlist
- **Automatic Progression**: Songs automatically advance when finished

### 3. Playback Progress
- **Progress Slider**: Visual representation of current playback position
- **Time Display**: Shows current time (mm:ss) and total duration (mm:ss)
- **Seek Functionality**: Drag the slider to jump to any position in the song

### 4. Player Controls
- **Play**: Start or resume playback
- **Pause**: Pause the current song
- **Stop**: Stop playback and reset position
- **Volume Control**: Adjust volume from 0-100 using the volume slider

### 5. Playlist Features
- Supports looping through the entire playlist
- Visual indication of currently playing song
- Display of song filenames in the playlist

### 6. Audio Format Support
The player supports multiple audio formats including:
- MP3
- WAV
- FLAC
- AAC
- OGG
- M4A
- WMA

### 7. Lyrics Display
- **Load Lyrics**: Load LRC format lyrics files
- **Synchronized Display**: Lyrics automatically highlight as the song plays
- **Click to Seek**: Click any lyrics line to jump to that position in the song
- **Auto-scroll**: Lyrics automatically scroll to show the current line
- **Clear Lyrics**: Remove loaded lyrics from display

## Lyrics File Format

The application supports LRC format lyrics files. Example:

```
[00:00.00]Song Title
[00:05.50]First line of lyrics
[00:10.20]Second line of lyrics
[00:15.80]Third line of lyrics
```

Time format: [mm:ss.xx] where:
- mm = minutes (2 digits)
- ss = seconds (2 digits)
- xx = centiseconds (2 digits, optional)

## Usage

1. **Adding Music**:
   - Click "Add Songs" button
   - Select one or more audio files
   - Songs will appear in the playlist

2. **Managing Playlist**:
   - Select a song and click "Remove" to delete it
   - Select a song and use "Move Up"/"Move Down" to reorder
   - Double-click any song to start playing it

3. **Playing Music**:
   - Click "Play" to start playback
   - Use "Pause" to temporarily stop
   - Use "Stop" to stop and reset
   - Use "Previous"/"Next" to navigate songs
   - Drag the progress slider to seek

4. **Loading Lyrics**:
   - Click "Load Lyrics" button
   - Select an LRC format lyrics file
   - Lyrics will display and synchronize with playback
   - Click any lyrics line to jump to that timestamp

## Technical Details

- Built with Qt 5/6
- Uses QMediaPlayer for audio playback
- Uses QMediaPlaylist for playlist management
- Implements QListWidget for playlist and lyrics display
- Supports UTF-8 encoded lyrics files

## Building

```bash
qmake report.pro
make
```

## Requirements

- Qt 5.x or Qt 6.x
- Qt Multimedia module
- Qt Widgets module
```
