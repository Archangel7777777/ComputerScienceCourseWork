#include "LibraryTab.hpp"
#include <QDebug>
#include <QGridLayout>

LibraryTab::LibraryTab(MainWidget *tabManager) :
    TabWidget(*tabManager, "Library")
{
    init(tabManager);
}
void LibraryTab::init(MainWidget *tabManager)
{
    dataHandler = new DatabaseHandler("Database.db");
    tableView = new QStackedWidget();
    playlistView = new QListWidget();

    tabManager->setFileManger(dataHandler);

    layout();
}
void LibraryTab::layout()
{
    QGridLayout *grid = new QGridLayout(this);
    PlaylistAdder *adder = new PlaylistAdder();
    PlayListView *playlistView = new PlayListView(dataHandler, tableView);

    connect(adder, SIGNAL(createPlaylist(QString)), dataHandler, SLOT(createPlaylist(QString)));
    connect(dataHandler, SIGNAL(playlistCreated()), playlistView, SLOT(extractFromDB()));

    grid->addWidget(playlistView, 0, 0);
    grid->addWidget(adder, 1, 0);
    grid->addWidget(tableView, 0, 1, 2, 1);
    grid->addWidget(new AudioPlayer(), 2, 0, -1, 0);
    grid->setColumnStretch(1, 2);
}

PlayListView::PlayListView(DatabaseHandler *dataHandler, QStackedWidget *tableView) :
    handler(dataHandler) ,
    stack(tableView)
{
    extractFromDB();

    this->viewport()->setAcceptDrops(true);
    this->setDragDropMode(QAbstractItemView::DragDrop);

    connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(changePlaylist(int)));
}

void PlayListView::extractFromDB()
{
    QList <std::shared_ptr<Playlist>> playlists = handler->getPlaylists();

    for (int i = 0; i < playlists.size(); ++i) 
    {
        if (stack->indexOf(playlists.at(i)->getView()) == -1)
        {
            this->addItem(playlists.at(i)->getName());
            stack->addWidget(playlists.at(i)->getView());
        }
    }
}
void PlayListView::changePlaylist(int index)
{
    stack->setCurrentIndex(index);
}
void PlayListView::dropEvent(QDropEvent *event)
{
    QString trackId(event->mimeData()->data("application/vnd.text.list"));
    QString tableName = this->itemAt(event->pos())->text();

    handler->addToPlaylist(tableName, trackId);
    int index = this->row(this->findItems(tableName, Qt::MatchFixedString).at(0));

    handler->getPlaylists().at(index)->update();

    event->accept();
    QListWidget::dropEvent(event);
}
QStringList PlayListView::mimeTypes() const
{
    return handler->getPlaylists().at(0)->mimeTypes();
}
QMimeData* PlayListView::mimeData(const QModelIndexList &index) const
{
    return handler->getPlaylists().at(0)->mimeData(index);
}

PlaylistAdder::PlaylistAdder()
{
    text = new QLineEdit();
    QPushButton *btn = new QPushButton("Add");

    connect(btn, SIGNAL(clicked()), this, SLOT(btnPressed()));
    QHBoxLayout *hbox = new QHBoxLayout(this);

    hbox->addWidget(btn);
    hbox->addWidget(text);
}
void PlaylistAdder::btnPressed()
{
    QString name = text->text();
    emit createPlaylist(name);
}
