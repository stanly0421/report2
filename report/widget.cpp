#include "widget.h"
#include "ui_widget.h"
#include <QFileInfo>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    
    // Initialize media player and playlist
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    player->setPlaylist(playlist);
    
    // Set playlist to loop
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    
    // Set initial volume
    player->setVolume(50);
    
    // Connect playlist management buttons
    connect(ui->addButton, &QPushButton::clicked, this, &Widget::onAddButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &Widget::onRemoveButtonClicked);
    connect(ui->moveUpButton, &QPushButton::clicked, this, &Widget::onMoveUpButtonClicked);
    connect(ui->moveDownButton, &QPushButton::clicked, this, &Widget::onMoveDownButtonClicked);
    connect(ui->playlistWidget, &QListWidget::itemDoubleClicked, this, &Widget::onPlaylistItemDoubleClicked);
    
    // Connect player control buttons
    connect(ui->playButton, &QPushButton::clicked, this, &Widget::onPlayButtonClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &Widget::onPauseButtonClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &Widget::onStopButtonClicked);
    connect(ui->previousButton, &QPushButton::clicked, this, &Widget::onPreviousButtonClicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &Widget::onNextButtonClicked);
    
    // Connect sliders
    connect(ui->progressSlider, &QSlider::sliderMoved, this, &Widget::onProgressSliderMoved);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &Widget::onVolumeSliderMoved);
    
    // Connect player signals
    connect(player, &QMediaPlayer::positionChanged, this, &Widget::onPlayerPositionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &Widget::onPlayerDurationChanged);
    connect(playlist, &QMediaPlaylist::currentMediaChanged, this, &Widget::onCurrentMediaChanged);
    
    // Connect lyrics buttons
    connect(ui->loadLyricsButton, &QPushButton::clicked, this, &Widget::onLoadLyricsButtonClicked);
    connect(ui->clearLyricsButton, &QPushButton::clicked, this, &Widget::onClearLyricsButtonClicked);
    connect(ui->lyricsWidget, &QListWidget::itemClicked, this, &Widget::onLyricsItemClicked);
}

Widget::~Widget()
{
    delete ui;
}

// Playlist management slots
void Widget::onAddButtonClicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Select Music Files"),
        "",
        tr("Audio Files (*.mp3 *.wav *.flac *.aac *.ogg *.m4a *.wma);;All Files (*)")
    );
    
    for (const QString &file : files) {
        playlist->addMedia(QUrl::fromLocalFile(file));
        QFileInfo fileInfo(file);
        ui->playlistWidget->addItem(fileInfo.fileName());
    }
}

void Widget::onRemoveButtonClicked()
{
    int currentRow = ui->playlistWidget->currentRow();
    if (currentRow >= 0 && currentRow < playlist->mediaCount()) {
        playlist->removeMedia(currentRow);
        delete ui->playlistWidget->takeItem(currentRow);
    }
}

void Widget::onMoveUpButtonClicked()
{
    int currentRow = ui->playlistWidget->currentRow();
    if (currentRow > 0) {
        // Move in playlist
        QMediaContent media = playlist->media(currentRow);
        playlist->removeMedia(currentRow);
        playlist->insertMedia(currentRow - 1, media);
        
        // Move in list widget
        QListWidgetItem *item = ui->playlistWidget->takeItem(currentRow);
        ui->playlistWidget->insertItem(currentRow - 1, item);
        ui->playlistWidget->setCurrentRow(currentRow - 1);
    }
}

void Widget::onMoveDownButtonClicked()
{
    int currentRow = ui->playlistWidget->currentRow();
    if (currentRow >= 0 && currentRow < playlist->mediaCount() - 1) {
        // Move in playlist
        QMediaContent media = playlist->media(currentRow);
        playlist->removeMedia(currentRow);
        playlist->insertMedia(currentRow + 1, media);
        
        // Move in list widget
        QListWidgetItem *item = ui->playlistWidget->takeItem(currentRow);
        ui->playlistWidget->insertItem(currentRow + 1, item);
        ui->playlistWidget->setCurrentRow(currentRow + 1);
    }
}

void Widget::onPlaylistItemDoubleClicked(QListWidgetItem *item)
{
    int row = ui->playlistWidget->row(item);
    playlist->setCurrentIndex(row);
    player->play();
}

// Player control slots
void Widget::onPlayButtonClicked()
{
    player->play();
}

void Widget::onPauseButtonClicked()
{
    player->pause();
}

void Widget::onStopButtonClicked()
{
    player->stop();
    ui->currentTimeLabel->setText("00:00");
    ui->progressSlider->setValue(0);
}

void Widget::onPreviousButtonClicked()
{
    playlist->previous();
}

void Widget::onNextButtonClicked()
{
    playlist->next();
}

// Progress and volume slots
void Widget::onProgressSliderMoved(int position)
{
    player->setPosition(position);
}

void Widget::onVolumeSliderMoved(int value)
{
    player->setVolume(value);
    ui->volumeValueLabel->setText(QString::number(value));
}

// Player state change slots
void Widget::onPlayerPositionChanged(qint64 position)
{
    if (!ui->progressSlider->isSliderDown()) {
        ui->progressSlider->setValue(position);
    }
    ui->currentTimeLabel->setText(formatTime(position));
    
    // Update lyrics highlight
    updateCurrentLyrics(position);
}

void Widget::onPlayerDurationChanged(qint64 duration)
{
    ui->progressSlider->setRange(0, duration);
    ui->totalTimeLabel->setText(formatTime(duration));
}

void Widget::onCurrentMediaChanged(const QMediaContent &media)
{
    if (media.isNull()) {
        ui->currentSongLabel->setText("No song playing");
    } else {
        QString filename = QFileInfo(media.request().url().toLocalFile()).fileName();
        ui->currentSongLabel->setText("Now playing: " + filename);
        
        // Highlight current song in playlist
        int currentIndex = playlist->currentIndex();
        if (currentIndex >= 0) {
            ui->playlistWidget->setCurrentRow(currentIndex);
        }
    }
}

// Lyrics slots
void Widget::onLoadLyricsButtonClicked()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("Select Lyrics File"),
        "",
        tr("Lyrics Files (*.lrc *.txt);;All Files (*)")
    );
    
    if (!filename.isEmpty()) {
        loadLyricsFile(filename);
    }
}

void Widget::onClearLyricsButtonClicked()
{
    ui->lyricsWidget->clear();
    lyricsMap.clear();
    lyricsItemMap.clear();
}

void Widget::onLyricsItemClicked(QListWidgetItem *item)
{
    // Find the time associated with this lyrics item
    for (auto it = lyricsItemMap.begin(); it != lyricsItemMap.end(); ++it) {
        if (it.value() == item) {
            player->setPosition(it.key());
            break;
        }
    }
}

// Helper functions
QString Widget::formatTime(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void Widget::loadLyricsFile(const QString &filename)
{
    ui->lyricsWidget->clear();
    lyricsMap.clear();
    lyricsItemMap.clear();
    
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open lyrics file:" << filename;
        return;
    }
    
    QTextStream in(&file);
    // Try to detect encoding
    in.setCodec("UTF-8");
    
    // Regular expression to match LRC format: [mm:ss.xx] or [mm:ss]
    QRegularExpression timeRegex(R"(\[(\d{2}):(\d{2})(?:\.(\d{2}))?\])");
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        QRegularExpressionMatch match = timeRegex.match(line);
        if (match.hasMatch()) {
            // Extract time components
            int minutes = match.captured(1).toInt();
            int seconds = match.captured(2).toInt();
            int centiseconds = match.captured(3).isEmpty() ? 0 : match.captured(3).toInt();
            
            // Convert to milliseconds
            qint64 timeMs = (minutes * 60 + seconds) * 1000 + centiseconds * 10;
            
            // Extract lyrics text (remove all time tags)
            QString lyricsText = line;
            lyricsText.remove(timeRegex);
            lyricsText = lyricsText.trimmed();
            
            if (!lyricsText.isEmpty()) {
                lyricsMap[timeMs] = lyricsText;
            }
        } else if (!line.isEmpty() && !line.startsWith('[')) {
            // Plain text lyrics without time tags - add at end
            qint64 timeMs = lyricsMap.isEmpty() ? 0 : lyricsMap.lastKey() + 1000;
            lyricsMap[timeMs] = line;
        }
    }
    
    file.close();
    
    // Add lyrics to list widget
    for (auto it = lyricsMap.begin(); it != lyricsMap.end(); ++it) {
        QListWidgetItem *item = new QListWidgetItem(it.value());
        ui->lyricsWidget->addItem(item);
        lyricsItemMap[it.key()] = item;
    }
}

void Widget::updateCurrentLyrics(qint64 position)
{
    if (lyricsMap.isEmpty()) {
        return;
    }
    
    // Find the lyrics line that should be highlighted
    qint64 currentTime = -1;
    for (auto it = lyricsMap.begin(); it != lyricsMap.end(); ++it) {
        if (it.key() <= position) {
            currentTime = it.key();
        } else {
            break;
        }
    }
    
    // Highlight the current lyrics line
    for (auto it = lyricsItemMap.begin(); it != lyricsItemMap.end(); ++it) {
        QListWidgetItem *item = it.value();
        if (it.key() == currentTime) {
            item->setBackground(QBrush(QColor(173, 216, 230))); // Light blue
            ui->lyricsWidget->scrollToItem(item);
        } else {
            item->setBackground(QBrush()); // Use default background
        }
    }
}
