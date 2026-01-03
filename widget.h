#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QListWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>
#include <QTime>
#include <QUrl>
#include <QMap>
#include <QList>
#include <QSet>


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

// 播放清單結構
struct Playlist {
    QString name;              // 播放清單名稱
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    // 播放清單選擇
    void onNewPlaylistClicked();
    void onDeletePlaylistClicked();
    void onPlaylistChanged(int index);

private:
    void setupUI();
    void createConnections();
    void updatePlaylistDisplay();
    void savePlaylistsToFile();
    void loadPlaylistsFromFile();

    Ui::Widget *ui;
    
    // UI 元件
    QPushButton* newPlaylistButton;
    QPushButton* deletePlaylistButton;
    QListWidget* playlistWidget;
    QComboBox* playlistComboBox;
    
    // 播放清單數據
    QList<Playlist> playlists;
    int currentPlaylistIndex;
    QString lastPlaylistName;
};

#endif // WIDGET_H
