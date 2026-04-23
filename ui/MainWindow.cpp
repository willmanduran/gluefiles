#include "MainWindow.h"
#include "FileListWidget.h"
#include "../core/Glue.h"
#include "../utils/FileConstants.h"

#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QProgressBar>
#include <QLineEdit>
#include <QCheckBox>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QDirIterator>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QEventLoop>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_glue(new Glue(this))
{
    setWindowTitle(tr("GlueFiles"));
    resize(1040, 640);

    setupLayout();
    connectSignals();
}

void MainWindow::setupLayout() {
    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* root = new QVBoxLayout(central);

    root->addWidget(new QLabel(tr("Browse on the left, add to the list on the right.")));

    auto* split = new QSplitter(Qt::Horizontal, this);
    root->addWidget(split, 1);

    auto* leftPanel = new QWidget(this);
    auto* leftLayout = new QVBoxLayout(leftPanel);

    auto* leftTools = new QHBoxLayout();
    m_addSelectedBtn = new QPushButton(tr("Add Selected →"));
    m_showHiddenCheck = new QCheckBox(tr("Show hidden"));
    leftTools->addWidget(m_addSelectedBtn);
    leftTools->addStretch(1);
    leftTools->addWidget(m_showHiddenCheck);
    leftLayout->addLayout(leftTools);

    m_fsModel = new QFileSystemModel(this);
    m_fsModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    const QString home = QDir::homePath();
    m_fsModel->setRootPath(home);

    m_fileView = new QTreeView(this);
    m_fileView->setModel(m_fsModel);
    m_fileView->setRootIndex(m_fsModel->index(home));
    m_fileView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_fileView->setSortingEnabled(true);
    m_fileView->setContextMenuPolicy(Qt::CustomContextMenu);

    leftLayout->addWidget(m_fileView);
    split->addWidget(leftPanel);

    auto* rightPanel = new QWidget(this);
    auto* rightLayout = new QVBoxLayout(rightPanel);

    m_list = new FileListWidget(this);
    rightLayout->addWidget(m_list);

    auto* listBtns = new QHBoxLayout();
    m_btnJson = new QPushButton(tr("Add from JSON…"));
    m_btnRemove = new QPushButton(tr("Remove Selected"));
    m_btnClear = new QPushButton(tr("Clear"));
    m_clearByExtEdit = new QLineEdit();
    m_clearByExtBtn = new QPushButton(tr("Limpiar por ext"));

    listBtns->addWidget(m_btnJson);
    listBtns->addWidget(m_btnRemove);
    listBtns->addWidget(m_btnClear);
    listBtns->addWidget(m_clearByExtEdit);
    listBtns->addWidget(m_clearByExtBtn);
    rightLayout->addLayout(listBtns);

    auto* outRow = new QHBoxLayout();
    m_outDirEdit = new QLineEdit(QDir::homePath());
    m_btnBrowse = new QPushButton(tr("Browse…"));
    outRow->addWidget(new QLabel(tr("Output Folder:")));
    outRow->addWidget(m_outDirEdit, 1);
    outRow->addWidget(m_btnBrowse);
    rightLayout->addLayout(outRow);

    m_outNameEdit = new QLineEdit(Config::DefaultOutputName);
    auto* nameRow = new QHBoxLayout();
    nameRow->addWidget(new QLabel(tr("Filename:")));
    nameRow->addWidget(m_outNameEdit);
    rightLayout->addLayout(nameRow);

    m_groupByFolder = new QCheckBox(tr("Group by folder"));
    m_groupByFolder->setChecked(true);
    m_includeHeaders = new QCheckBox(tr("Include file path headers"));
    rightLayout->addWidget(m_groupByFolder);
    rightLayout->addWidget(m_includeHeaders);

    auto* buildRow = new QHBoxLayout();
    m_progress = new QProgressBar();

    m_btnOutputPaths = new QPushButton(tr("Output Paths Only"));
    m_btnBuild = new QPushButton(tr("Build Output"));

    buildRow->addWidget(m_progress, 1);
    buildRow->addWidget(m_btnOutputPaths);
    buildRow->addWidget(m_btnBuild);
    rightLayout->addLayout(buildRow);

    m_status = new QLabel("");
    rightLayout->addWidget(m_status);

    split->addWidget(rightPanel);
}

void MainWindow::connectSignals() {
    connect(m_addSelectedBtn, &QPushButton::clicked, this, &MainWindow::onAddSelected);
    connect(m_showHiddenCheck, &QCheckBox::toggled, this, [this](bool on) {
        auto f = QDir::AllEntries | QDir::NoDotAndDotDot;
        if (on) f |= QDir::Hidden;
        m_fsModel->setFilter(f);
    });
    connect(m_fileView, &QTreeView::doubleClicked, this, [this](const QModelIndex& idx) {
        if (idx.isValid() && !m_fsModel->isDir(idx)) m_list->addPaths({ m_fsModel->filePath(idx) });
    });
    connect(m_fileView, &QTreeView::customContextMenuRequested, this, &MainWindow::onFileViewContextMenu);

    connect(m_btnJson, &QPushButton::clicked, this, &MainWindow::onAddFromJson);
    connect(m_btnRemove, &QPushButton::clicked, this, &MainWindow::onRemoveSelected);
    connect(m_btnClear, &QPushButton::clicked, m_list, &QListWidget::clear);
    connect(m_clearByExtBtn, &QPushButton::clicked, this, &MainWindow::onClearByExt);
    connect(m_btnBrowse, &QPushButton::clicked, this, &MainWindow::onBrowseOutDir);
    connect(m_btnBuild, &QPushButton::clicked, this, &MainWindow::onBuild);
    connect(m_btnOutputPaths, &QPushButton::clicked, this, &MainWindow::onOutputPaths);
    connect(m_glue, &Glue::progressUpdated, this, &MainWindow::updateProgress);
    connect(m_glue, &Glue::statusMessage, m_status, &QLabel::setText);
    connect(m_glue, &Glue::finished, this, &MainWindow::handleFinished);
}

bool MainWindow::isSkippableExt(const QString& ext) const {
    const QString e = ext.toLower();
    return Config::ImageExtensions.contains(e) ||
           Config::VideoExtensions.contains(e) ||
           Config::ExecutableExtensions.contains(e);
}

void MainWindow::addAllFilesUnderIndex(const QModelIndex& idx) {
    const QString rootDir = m_fsModel->filePath(idx);
    if (rootDir.isEmpty()) return;

    QStringList candidates;
    QDirIterator it(rootDir, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        const QString filePath = it.next();
        if (!isSkippableExt(QFileInfo(filePath).suffix())) candidates.append(filePath);
    }

    if (!candidates.isEmpty()) {
        m_list->addPaths(candidates);
        m_status->setText(tr("Added %1 files from %2").arg(candidates.size()).arg(rootDir));
    }
}

void MainWindow::expandAllSubdirs(const QModelIndex& rootIdx) {
    if (!rootIdx.isValid() || !m_fsModel->isDir(rootIdx)) return;

    QList<QModelIndex> queue = { rootIdx };
    while (!queue.isEmpty()) {
        const QModelIndex cur = queue.takeFirst();
        m_fileView->expand(cur);

        while (m_fsModel->canFetchMore(cur)) {
            m_fsModel->fetchMore(cur);
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        for (int r = 0; r < m_fsModel->rowCount(cur); ++r) {
            const QModelIndex child = m_fsModel->index(r, 0, cur);
            if (m_fsModel->isDir(child)) queue.append(child);
        }
    }
}

void MainWindow::onFileViewContextMenu(const QPoint& pos) {
    const QModelIndex idx = m_fileView->indexAt(pos);
    if (!idx.isValid() || !m_fsModel->isDir(idx)) return;

    QMenu menu(this);
    QAction* actExpand = menu.addAction(tr("Expand all subfolders"));
    QAction* actAddAll = menu.addAction(tr("Add all files in folder"));

    QAction* chosen = menu.exec(m_fileView->viewport()->mapToGlobal(pos));
    if (chosen == actExpand) expandAllSubdirs(idx);
    else if (chosen == actAddAll) addAllFilesUnderIndex(idx);
}

void MainWindow::onAddSelected() {
    const auto rows = m_fileView->selectionModel()->selectedRows(0);
    QStringList paths;
    for (const QModelIndex& idx : rows) {
        if (!m_fsModel->isDir(idx)) paths.append(m_fsModel->filePath(idx));
    }
    m_list->addPaths(paths);
}

void MainWindow::onAddFromJson() {
    bool ok;
    const QString text = QInputDialog::getMultiLineText(this, tr("Paste JSON"), tr("Array or {\"files\": []}"), "", &ok);
    if (!ok || text.trimmed().isEmpty()) return;

    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
    QStringList paths;
    if (doc.isArray()) {
        for (const auto& v : doc.array()) paths.append(v.toString());
    } else if (doc.isObject() && doc.object().contains("files")) {
        for (const auto& v : doc.object().value("files").toArray()) paths.append(v.toString());
    }
    m_list->addPaths(paths);
}

void MainWindow::onRemoveSelected() {
    for (QListWidgetItem* it : m_list->selectedItems()) delete it;
}

void MainWindow::onClearByExt() {
    const QString raw = m_clearByExtEdit->text().trimmed().toLower();
    if (raw.isEmpty()) return;

    QStringList exts = raw.split(QRegularExpression("[,;\\s]+"), Qt::SkipEmptyParts);
    for (QString& e : exts) if (e.startsWith('.')) e.remove(0, 1);

    int removed = 0;
    for (int i = m_list->count() - 1; i >= 0; --i) {
        QString ext = QFileInfo(m_list->item(i)->data(Qt::UserRole).toString()).suffix().toLower();
        if (exts.contains(ext)) {
            delete m_list->takeItem(i);
            removed++;
        }
    }
    m_status->setText(tr("Removed %1 files").arg(removed));
}

void MainWindow::onBrowseOutDir() {
    QString d = QFileDialog::getExistingDirectory(this, tr("Output Folder"), m_outDirEdit->text());
    if (!d.isEmpty()) m_outDirEdit->setText(d);
}

void MainWindow::onBuild() {
    GlueSettings s;
    s.groupByFolder = m_groupByFolder->isChecked();
    s.includeHeaders = m_includeHeaders->isChecked();
    s.outputDirectory = m_outDirEdit->text();
    s.outputFilename = m_outNameEdit->text();

    m_btnBuild->setEnabled(false);
    m_glue->execute(m_list->currentPaths(), s);
}

void MainWindow::updateProgress(int current, int total) {
    m_progress->setRange(0, total);
    m_progress->setValue(current);
}

void MainWindow::handleFinished(bool success, const QString& error) {
    m_btnBuild->setEnabled(true);
    m_btnOutputPaths->setEnabled(true);
    if (!success) QMessageBox::critical(this, tr("Error"), error);
    else QMessageBox::information(this, tr("Done"), tr("Operation completed successfully!"));
}

void MainWindow::onOutputPaths() {
    GlueSettings s;
    s.outputDirectory = m_outDirEdit->text();
    s.outputFilename = m_outNameEdit->text();

    m_btnOutputPaths->setEnabled(false);
    m_glue->writePathsList(m_list->currentPaths(), s);
    m_progress->setRange(0, 1);
    m_progress->setValue(1);
}