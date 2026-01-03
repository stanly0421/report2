#include "widget.h"
#include "ui_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QSplitter>
#include <QRegularExpression>
#include <QTextBrowser>
#include <QProcess>
#include <QDesktopServices>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , mediaPlayer(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput(this))
    , videoDisplayArea(nullptr)
    , whisperProcess(new QProcess(this))
    , subtitleDisplay(nullptr)
    , currentPlaylistIndex(-1)
    , currentVideoIndex(-1)
    , isShuffleMode(false)
    , isRepeatMode(false)
    , isPlaying(false)
{
    ui->setupUi(this);
    
    // 設置媒體播放器
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
    
    // 設置窗口
    setWindowTitle("音樂播放器");
    setMinimumSize(1000, 700);
    
    // 建立UI
    setupUI();
    
    // 建立信號連接
    createConnections();
    
    // 加載保存的播放清單
    loadPlaylistsFromFile();
    
    // 如果沒有播放清單，創建默認播放清單
    if (playlists.isEmpty()) {
        Playlist defaultPlaylist;
        defaultPlaylist.name = "我的播放清單";
        playlists.append(defaultPlaylist);
        
        Playlist favoritesPlaylist;
        favoritesPlaylist.name = "我的最愛";
        playlists.append(favoritesPlaylist);
        
        playlistComboBox->addItem(defaultPlaylist.name);
        playlistComboBox->addItem(favoritesPlaylist.name);
        currentPlaylistIndex = 0;
    } else {
        // 恢復播放清單到ComboBox
        for (const Playlist& playlist : playlists) {
            playlistComboBox->addItem(playlist.name);
        }
        
        // 恢復上次的播放清單
        int lastIndex = 0;
        for (int i = 0; i < playlists.size(); i++) {
            if (playlists[i].name == lastPlaylistName) {
                lastIndex = i;
                break;
            }
        }
        playlistComboBox->setCurrentIndex(lastIndex);
        currentPlaylistIndex = lastIndex;
        updatePlaylistDisplay();
    }
    
    // 更新按鈕狀態
    updateButtonStates();
}

Widget::~Widget()
{
    // 保存播放清單
    savePlaylistsToFile();
    delete ui;
}

void Widget::setupUI()
{
    // 主佈局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 設置深色主題
    setStyleSheet(
        "QWidget {"
        "   background-color: #121212;"
        "   color: #FFFFFF;"
        "}"
        "QLineEdit {"
        "   background-color: #282828;"
        "   border: 1px solid #404040;"
        "   border-radius: 20px;"
        "   padding: 8px 16px;"
        "   color: #FFFFFF;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #1DB954;"
        "}"
        "QListWidget {"
        "   background-color: #181818;"
        "   border: none;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   padding: 10px;"
        "   border-bottom: 1px solid #282828;"
        "   color: #B3B3B3;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #282828;"
        "   color: #FFFFFF;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #1DB954;"
        "   color: #FFFFFF;"
        "}"
        "QComboBox {"
        "   background-color: #282828;"
        "   border: 1px solid #404040;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "   color: #FFFFFF;"
        "   min-width: 150px;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #282828;"
        "   color: #FFFFFF;"
        "   selection-background-color: #1DB954;"
        "}"
    );
    
    // === 頂部搜尋欄 ===
    QWidget* topBar = new QWidget(this);
    topBar->setStyleSheet("background-color: #000000; padding: 16px;");
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    
    QLabel* logoLabel = new QLabel("🎵 音樂播放器", topBar);
    logoLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #1DB954;");
    topLayout->addWidget(logoLabel);
    
    topLayout->addStretch();
    
    searchEdit = new QLineEdit(topBar);
    searchEdit->setPlaceholderText("貼上 YouTube 連結...");
    searchEdit->setMinimumWidth(400);
    topLayout->addWidget(searchEdit);
    
    searchButton = new QPushButton("▶ 播放", topBar);
    searchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #1DB954;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 8px 24px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #1ED760; }"
        "QPushButton:pressed { background-color: #1AA34A; }"
    );
    topLayout->addWidget(searchButton);
    
    loadLocalFileButton = new QPushButton("📁 本地音樂", topBar);
    loadLocalFileButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 8px 24px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #404040; }"
        "QPushButton:pressed { background-color: #505050; }"
    );
    topLayout->addWidget(loadLocalFileButton);
    
    mainLayout->addWidget(topBar);
    
    // === 內容區域 ===
    QSplitter* contentSplitter = new QSplitter(Qt::Horizontal, this);
    contentSplitter->setStyleSheet("QSplitter::handle { background-color: #282828; }");
    
    // === 左側面板：播放清單 ===
    QWidget* leftPanel = new QWidget(contentSplitter);
    leftPanel->setStyleSheet("background-color: #000000;");
    leftPanel->setMinimumWidth(250);
    leftPanel->setMaximumWidth(350);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(16, 16, 16, 16);
    leftLayout->setSpacing(12);
    
    QLabel* playlistLabel = new QLabel("播放清單", leftPanel);
    playlistLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF; margin-bottom: 8px;");
    leftLayout->addWidget(playlistLabel);
    
    playlistComboBox = new QComboBox(leftPanel);
    leftLayout->addWidget(playlistComboBox);
    
    QHBoxLayout* playlistButtonLayout = new QHBoxLayout();
    
    newPlaylistButton = new QPushButton("➕ 新增", leftPanel);
    newPlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
    );
    playlistButtonLayout->addWidget(newPlaylistButton);
    
    deletePlaylistButton = new QPushButton("🗑️ 刪除", leftPanel);
    deletePlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
    );
    playlistButtonLayout->addWidget(deletePlaylistButton);
    
    leftLayout->addLayout(playlistButtonLayout);
    
    playlistWidget = new QListWidget(leftPanel);
    playlistWidget->setDragDropMode(QAbstractItemView::InternalMove);
    playlistWidget->setDefaultDropAction(Qt::MoveAction);
    leftLayout->addWidget(playlistWidget);
    
    contentSplitter->addWidget(leftPanel);
    
    // === 中央面板：影片播放器和搜尋結果 ===
    QWidget* centerPanel = new QWidget(contentSplitter);
    centerPanel->setStyleSheet("background-color: #121212;");
    QVBoxLayout* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(16, 16, 16, 16);
    centerLayout->setSpacing(16);
    
    // 影片資訊
    videoTitleLabel = new QLabel("選擇一首歌曲開始播放", centerPanel);
    videoTitleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    videoTitleLabel->setWordWrap(true);
    centerLayout->addWidget(videoTitleLabel);
    
    channelLabel = new QLabel("", centerPanel);
    channelLabel->setStyleSheet("font-size: 14px; color: #B3B3B3;");
    centerLayout->addWidget(channelLabel);
    
    // 影片顯示區域 - 使用 QTextBrowser 支援 HTML 顯示和連結點擊
    videoDisplayArea = new QTextBrowser(centerPanel);
    videoDisplayArea->setMinimumHeight(400);
    videoDisplayArea->setOpenExternalLinks(true);  // 允許點擊連結在瀏覽器中開啟
    videoDisplayArea->setStyleSheet(
        "QTextBrowser {"
        "   background-color: #000000;"
        "   border-radius: 8px;"
        "   color: #FFFFFF;"
        "   font-size: 16px;"
        "   padding: 20px;"
        "}"
        "QTextBrowser a {"
        "   color: #1DB954;"
        "   text-decoration: none;"
        "}"
        "QTextBrowser a:hover {"
        "   color: #1ED760;"
        "   text-decoration: underline;"
        "}"
    );
    videoDisplayArea->setHtml(
        "<div style='text-align: center; padding-top: 100px;'>"
        "<h2 style='color: #1DB954; font-size: 32px;'>🎵 音樂播放器</h2>"
        "<p style='color: #B3B3B3; font-size: 18px; margin-top: 20px;'>選擇一首歌曲開始播放</p>"
        "</div>"
    );
    centerLayout->addWidget(videoDisplayArea, 1);
    
    // 字幕顯示區域
    subtitleDisplay = new QTextEdit(centerPanel);
    subtitleDisplay->setReadOnly(true);
    subtitleDisplay->setMaximumHeight(100);
    subtitleDisplay->setStyleSheet(
        "QTextEdit {"
        "   background-color: #181818;"
        "   color: #FFFFFF;"
        "   font-size: 14px;"
        "   padding: 10px;"
        "   border-radius: 8px;"
        "   border: 1px solid #282828;"
        "}"
    );
    subtitleDisplay->setPlaceholderText("字幕將在播放時顯示...");
    centerLayout->addWidget(subtitleDisplay);
    
    // 播放控制區域
    QWidget* controlWidget = new QWidget(centerPanel);
    controlWidget->setStyleSheet("background-color: #181818; border-radius: 8px; padding: 16px;");
    QHBoxLayout* controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setSpacing(12);
    
    QString buttonStyle = 
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #FFFFFF;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   min-width: 40px;"
        "}"
        "QPushButton:hover { background-color: #404040; }"
        "QPushButton:pressed { background-color: #505050; }"
        "QPushButton:disabled { background-color: #181818; color: #404040; }";
    
    shuffleButton = new QPushButton("🔀", controlWidget);
    shuffleButton->setStyleSheet(buttonStyle);
    shuffleButton->setCheckable(true);
    shuffleButton->setToolTip("隨機播放");
    controlLayout->addWidget(shuffleButton);
    
    previousButton = new QPushButton("⏮", controlWidget);
    previousButton->setStyleSheet(buttonStyle);
    previousButton->setToolTip("上一首");
    controlLayout->addWidget(previousButton);
    
    playPauseButton = new QPushButton("▶", controlWidget);
    playPauseButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #1DB954;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 25px;"
        "   padding: 12px;"
        "   font-size: 18px;"
        "   min-width: 50px;"
        "   min-height: 50px;"
        "}"
        "QPushButton:hover { background-color: #1ED760; }"
        "QPushButton:pressed { background-color: #1AA34A; }"
        "QPushButton:disabled { background-color: #282828; color: #404040; }"
    );
    controlLayout->addWidget(playPauseButton);
    
    nextButton = new QPushButton("⏭", controlWidget);
    nextButton->setStyleSheet(buttonStyle);
    nextButton->setToolTip("下一首");
    controlLayout->addWidget(nextButton);
    
    repeatButton = new QPushButton("🔁", controlWidget);
    repeatButton->setStyleSheet(buttonStyle);
    repeatButton->setCheckable(true);
    repeatButton->setToolTip("循環播放");
    controlLayout->addWidget(repeatButton);
    
    controlLayout->addStretch();
    
    toggleFavoriteButton = new QPushButton("❤️ 加入最愛", controlWidget);
    toggleFavoriteButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 10px 20px;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
        "QPushButton:disabled { background-color: #181818; color: #404040; }"
    );
    toggleFavoriteButton->setEnabled(false);
    controlLayout->addWidget(toggleFavoriteButton);
    
    centerLayout->addWidget(controlWidget);
    
    contentSplitter->addWidget(centerPanel);
    
    // 設置分割器比例
    contentSplitter->setStretchFactor(0, 1);
    contentSplitter->setStretchFactor(1, 3);
    
    mainLayout->addWidget(contentSplitter, 1);
}

void Widget::createConnections()
{
    // 搜尋功能
    connect(searchButton, &QPushButton::clicked, this, &Widget::onSearchClicked);
    connect(searchEdit, &QLineEdit::returnPressed, this, &Widget::onSearchClicked);
    connect(loadLocalFileButton, &QPushButton::clicked, this, &Widget::onLoadLocalFileClicked);
    
    // 播放控制按鈕
    connect(playPauseButton, &QPushButton::clicked, this, &Widget::onPlayPauseClicked);
    connect(previousButton, &QPushButton::clicked, this, &Widget::onPreviousClicked);
    connect(nextButton, &QPushButton::clicked, this, &Widget::onNextClicked);
    connect(shuffleButton, &QPushButton::clicked, this, &Widget::onShuffleClicked);
    connect(repeatButton, &QPushButton::clicked, this, &Widget::onRepeatClicked);
    
    // 播放清單管理
    connect(playlistWidget, &QListWidget::itemDoubleClicked, this, &Widget::onVideoDoubleClicked);
    connect(playlistWidget, &QListWidget::itemSelectionChanged, this, &Widget::updateButtonStates);
    
    // 最愛按鈕
    connect(toggleFavoriteButton, &QPushButton::clicked, this, &Widget::onToggleFavoriteClicked);
    
    // 播放清單選擇
    connect(newPlaylistButton, &QPushButton::clicked, this, &Widget::onNewPlaylistClicked);
    connect(deletePlaylistButton, &QPushButton::clicked, this, &Widget::onDeletePlaylistClicked);
    connect(playlistComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Widget::onPlaylistChanged);
    
    // 媒體播放器
    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &Widget::onMediaPlayerStateChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &Widget::onMediaPlayerPositionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &Widget::onMediaPlayerDurationChanged);
    
    // Whisper 轉錄
    connect(whisperProcess, &QProcess::readyReadStandardOutput, this, &Widget::onWhisperOutputReady);
    connect(whisperProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), 
            this, &Widget::onWhisperFinished);
    
    // 播放清單拖放重排
    connect(playlistWidget->model(), &QAbstractItemModel::rowsMoved, 
            [this](const QModelIndex &, int, int, const QModelIndex &, int) {
                // 當項目被移動時，更新內部資料結構
                if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
                    Playlist& playlist = playlists[currentPlaylistIndex];
                    QList<VideoInfo> newVideos;
                    for (int i = 0; i < playlistWidget->count(); i++) {
                        QListWidgetItem* item = playlistWidget->item(i);
                        int oldIndex = item->data(Qt::UserRole).toInt();
                        if (oldIndex >= 0 && oldIndex < playlist.videos.size()) {
                            newVideos.append(playlist.videos[oldIndex]);
                        }
                    }
                    playlist.videos = newVideos;
                    // 重新分配索引
                    for (int i = 0; i < playlistWidget->count(); i++) {
                        playlistWidget->item(i)->setData(Qt::UserRole, i);
                    }
                    savePlaylistsToFile();
                }
            });
}

void Widget::onSearchClicked()
{
    QString link = searchEdit->text().trimmed();
    if (link.isEmpty()) {
        QMessageBox::warning(this, "播放", "請貼上 YouTube 連結！");
        return;
    }
    
    playYouTubeLink(link);
}

void Widget::onLoadLocalFileClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, 
        "選擇音樂檔案", 
        QDir::homePath(),
        "音樂檔案 (*.mp3 *.wav *.flac *.m4a *.ogg *.aac);;所有檔案 (*.*)");
    
    if (!filePath.isEmpty()) {
        playLocalFile(filePath);
    }
}

QString Widget::extractYouTubeVideoId(const QString& url)
{
    // 支援多種 YouTube URL 格式
    // https://www.youtube.com/watch?v=VIDEO_ID
    // https://youtu.be/VIDEO_ID
    // https://www.youtube.com/embed/VIDEO_ID
    
    QString videoId;
    
    if (url.contains("youtube.com/watch")) {
        QRegularExpression rx("v=([a-zA-Z0-9_-]+)");
        QRegularExpressionMatch match = rx.match(url);
        if (match.hasMatch()) {
            videoId = match.captured(1);
        }
    } else if (url.contains("youtu.be/")) {
        QRegularExpression rx("youtu\\.be/([a-zA-Z0-9_-]+)");
        QRegularExpressionMatch match = rx.match(url);
        if (match.hasMatch()) {
            videoId = match.captured(1);
        }
    } else if (url.contains("youtube.com/embed/")) {
        QRegularExpression rx("embed/([a-zA-Z0-9_-]+)");
        QRegularExpressionMatch match = rx.match(url);
        if (match.hasMatch()) {
            videoId = match.captured(1);
        }
    }
    
    return videoId;
}

void Widget::playYouTubeLink(const QString& link)
{
    QString videoId = extractYouTubeVideoId(link);
    
    if (videoId.isEmpty()) {
        QMessageBox::warning(this, "錯誤", "無法識別 YouTube 連結格式！\n\n支援的格式：\n- https://www.youtube.com/watch?v=VIDEO_ID\n- https://youtu.be/VIDEO_ID\n- https://www.youtube.com/embed/VIDEO_ID");
        return;
    }
    
    // 停止當前播放
    mediaPlayer->stop();
    
    // 創建影片資訊
    VideoInfo video;
    video.videoId = videoId;
    video.title = "YouTube 影片";
    video.channelTitle = "正在播放 YouTube 影片";
    video.isFavorite = false;
    video.isLocalFile = false;
    video.filePath = "";
    
    // 使用 QTextBrowser 顯示 YouTube 影片連結
    videoDisplayArea->setHtml(generateYouTubeDisplayHTML(video.title, video.channelTitle, videoId));
    
    // 顯示影片資訊
    updateVideoLabels(video);
    
    // 更新狀態
    isPlaying = true;
    playPauseButton->setText("⏸");
    currentVideoIndex = -1;  // 不屬於播放清單
    
    updateButtonStates();
    
    // 清空字幕顯示
    subtitleDisplay->clear();
}

void Widget::playLocalFile(const QString& filePath)
{
    // 停止當前播放
    mediaPlayer->stop();
    
    // 創建影片資訊
    VideoInfo video;
    video.filePath = filePath;
    video.videoId = "";
    
    // 從檔案名提取標題
    QFileInfo fileInfo(filePath);
    video.title = fileInfo.baseName();
    video.channelTitle = "本地音樂";
    video.isFavorite = false;
    video.isLocalFile = true;
    
    // 設置媒體播放器
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    mediaPlayer->play();
    
    // 在 WebView 中顯示本地音樂資訊
    QFileInfo fileInfo(filePath);
    videoDisplayArea->setHtml(generateLocalMusicHTML(video.title, fileInfo.fileName()));
    
    updateVideoLabels(video);
    
    // 更新播放狀態
    isPlaying = true;
    playPauseButton->setText("⏸");
    currentVideoIndex = -1;  // 不屬於播放清單
    
    updateButtonStates();
    
    // 啟動 Whisper 轉錄
    startWhisperTranscription(filePath);
}

void Widget::onPlayPauseClicked()
{
    if (currentVideoIndex >= 0) {
        // 有正在播放的影片
        if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
            const Playlist& playlist = playlists[currentPlaylistIndex];
            if (currentVideoIndex < playlist.videos.size()) {
                const VideoInfo& video = playlist.videos[currentVideoIndex];
                
                if (video.isLocalFile) {
                    // 本地檔案，控制媒體播放器
                    if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
                        mediaPlayer->pause();
                        isPlaying = false;
                        playPauseButton->setText("▶");
                    } else {
                        mediaPlayer->play();
                        isPlaying = true;
                        playPauseButton->setText("⏸");
                    }
                } else {
                    // YouTube 影片，無法直接控制播放
                    // 顯示提示訊息
                    isPlaying = !isPlaying;
                    playPauseButton->setText(isPlaying ? "⏸" : "▶");
                    QMessageBox::information(this, "提示", 
                        "YouTube 影片播放需要在瀏覽器中操作。\n請點擊顯示區域的連結在瀏覽器中播放。");
                }
            }
        }
    } else {
        // 沒有影片，播放播放清單第一首
        if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
            Playlist& playlist = playlists[currentPlaylistIndex];
            if (!playlist.videos.isEmpty()) {
                playVideo(0);
            }
        }
    }
}

void Widget::onMediaPlayerStateChanged()
{
    // 當媒體播放器狀態改變時更新按鈕
    if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        isPlaying = true;
        playPauseButton->setText("⏸");
    } else if (mediaPlayer->playbackState() == QMediaPlayer::StoppedState) {
        isPlaying = false;
        playPauseButton->setText("▶");
        
        // 本地檔案播放結束，自動播放下一首（如果有）
        // 只有當前正在播放本地檔案時才自動播放下一首
        if (currentVideoIndex >= 0 && currentPlaylistIndex >= 0 && 
            currentPlaylistIndex < playlists.size()) {
            const Playlist& playlist = playlists[currentPlaylistIndex];
            if (currentVideoIndex < playlist.videos.size() &&
                playlist.videos[currentVideoIndex].isLocalFile) {
                int nextIndex = getNextVideoIndex();
                if (nextIndex >= 0) {
                    playVideo(nextIndex);
                }
            }
        }
    }
}

void Widget::onMediaPlayerPositionChanged(qint64 position)
{
    // 可以在這裡更新進度條（如果需要的話）
}

void Widget::onMediaPlayerDurationChanged(qint64 duration)
{
    // 可以在這裡設置進度條的最大值（如果需要的話）
}

void Widget::onPreviousClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.videos.isEmpty()) return;
    
    if (isShuffleMode) {
        int newIndex = getRandomVideoIndex(true);
        if (newIndex >= 0) {
            playVideo(newIndex);
        }
    } else {
        int newIndex = currentVideoIndex - 1;
        if (newIndex < 0) {
            newIndex = playlist.videos.size() - 1;
        }
        playVideo(newIndex);
    }
}

void Widget::onNextClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.videos.isEmpty()) return;
    
    int newIndex = getNextVideoIndex();
    if (newIndex >= 0) {
        playVideo(newIndex);
    }
}

void Widget::onShuffleClicked()
{
    isShuffleMode = !isShuffleMode;
    shuffleButton->setChecked(isShuffleMode);
    
    if (isShuffleMode) {
        playedVideosInCurrentSession.clear();
        shuffleButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #1DB954;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #1ED760; }"
        );
    } else {
        shuffleButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #282828;"
            "   color: #FFFFFF;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #404040; }"
        );
    }
}

void Widget::onRepeatClicked()
{
    isRepeatMode = !isRepeatMode;
    repeatButton->setChecked(isRepeatMode);
    
    if (isRepeatMode) {
        repeatButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #1DB954;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #1ED760; }"
        );
    } else {
        repeatButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #282828;"
            "   color: #FFFFFF;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #404040; }"
        );
    }
}

void Widget::onVideoDoubleClicked(QListWidgetItem* item)
{
    int index = playlistWidget->row(item);
    playVideo(index);
}

void Widget::onToggleFavoriteClicked()
{
    if (currentVideoIndex < 0 || currentPlaylistIndex < 0) return;
    if (currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& currentPlaylist = playlists[currentPlaylistIndex];
    if (currentVideoIndex >= currentPlaylist.videos.size()) return;
    
    VideoInfo& video = currentPlaylist.videos[currentVideoIndex];
    
    // 找到 "我的最愛" 播放清單
    int favoritesIndex = -1;
    for (int i = 0; i < playlists.size(); i++) {
        if (playlists[i].name == "我的最愛") {
            favoritesIndex = i;
            break;
        }
    }
    
    if (favoritesIndex < 0) {
        // 創建 "我的最愛" 播放清單
        Playlist favoritesPlaylist;
        favoritesPlaylist.name = "我的最愛";
        playlists.append(favoritesPlaylist);
        playlistComboBox->addItem(favoritesPlaylist.name);
        favoritesIndex = playlists.size() - 1;
    }
    
    Playlist& favoritesPlaylist = playlists[favoritesIndex];
    
    // 檢查是否已在最愛中
    bool isInFavorites = false;
    int favoriteIndex = -1;
    for (int i = 0; i < favoritesPlaylist.videos.size(); i++) {
        if (favoritesPlaylist.videos[i].videoId == video.videoId) {
            isInFavorites = true;
            favoriteIndex = i;
            break;
        }
    }
    
    if (isInFavorites) {
        // 從最愛移除
        favoritesPlaylist.videos.removeAt(favoriteIndex);
        video.isFavorite = false;
        toggleFavoriteButton->setText("❤️ 加入最愛");
        QMessageBox::information(this, "我的最愛", "已從最愛中移除！");
    } else {
        // 加入最愛
        VideoInfo favoriteVideo = video;
        favoriteVideo.isFavorite = true;
        favoritesPlaylist.videos.append(favoriteVideo);
        video.isFavorite = true;
        toggleFavoriteButton->setText("💔 移除最愛");
        QMessageBox::information(this, "我的最愛", "已加入最愛！");
    }
    
    updatePlaylistDisplay();
}

void Widget::onNewPlaylistClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增播放清單", 
                                         "請輸入播放清單名稱:", 
                                         QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        // 檢查是否重複
        for (const Playlist& p : playlists) {
            if (p.name == name) {
                QMessageBox::warning(this, "新增播放清單", "播放清單名稱已存在！");
                return;
            }
        }
        
        Playlist newPlaylist;
        newPlaylist.name = name;
        playlists.append(newPlaylist);
        playlistComboBox->addItem(name);
        
        int newIndex = playlists.size() - 1;
        playlistComboBox->setCurrentIndex(newIndex);
        currentPlaylistIndex = newIndex;
        lastPlaylistName = name;
        updatePlaylistDisplay();
        updateButtonStates();
    }
}

void Widget::onDeletePlaylistClicked()
{
    if (playlists.size() <= 1) {
        QMessageBox::warning(this, "無法刪除", "至少需要保留一個播放清單！");
        return;
    }
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    int ret = QMessageBox::question(this, "確認刪除", 
                                    QString("確定要刪除播放清單「%1」嗎？")
                                    .arg(playlists[currentPlaylistIndex].name),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        videoDisplayArea->setHtml(
            "<div style='text-align: center; padding-top: 100px;'>"
            "<h2 style='color: #1DB954; font-size: 32px;'>🎵 音樂播放器</h2>"
            "<p style='color: #B3B3B3; font-size: 18px; margin-top: 20px;'>選擇一首歌曲開始播放</p>"
            "</div>"
        );
        currentVideoIndex = -1;
        isPlaying = false;
        playlists.removeAt(currentPlaylistIndex);
        playlistComboBox->removeItem(currentPlaylistIndex);
    }
}

void Widget::onPlaylistChanged(int index)
{
    if (index < 0 || index >= playlists.size()) return;
    
    currentPlaylistIndex = index;
    currentVideoIndex = -1;
    playedVideosInCurrentSession.clear();
    updatePlaylistDisplay();
    updateButtonStates();
}

void Widget::updatePlaylistDisplay()
{
    playlistWidget->clear();
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    const Playlist& playlist = playlists[currentPlaylistIndex];
    for (int i = 0; i < playlist.videos.size(); i++) {
        const VideoInfo& video = playlist.videos[i];
        QString displayText = QString("%1. %2\n   %3")
                                .arg(i + 1)
                                .arg(video.title)
                                .arg(video.channelTitle);
        
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, i);
        
        // 高亮當前播放的影片
        if (i == currentVideoIndex) {
            item->setBackground(QColor("#1DB954"));
            item->setForeground(QColor("#FFFFFF"));
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
        }
        
        playlistWidget->addItem(item);
    }
}

void Widget::playVideo(int index)
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (index < 0 || index >= playlist.videos.size()) return;
    
    currentVideoIndex = index;
    const VideoInfo& video = playlist.videos[index];
    
    playedVideosInCurrentSession.insert(index);
    
    // 停止當前播放
    mediaPlayer->stop();
    
    if (video.isLocalFile) {
        // 播放本地檔案
        mediaPlayer->setSource(QUrl::fromLocalFile(video.filePath));
        mediaPlayer->play();
        
        QFileInfo fileInfo(video.filePath);
        videoDisplayArea->setHtml(generateLocalMusicHTML(video.title, fileInfo.fileName()));
        
        isPlaying = true;
        playPauseButton->setText("⏸");
        
        // 啟動 Whisper 轉錄
        startWhisperTranscription(video.filePath);
    } else {
        // 播放 YouTube 影片 - 顯示連結供用戶在瀏覽器中播放
        videoDisplayArea->setHtml(generateYouTubeDisplayHTML(video.title, video.channelTitle, video.videoId));
        isPlaying = true;
        playPauseButton->setText("⏸");
        
        // 清空字幕顯示
        subtitleDisplay->clear();
    }
    
    // 更新顯示
    updateVideoLabels(video);
    
    // 更新最愛按鈕
    if (video.isFavorite) {
        toggleFavoriteButton->setText("💔 移除最愛");
    } else {
        toggleFavoriteButton->setText("❤️ 加入最愛");
    }
    
    updatePlaylistDisplay();
    updateButtonStates();
    
    playlistWidget->setCurrentRow(index);
}

void Widget::updateButtonStates()
{
    bool hasPlaylist = (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size());
    bool hasVideos = hasPlaylist && !playlists[currentPlaylistIndex].videos.isEmpty();
    int selectedRow = playlistWidget->currentRow();
    bool hasSelection = selectedRow >= 0;
    bool hasMediaPlaying = currentVideoIndex >= 0;
    
    playPauseButton->setEnabled(hasVideos || hasMediaPlaying);
    previousButton->setEnabled(hasVideos);
    nextButton->setEnabled(hasVideos);
    deletePlaylistButton->setEnabled(playlists.size() > 1);
    toggleFavoriteButton->setEnabled(hasMediaPlaying);
}

void Widget::savePlaylistsToFile()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }
    
    QString configFile = configDir + "/youtube_playlists.json";
    
    QJsonObject rootObj;
    QJsonArray playlistsArray;
    
    for (const Playlist& playlist : playlists) {
        QJsonObject playlistObj;
        playlistObj["name"] = playlist.name;
        
        QJsonArray videosArray;
        for (const VideoInfo& video : playlist.videos) {
            QJsonObject videoObj;
            videoObj["videoId"] = video.videoId;
            videoObj["filePath"] = video.filePath;
            videoObj["title"] = video.title;
            videoObj["channelTitle"] = video.channelTitle;
            videoObj["thumbnailUrl"] = video.thumbnailUrl;
            videoObj["description"] = video.description;
            videoObj["isFavorite"] = video.isFavorite;
            videoObj["isLocalFile"] = video.isLocalFile;
            videosArray.append(videoObj);
        }
        playlistObj["videos"] = videosArray;
        playlistsArray.append(playlistObj);
    }
    
    rootObj["playlists"] = playlistsArray;
    if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
        rootObj["lastPlaylist"] = playlists[currentPlaylistIndex].name;
    }
    
    QJsonDocument doc(rootObj);
    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void Widget::loadPlaylistsFromFile()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString configFile = configDir + "/youtube_playlists.json";
    
    QFile file(configFile);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return;
    }
    
    QJsonObject rootObj = doc.object();
    lastPlaylistName = rootObj["lastPlaylist"].toString();
    
    QJsonArray playlistsArray = rootObj["playlists"].toArray();
    playlists.clear();
    
    for (const QJsonValue& value : playlistsArray) {
        QJsonObject playlistObj = value.toObject();
        Playlist playlist;
        playlist.name = playlistObj["name"].toString();
        
        QJsonArray videosArray = playlistObj["videos"].toArray();
        for (const QJsonValue& videoValue : videosArray) {
            QJsonObject videoObj = videoValue.toObject();
            VideoInfo video;
            video.videoId = videoObj["videoId"].toString();
            video.filePath = videoObj["filePath"].toString();
            video.title = videoObj["title"].toString();
            video.channelTitle = videoObj["channelTitle"].toString();
            video.thumbnailUrl = videoObj["thumbnailUrl"].toString();
            video.description = videoObj["description"].toString();
            video.isFavorite = videoObj["isFavorite"].toBool();
            video.isLocalFile = videoObj["isLocalFile"].toBool();
            
            playlist.videos.append(video);
        }
        playlists.append(playlist);
    }
}

int Widget::getNextVideoIndex()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return -1;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.videos.isEmpty()) return -1;
    
    if (isShuffleMode) {
        return getRandomVideoIndex(true);
    } else {
        int newIndex = currentVideoIndex + 1;
        if (newIndex >= playlist.videos.size()) {
            if (isRepeatMode) {
                return 0;
            } else {
                return -1;
            }
        }
        return newIndex;
    }
}

int Widget::getRandomVideoIndex(bool excludeCurrent)
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return -1;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.videos.isEmpty()) return -1;
    
    if (playlist.videos.size() == 1) {
        if (excludeCurrent && currentVideoIndex == 0) {
            return -1;
        }
        return 0;
    }
    
    QList<int> unplayedVideos = getUnplayedVideoIndices(excludeCurrent);
    
    if (unplayedVideos.isEmpty() && isRepeatMode) {
        playedVideosInCurrentSession.clear();
        unplayedVideos = getUnplayedVideoIndices(excludeCurrent);
    }
    
    if (unplayedVideos.isEmpty()) {
        return -1;
    }
    
    int randomIndex = QRandomGenerator::global()->bounded(unplayedVideos.size());
    return unplayedVideos[randomIndex];
}

QList<int> Widget::getUnplayedVideoIndices(bool excludeCurrent)
{
    QList<int> unplayedVideos;
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) {
        return unplayedVideos;
    }
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    
    for (int i = 0; i < playlist.videos.size(); i++) {
        if (!playedVideosInCurrentSession.contains(i)) {
            if (!excludeCurrent || i != currentVideoIndex) {
                unplayedVideos.append(i);
            }
        }
    }
    
    return unplayedVideos;
}

QString Widget::generateYouTubeDisplayHTML(const QString& title, const QString& channel, const QString& videoId)
{
    QString watchUrl = QString("https://www.youtube.com/watch?v=%1").arg(videoId);
    return QString(
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<style>"
        "body { background-color: #000000; color: #FFFFFF; font-family: Arial, sans-serif; text-align: center; padding: 50px; }"
        "h2 { color: #1DB954; font-size: 32px; margin-bottom: 20px; }"
        "p { font-size: 18px; margin: 20px 0; color: #B3B3B3; }"
        "a { color: #1DB954; text-decoration: none; font-size: 20px; font-weight: bold; }"
        "a:hover { color: #1ED760; text-decoration: underline; }"
        ".info { font-size: 14px; color: #888; margin: 30px 0; }"
        "</style>"
        "</head>"
        "<body>"
        "<h2>🎵 %1</h2>"
        "<p>%2</p>"
        "<div style='margin: 40px 0;'>"
        "<a href='%3'>🔗 點擊此處在瀏覽器中播放</a>"
        "</div>"
        "<p class='info'>由於不使用 WebEngine，YouTube 影片將在瀏覽器中播放</p>"
        "</body>"
        "</html>"
    ).arg(title.toHtmlEscaped())
     .arg(channel.toHtmlEscaped())
     .arg(watchUrl);
}

QString Widget::generateLocalMusicHTML(const QString& title, const QString& fileName)
{
    return QString(
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<style>"
        "body { background-color: #000000; color: #FFFFFF; font-family: Arial, sans-serif; text-align: center; padding: 50px; }"
        "h2 { color: #1DB954; font-size: 32px; margin-bottom: 20px; }"
        "p { font-size: 18px; margin: 20px 0; color: #B3B3B3; }"
        ".filename { font-size: 14px; color: #888; margin: 10px 0; }"
        "</style>"
        "</head>"
        "<body>"
        "<h2>🎵 本地音樂</h2>"
        "<p>%1</p>"
        "<p class='filename'>檔案: %2</p>"
        "<p style='color: #666; font-size: 12px; margin-top: 30px;'>正在播放本地音樂檔案</p>"
        "</body>"
        "</html>"
    ).arg(title.toHtmlEscaped())
     .arg(fileName.toHtmlEscaped());
}

void Widget::updateVideoLabels(const VideoInfo& video)
{
    videoTitleLabel->setText(video.title);
    channelLabel->setText(video.channelTitle);
}

QString Widget::createVideoDisplayHTML(const VideoInfo& video)
{
    QString watchUrl = QString("https://www.youtube.com/watch?v=%1").arg(video.videoId);
    QString escapedTitle = video.title.toHtmlEscaped();
    QString escapedChannel = video.channelTitle.toHtmlEscaped();
    
    return QString(
        "<div style='text-align: center;'>"
        "<h2 style='color: #1DB954;'>🎵 YouTube 影片</h2>"
        "<p style='font-size: 18px; margin: 20px 0;'>%1</p>"
        "<p style='font-size: 14px; color: #888; margin: 10px 0;'>頻道: %2</p>"
        "<p style='margin: 30px 0;'><a href='%3' style='color: #1DB954; text-decoration: none; font-size: 16px;'>🔗 在瀏覽器中播放</a></p>"
        "<p style='color: #666; font-size: 12px;'>點擊上方連結在您的瀏覽器中觀看此影片</p>"
        "</div>"
    ).arg(escapedTitle).arg(escapedChannel).arg(watchUrl);
}

void Widget::startWhisperTranscription(const QString& audioFilePath)
{
    // 停止現有的 Whisper 處理程序
    if (whisperProcess->state() != QProcess::NotRunning) {
        whisperProcess->kill();
        whisperProcess->waitForFinished();
    }
    
    // 清空字幕顯示
    subtitleDisplay->clear();
    subtitleDisplay->append("正在啟動語音轉錄...");
    
    // 檢查 Whisper 腳本是否存在
    QString whisperScript = "whisper_transcribe.py";
    QFileInfo scriptInfo(whisperScript);
    
    if (!scriptInfo.exists()) {
        subtitleDisplay->append("注意: whisper_transcribe.py 腳本不存在");
        subtitleDisplay->append("請確保已安裝 Whisper 並創建轉錄腳本");
        return;
    }
    
    // 啟動 Whisper 處理程序
    QStringList arguments;
    arguments << whisperScript << audioFilePath;
    
    whisperProcess->start("python3", arguments);
    
    if (!whisperProcess->waitForStarted(3000)) {
        subtitleDisplay->append("錯誤: 無法啟動 Whisper 處理程序");
        subtitleDisplay->append("請確保已安裝 Python 和 Whisper");
    }
}

void Widget::onWhisperOutputReady()
{
    // 讀取 Whisper 的標準輸出
    QByteArray output = whisperProcess->readAllStandardOutput();
    QString text = QString::fromUtf8(output).trimmed();
    
    if (!text.isEmpty()) {
        // 將新的轉錄文字附加到字幕顯示
        subtitleDisplay->append(text);
        
        // 自動滾動到底部
        QTextCursor cursor = subtitleDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        subtitleDisplay->setTextCursor(cursor);
    }
}

void Widget::onWhisperFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        subtitleDisplay->append("\n[轉錄處理程序異常終止]");
    } else if (exitCode != 0) {
        subtitleDisplay->append(QString("\n[轉錄處理程序結束，退出碼: %1]").arg(exitCode));
        
        // 讀取錯誤輸出
        QByteArray errorOutput = whisperProcess->readAllStandardError();
        if (!errorOutput.isEmpty()) {
            subtitleDisplay->append("錯誤信息: " + QString::fromUtf8(errorOutput));
        }
    } else {
        subtitleDisplay->append("\n[轉錄完成]");
    }
}
