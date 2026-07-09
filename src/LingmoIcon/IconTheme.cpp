#include "private/IconTheme_p.h"
#include <QDir>
#include <QStandardPaths>
namespace Lingmo {
static IconTheme *s_instance = nullptr;
IconTheme::IconTheme(QObject *parent) : QObject(parent), d(std::make_unique<IconThemePrivate>()) {
    d->themeName = QStringLiteral("lingmo-icons");
    d->searchPaths = { QStringLiteral("/usr/share/icons"), QDir::homePath() + QStringLiteral("/.local/share/icons") };
}
IconTheme::~IconTheme() = default;
IconTheme *IconTheme::instance() { if (!s_instance) s_instance = new IconTheme; return s_instance; }
QString IconTheme::themeName() const { return d->themeName; }
void IconTheme::setThemeName(const QString &name) { if (d->themeName != name) { d->themeName = name; emit themeNameChanged(); } }
QStringList IconTheme::searchPaths() const { return d->searchPaths; }
void IconTheme::setSearchPaths(const QStringList &paths) { d->searchPaths = paths; emit searchPathsChanged(); }
QStringList IconTheme::availableThemes() const {
    QStringList themes;
    for (const QString &path : d->searchPaths) {
        const QDir dir(path);
        if (!dir.exists()) continue;
        const QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &entry : entries) {
            if (QDir(dir.filePath(entry)).exists(QStringLiteral("index.theme")))
                themes.append(entry);
        }
    }
    return themes;
}
QString IconTheme::fallbackIcon(const QString &iconName) const { return iconName; }
} // namespace Lingmo
