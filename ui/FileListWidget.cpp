#include "FileListWidget.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QSet>

FileListWidget::FileListWidget(QWidget* parent) : QListWidget(parent) {
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setMinimumHeight(220);
}

QList<QString> FileListWidget::currentPaths() const {
    QList<QString> out;
    out.reserve(count());
    for (int i = 0; i < count(); ++i) {
        out.append(item(i)->data(Qt::UserRole).toString());
    }
    return out;
}

void FileListWidget::addPaths(const QList<QString>& paths) {
    QSet<QString> existing;
    for (int i = 0; i < count(); ++i)
        existing.insert(item(i)->data(Qt::UserRole).toString());

    for (const QString& raw : paths) {
        QFileInfo fi(raw);
        if (!fi.exists() || !fi.isFile()) continue;
        const QString abs = fi.absoluteFilePath();

        if (existing.contains(abs)) continue;
        existing.insert(abs);

        auto* it = new QListWidgetItem(abs, this);
        it->setToolTip(abs);
        it->setData(Qt::UserRole, abs);
    }
}

void FileListWidget::dragEnterEvent(QDragEnterEvent* e) {
    if (e->mimeData()->hasUrls()) e->acceptProposedAction();
    else QListWidget::dragEnterEvent(e);
}

void FileListWidget::dragMoveEvent(QDragMoveEvent* e) {
    if (e->mimeData()->hasUrls()) e->acceptProposedAction();
    else QListWidget::dragMoveEvent(e);
}

void FileListWidget::dropEvent(QDropEvent* e) {
    if (e->mimeData()->hasUrls()) {
        QList<QString> paths;
        for (const QUrl& u : e->mimeData()->urls())
            paths.append(u.toLocalFile());
        addPaths(paths);
        e->acceptProposedAction();
    } else QListWidget::dropEvent(e);
}