#include "widget.h"
#include "ui_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , currentPlaylistIndex(-1)
{
    ui->setupUi(this);
    
    // 設置窗口
    setWindowTitle("播放清單管理器");
    setMinimumSize(600, 400);
    
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
        
        playlistComboBox->addItem(defaultPlaylist.name);
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
    
    // 標題
    QLabel* titleLabel = new QLabel("播放清單管理器", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);
    
    // 播放清單選擇
    QHBoxLayout* playlistSelectLayout = new QHBoxLayout();
    QLabel* playlistLabel = new QLabel("播放清單:", this);
    playlistSelectLayout->addWidget(playlistLabel);
    
    playlistComboBox = new QComboBox(this);
    playlistSelectLayout->addWidget(playlistComboBox);
    
    mainLayout->addLayout(playlistSelectLayout);
    
    // 播放清單管理按鈕
    QHBoxLayout* playlistButtonLayout = new QHBoxLayout();
    
    newPlaylistButton = new QPushButton("新增播放清單", this);
    playlistButtonLayout->addWidget(newPlaylistButton);
    
    deletePlaylistButton = new QPushButton("刪除播放清單", this);
    playlistButtonLayout->addWidget(deletePlaylistButton);
    
    playlistButtonLayout->addStretch();
    mainLayout->addLayout(playlistButtonLayout);
    
    // 播放清單項目列表
    playlistWidget = new QListWidget(this);
    mainLayout->addWidget(playlistWidget);
}

void Widget::createConnections()
{
    // 播放清單選擇
    connect(newPlaylistButton, &QPushButton::clicked, this, &Widget::onNewPlaylistClicked);
    connect(deletePlaylistButton, &QPushButton::clicked, this, &Widget::onDeletePlaylistClicked);
    connect(playlistComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Widget::onPlaylistChanged);
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
        playlists.removeAt(currentPlaylistIndex);
        playlistComboBox->removeItem(currentPlaylistIndex);
    }
}

void Widget::onPlaylistChanged(int index)
{
    if (index < 0 || index >= playlists.size()) return;
    
    currentPlaylistIndex = index;
    updatePlaylistDisplay();
}

void Widget::updatePlaylistDisplay()
{
    playlistWidget->clear();
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    const Playlist& playlist = playlists[currentPlaylistIndex];
    QListWidgetItem* item = new QListWidgetItem(QString("播放清單: %1").arg(playlist.name));
    playlistWidget->addItem(item);
}

void Widget::savePlaylistsToFile()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }
    
    QString configFile = configDir + "/playlists.json";
    
    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << "{\n";
        out << "  \"playlists\": [\n";
        for (int i = 0; i < playlists.size(); i++) {
            out << "    {\"name\": \"" << playlists[i].name << "\"}";
            if (i < playlists.size() - 1) out << ",";
            out << "\n";
        }
        out << "  ],\n";
        if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
            out << "  \"lastPlaylist\": \"" << playlists[currentPlaylistIndex].name << "\"\n";
        }
        out << "}\n";
        file.close();
    }
}

void Widget::loadPlaylistsFromFile()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString configFile = configDir + "/playlists.json";
    
    QFile file(configFile);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    file.close();
}
