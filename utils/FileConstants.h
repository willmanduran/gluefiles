#ifndef FILECONSTANTS_H
#define FILECONSTANTS_H

#include <QStringList>
#include <QSet>

namespace Config {
    static constexpr const char* DefaultOutputName = "output.txt";
    static constexpr const char* AppName = "GlueFiles";
    static constexpr const char* Organization = "SQC";

    const QSet<QString> ImageExtensions = {
        "jpg", "jpeg", "png", "gif", "bmp", "webp",
        "tif", "tiff", "svg", "ico", "avif", "heic"
    };

    const QSet<QString> VideoExtensions = {
        "mp4", "mkv", "mov", "avi", "wmv", "flv",
        "webm", "m4v", "mpg", "mpeg", "3gp", "ts"
    };

    const QSet<QString> ExecutableExtensions = {
        "exe", "deb", "bin", "msi", "appimage"
    };
}

#endif