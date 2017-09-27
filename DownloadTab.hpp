#ifndef DOWNLOADTAB_H
#define DOWNLOADTAB_H

#include "Window.hpp"
#include "PlayerWindow.hpp"
#include "VideoYoutube.hpp"

#include <map>

#include <QtWebKitWidgets/QWebView>
#include <QListWidget>
#include <QWebElementCollection>

class SearchList : public QListWidget
{
    Q_OBJECT;

public:
    SearchList();
    void searchYoutube(const QString text);
    std::map<QString, QString> urlMap;
private slots:
    void requestReceived(QNetworkReply *reply);
private:
    QNetworkAccessManager *networkManger;
    QWebElementCollection collection;

    void networkSetup();
};

class DownloadWidget : public TabWidget
{
    Q_OBJECT;

public:
    DownloadWidget(MainWidget *tabManger);

private slots:
    void search();
    void itemSelectionChanged();
    void getVideo();

private:
    QPushButton *searchBtn;
    QLineEdit *searchBar;
    PlayerWindow *preview;
    QNetworkAccessManager *manager;
    QWebElementCollection collection;
    VideoYoutube *currentVideo;

    SearchList *videoList;

    void init();
    void btns();
    void textBars();
    void layout();
    void setPreview();
};

#endif
