#include <LingmoIcon/IconLoader.h>
#include <LingmoIcon/IconTheme.h>
#include <QIcon>
#include <QPixmap>
#include <QDir>
namespace Lingmo {
QIcon IconLoader::fromTheme(const QString &iconName, const QSize &size) {
    QIcon icon = QIcon::fromTheme(iconName);
    if (icon.isNull()) {
        // Try with lingmo prefix
        icon = QIcon::fromTheme(QStringLiteral("lingmo-") + iconName);
    }
    if (!icon.isNull() && !size.isEmpty())
        icon = icon.pixmap(size);
    return icon;
}
QPixmap IconLoader::pixmap(const QString &iconName, const QSize &size) {
    const QSize actualSize = size.isEmpty() ? QSize(64, 64) : size;
    const QIcon icon = fromTheme(iconName, {});
    return icon.isNull() ? QPixmap(actualSize) : icon.pixmap(actualSize);
}
bool IconLoader::hasIcon(const QString &iconName) {
    return QIcon::hasThemeIcon(iconName) || QIcon::hasThemeIcon(QStringLiteral("lingmo-") + iconName);
}
QString IconLoader::iconPath(const QString &iconName, const QSize &) {
    const QString path = QIcon::themeSearchPaths().value(0) + QStringLiteral("/%1/scalable/%2.svg")
        .arg(IconTheme::instance()->themeName(), iconName);
    return QFile::exists(path) ? path : QString();
}
} // namespace Lingmo
