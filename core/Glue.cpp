#include "Glue.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <algorithm>

Glue::Glue(QObject* parent) : QObject(parent) {}

bool Glue::byFolderThenName(const QString& a, const QString& b) {
    QFileInfo fa(a), fb(b);
    const int cmp = QString::compare(fa.absolutePath(), fb.absolutePath(), Qt::CaseInsensitive);
    if (cmp != 0) return cmp < 0;
    return QString::compare(fa.fileName(), fb.fileName(), Qt::CaseInsensitive) < 0;
}

void Glue::execute(const QStringList& originalPaths, const GlueSettings& settings) {
    QStringList paths = originalPaths;

    if (paths.isEmpty()) {
        emit finished(false, tr("No files to process."));
        return;
    }

    if (settings.groupByFolder) {
        std::sort(paths.begin(), paths.end(), byFolderThenName);
    }

    QDir outDir(settings.outputDirectory);
    if (!outDir.exists() && !outDir.mkpath(".")) {
        emit finished(false, tr("Failed to create output directory."));
        return;
    }

    QFile outFile(outDir.filePath(settings.outputFilename));
    if (!outFile.open(QIODevice::WriteOnly)) {
        emit finished(false, outFile.errorString());
        return;
    }

    const QByteArray nl("\n");
    for (int i = 0; i < paths.size(); ++i) {
        const QString& p = paths[i];

        if (settings.includeHeaders) {
            outFile.write(p.toUtf8() + nl + nl);
        }

        QFile inFile(p);
        if (inFile.open(QIODevice::ReadOnly)) {
            const QByteArray data = inFile.readAll();
            outFile.write(data);
            if (data.isEmpty() || (!data.endsWith('\n') && !data.endsWith('\r'))) {
                outFile.write(nl);
            }
        } else {
            outFile.write("<ERR: " + inFile.errorString().toUtf8() + ">\n");
        }

        outFile.write(nl);
        emit progressUpdated(i + 1, paths.size());
    }

    outFile.close();
    emit statusMessage(tr("Glued %1 files into %2")
                       .arg(paths.size())
                       .arg(settings.outputFilename));
    emit finished(true);
}

void Glue::writePathsList(const QStringList& paths, const GlueSettings& settings) {
    if (paths.isEmpty()) {
        emit finished(false, tr("No files to process."));
        return;
    }

    QDir outDir(settings.outputDirectory);
    if (!outDir.exists() && !outDir.mkpath(".")) {
        emit finished(false, tr("Failed to create output directory."));
        return;
    }

    QFile outFile(outDir.filePath(settings.outputFilename));
    if (!outFile.open(QIODevice::WriteOnly)) {
        emit finished(false, outFile.errorString());
        return;
    }

    for (const QString& p : paths) {
        outFile.write(p.toUtf8() + "\n");
    }

    outFile.close();
    emit statusMessage(tr("List of paths written to %1").arg(settings.outputFilename));
    emit finished(true);
}