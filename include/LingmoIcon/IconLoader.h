#pragma once
#include <LingmoIcon/LingmoIconExport.h>
#include <QObject>
#include <QString>
#include <QPixmap>
#include <QIcon>
#include <QSize>

namespace Lingmo {

// Theme-aware icon loading with fallback chain.
// Loads icons from the current XDG icon theme, applying fallback rules.
class LINGMOICON_EXPORT IconLoader
{
public:
    // Load an icon by name and size
    static QIcon fromTheme(const QString &iconName, const QSize &size = {});

    // Load a pixmap by name and size
    static QPixmap pixmap(const QString &iconName, const QSize &size = {});

    // Check if an icon exists in the current theme
    static bool hasIcon(const QString &iconName);

    // Get the file path of an icon
    static QString iconPath(const QString &iconName, const QSize &size = {});

private:
    IconLoader() = delete;
};

} // namespace Lingmo
