# Architecture Overview

## Component Diagram

```
┌──────────────────────────────────────────────────────────────┐
│                         Widget (Main Window)                  │
├──────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌─────────────────────┐        ┌────────────────────────┐  │
│  │  Playlist Section   │        │   Lyrics Section       │  │
│  ├─────────────────────┤        ├────────────────────────┤  │
│  │ QListWidget         │        │ QListWidget            │  │
│  │ - Song list display │        │ - Lyrics display       │  │
│  │                     │        │ - Click to seek        │  │
│  │ [Add Songs]         │        │ [Load Lyrics]          │  │
│  │ [Remove]            │        │ [Clear Lyrics]         │  │
│  │ [Move Up/Down]      │        │                        │  │
│  └─────────────────────┘        └────────────────────────┘  │
│                                                               │
│  ┌──────────────────────────────────────────────────────┐   │
│  │            Current Song Display                       │   │
│  │            "Now playing: song.mp3"                    │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                               │
│  ┌──────────────────────────────────────────────────────┐   │
│  │   [00:00] ═══════════●════════════ [03:45]           │   │
│  │           Progress Slider                             │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                               │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  [Previous] [Play] [Pause] [Stop] [Next]             │   │
│  │  Volume: ════●════ [50]                               │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                               │
├──────────────────────────────────────────────────────────────┤
│                    Backend Components                         │
├──────────────────────────────────────────────────────────────┤
│                                                               │
│  QMediaPlayer ◄──────► QMediaPlaylist                        │
│  - Playback engine     - Song queue                          │
│  - Volume control      - Current index                       │
│  - Position tracking   - Loop mode                           │
│                                                               │
│  LyricsMap                                                    │
│  - Time → Text mapping                                       │
│  - Synchronized display                                      │
│                                                               │
└──────────────────────────────────────────────────────────────┘
```

## Data Flow

### 1. Adding Songs
```
User clicks "Add Songs"
    ↓
QFileDialog opens
    ↓
User selects files
    ↓
Files added to QMediaPlaylist
    ↓
Filenames added to QListWidget
```

### 2. Playback
```
User clicks Play/Double-clicks song
    ↓
QMediaPlayer starts playback
    ↓
positionChanged signal emitted
    ↓
Update progress slider
    ↓
Update time labels
    ↓
Highlight current lyrics
```

### 3. Lyrics Synchronization
```
Load LRC file
    ↓
Parse time tags [mm:ss.xx]
    ↓
Store in QMap<time, text>
    ↓
Display in QListWidget
    ↓
During playback:
    - Find current lyrics line
    - Highlight with blue background
    - Auto-scroll to visible
```

### 4. Click-to-Seek
```
User clicks lyrics line
    ↓
Find associated timestamp
    ↓
Call player->setPosition(timestamp)
    ↓
Playback jumps to position
    ↓
Lyrics highlight updates
```

## Signal-Slot Connections

### Player Signals
- `player->positionChanged(qint64)` → Update progress & lyrics
- `player->durationChanged(qint64)` → Update total time
- `playlist->currentMediaChanged()` → Update current song display

### UI Signals
- Button clicks → Control functions
- Slider moved → Seek/Volume change
- List item clicked → Play song / Seek to lyrics

## Key Features Implementation

### Playlist Management
- **Add**: QFileDialog → playlist->addMedia()
- **Remove**: playlist->removeMedia(index)
- **Reorder**: removeMedia() + insertMedia()

### Time Display
- Format: `QString("%1:%2").arg(mm).arg(ss)`
- Updates every position change (~1 second)

### Lyrics Parser
- Regex: `\[(\d{2}):(\d{2})(?:\.(\d{2}))?\]`
- Converts to milliseconds for precise sync

### Volume Control
- Range: 0-100
- Real-time display of value
- Directly controls player->setVolume()

## File Structure

```
report2/
├── .gitignore                  # Ignore build artifacts
├── README.md                   # English documentation
├── IMPLEMENTATION.md           # Chinese documentation
├── ARCHITECTURE.md             # This file
├── sample_lyrics.lrc          # Example lyrics
└── report/
    ├── report.pro             # Qt project (with multimedia)
    ├── main.cpp               # Entry point
    ├── widget.h               # Class declaration
    ├── widget.cpp             # Implementation (~320 lines)
    └── widget.ui              # UI layout
```

## Dependencies

- Qt Core (QString, QMap, QList, etc.)
- Qt Widgets (QWidget, QPushButton, QSlider, etc.)
- Qt Multimedia (QMediaPlayer, QMediaPlaylist)
- Standard C++ (for logic and algorithms)

## Testing Checklist

- [ ] Add songs from file dialog
- [ ] Remove songs from playlist
- [ ] Reorder songs (move up/down)
- [ ] Double-click to play song
- [ ] Play/Pause/Stop controls
- [ ] Previous/Next navigation
- [ ] Volume adjustment
- [ ] Progress slider seeking
- [ ] Time display accuracy
- [ ] Load LRC lyrics file
- [ ] Lyrics synchronization
- [ ] Click lyrics to seek
- [ ] Auto-scroll lyrics
- [ ] Multiple audio formats
- [ ] Playlist looping
