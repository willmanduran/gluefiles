#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include "../core/Glue.h"

class QFileSystemModel;
class QTreeView;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QProgressBar;
class QLabel;
class FileListWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onBuild();
    void onAddSelected();
    void onClearByExt();
    void updateProgress(int current, int total);
    void handleFinished(bool success, const QString& error);
    void onFileViewContextMenu(const QPoint& pos);
    void onBrowseOutDir();
    void onAddFromJson();
    void onRemoveSelected();
    void onOutputPaths();

private:
    void setupLayout();
    void connectSignals();

    void addAllFilesUnderIndex(const QModelIndex& idx);
    void expandAllSubdirs(const QModelIndex& rootIdx);
    bool isSkippableExt(const QString& ext) const;

    QFileSystemModel* m_fsModel;
    QTreeView* m_fileView;
    FileListWidget* m_list;
    QLineEdit* m_outDirEdit;
    QLineEdit* m_outNameEdit;
    QCheckBox* m_groupByFolder;
    QCheckBox* m_includeHeaders;
    QProgressBar* m_progress;
    QLabel* m_status;
    QCheckBox* m_showHiddenCheck;
    QPushButton* m_addSelectedBtn;
    QLineEdit* m_clearByExtEdit;
    QPushButton* m_clearByExtBtn;
    QPushButton* m_btnOutputPaths;
    QPushButton* m_btnBuild;
    QPushButton* m_btnBrowse;
    QPushButton* m_btnJson;
    QPushButton* m_btnRemove;
    QPushButton* m_btnClear;

    Glue* m_glue;
};

#endif