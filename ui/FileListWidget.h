#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QListWidget>

class FileListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit FileListWidget(QWidget* parent = nullptr);

    QList<QString> currentPaths() const;
    void addPaths(const QList<QString>& paths);

protected:
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dropEvent(QDropEvent* e) override;
};

#endif