#include <LingmoIcon/IconLoader.h>
#include <LingmoIcon/IconTheme.h>

#include <QFile>
#include <QIcon>
#include <QPixmap>

namespace Lingmo {

QIcon IconLoader::fromTheme(const QString &iconName, const QSize &size)
{
    // 1. Try our own XDG-compliant lookup to find the best file path.
    const int px = size.isValid() ? std::max(size.width(), size.height()) : 0;
    const QString path = iconPath(iconName, size);
    if (!path.isEmpty()) {
        QIcon icon(path);
        if (!icon.isNull())
            return icon;
    }

    // 2. Fall back to Qt's built-in QIcon::fromTheme, which respects
    //    QIcon::setThemeName / setThemeSearchPaths (synced by IconTheme).
    QIcon icon = QIcon::fromTheme(iconName);
    if (icon.isNull())
        icon = QIcon::fromTheme(QStringLiteral("lingmo-") + iconName);

    // 3. Scale to requested size if an exact-size variant wasn't loaded.
    if (!icon.isNull() && size.isValid() && !size.isEmpty()) {
        // Return as a pixmap-based icon at the requested size.
        return QIcon(icon.pixmap(size));
    }
    return icon;
}

QPixmap IconLoader::pixmap(const QString &iconName, const QSize &size)
{
    const QSize actualSize = (size.isValid() && !size.isEmpty())
                             ? size
                             : QSize(64, 64);
    const QIcon icon = fromTheme(iconName, {});
    if (!icon.isNull())
        return icon.pixmap(actualSize);
    // Return a transparent placeholder rather than a null pixmap so callers
    // can always use the result without crashing.
    QPixmap placeholder(actualSize);
    placeholder.fill(Qt::transparent);
    return placeholder;
}

bool IconLoader::hasIcon(const QString &iconName)
{
    // Check our XDG lookup first, then Qt's built-in registry.
    if (!iconPath(iconName, {}).isEmpty())
        return true;
    return QIcon::hasThemeIcon(iconName)
        || QIcon::hasThemeIcon(QStringLiteral("lingmo-") + iconName);
}

QString IconLoader::iconPath(const QString &iconName, const QSize &size)
{
    IconTheme *theme = IconTheme::instance();
    const int px = (size.isValid() && !size.isEmpty())
                   ? std::max(size.width(), size.height())
                   : 32; // default lookup size

    return theme->lookupIconPath(iconName, px, {});
}

} // namespace Lingmo
