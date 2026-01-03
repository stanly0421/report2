#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMap>
#include <QPair>
#include <QUrl>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QMediaPlaylist>
#include <QMediaContent>
#else
#include <QAudioOutput>
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    // Playlist management
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onMoveUpButtonClicked();
    void onMoveDownButtonClicked();
    void onPlaylistItemDoubleClicked(QListWidgetItem *item);
    
    // Player controls
    void onPlayButtonClicked();
    void onPauseButtonClicked();
    void onStopButtonClicked();
    void onPreviousButtonClicked();
    void onNextButtonClicked();
    
    // Progress and volume
    void onProgressSliderMoved(int position);
    void onVolumeSliderMoved(int value);
    
    // Player state changes
    void onPlayerPositionChanged(qint64 position);
    void onPlayerDurationChanged(qint64 duration);
    
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void onCurrentMediaChanged(const QMediaContent &media);
#else
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
#endif
    
    // Lyrics
    void onLoadLyricsButtonClicked();
    void onClearLyricsButtonClicked();
    void onLyricsItemClicked(QListWidgetItem *item);

private:
    Ui::Widget *ui;
    QMediaPlayer *player;
    
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QMediaPlaylist *playlist;
#else
    QAudioOutput *audioOutput;
    QList<QUrl> playlist;
    int currentIndex;
#endif
    
    // Lyrics data: time in milliseconds -> lyrics text
    QMap<qint64, QString> lyricsMap;
    QMap<qint64, QListWidgetItem*> lyricsItemMap;
    
    // Constants for lyrics parsing
    static const QRegularExpression lyricsTimeRegex;
    
    // Helper functions
    QString formatTime(qint64 milliseconds);
    void loadLyricsFile(const QString &filename);
    void updateCurrentLyrics(qint64 position);
    void updatePlaylistDisplay();
    void playCurrentIndex();
    void updateCurrentSongDisplay();
};

#endif // WIDGET_H
