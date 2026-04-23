#ifndef GLUE_H
#define GLUE_H

#include <QObject>
#include <QStringList>

struct GlueSettings {
    bool groupByFolder;
    bool includeHeaders;
    QString outputDirectory;
    QString outputFilename;
};

class Glue : public QObject {
    Q_OBJECT
public:
    explicit Glue(QObject* parent = nullptr);

    void execute(const QStringList& paths, const GlueSettings& settings);
    void writePathsList(const QStringList& paths, const GlueSettings& settings);

    signals:
        void progressUpdated(int current, int total);
    void statusMessage(const QString& message);
    void finished(bool success, const QString& errorMessage = "");

private:
    static bool byFolderThenName(const QString& a, const QString& b);
};

#endif