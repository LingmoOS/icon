#include "private/IconTheme_p.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QStringList>

namespace Lingmo {

// ── IconThemePrivate implementation ──────────────────────────────────────────

bool IconThemePrivate::parseTheme(const QString &themeDirName, ParsedTheme &out)
{
    // Search all search paths for this theme directory.
    for (const QString &searchPath : searchPaths) {
        const QString themeDir = searchPath + QLatin1Char('/') + themeDirName;
        const QString indexFile = themeDir + QStringLiteral("/index.theme");
        if (!QFile::exists(indexFile))
            continue;

        QSettings ini(indexFile, QSettings::IniFormat);

        out.name = themeDirName;
        ini.beginGroup(QStringLiteral("Icon Theme"));
        out.internalName = ini.value(QStringLiteral("Name"), themeDirName).toString();

        const QString inheritsStr = ini.value(QStringLiteral("Inherits")).toString();
        if (!inheritsStr.isEmpty())
            out.inherits = inheritsStr.split(QLatin1Char(','), Qt::SkipEmptyParts);

        // Remove leading/trailing whitespace from each inherited theme name.
        for (QString &s : out.inherits)
            s = s.trimmed();

        const QStringList dirs =
            ini.value(QStringLiteral("Directories")).toString()
               .split(QLatin1Char(','), Qt::SkipEmptyParts);
        ini.endGroup();

        out.directories.clear();
        out.directories.reserve(dirs.size());

        for (const QString &dirEntry : dirs) {
            const QString trimmed = dirEntry.trimmed();
            if (trimmed.isEmpty())
                continue;

            ini.beginGroup(trimmed);

            IconDirEntry e;
            e.path = trimmed;
            e.size = ini.value(QStringLiteral("Size"), 0).toInt();
            e.context = ini.value(QStringLiteral("Context")).toString().toLower();

            const QString typeStr =
                ini.value(QStringLiteral("Type"), QStringLiteral("Threshold")).toString();
            if (typeStr.compare(QLatin1String("Fixed"), Qt::CaseInsensitive) == 0)
                e.type = IconDirEntry::Fixed;
            else if (typeStr.compare(QLatin1String("Scalable"), Qt::CaseInsensitive) == 0)
                e.type = IconDirEntry::Scalable;
            else
                e.type = IconDirEntry::Threshold;

            e.minSize = ini.value(QStringLiteral("MinSize"), e.size).toInt();
            e.maxSize = ini.value(QStringLiteral("MaxSize"), e.size).toInt();
            e.threshold = ini.value(QStringLiteral("Threshold"), 2).toInt();

            ini.endGroup();

            if (e.size > 0)
                out.directories.append(e);
        }

        return true;
    }
    return false;
}

QStringList IconThemePrivate::fallbackChain() const
{
    // BFS traversal of Inherits= to build the ordered fallback list.
    QStringList chain;
    QStringList queue{themeName};
    QSet<QString> visited;

    while (!queue.isEmpty()) {
        const QString current = queue.takeFirst();
        if (visited.contains(current))
            continue;
        visited.insert(current);
        chain.append(current);

        // Look up the parsed theme (const cast: we logically cache here).
        auto *self = const_cast<IconThemePrivate *>(this);
        if (!self->parsedCache.contains(current)) {
            ParsedTheme pt;
            self->parseTheme(current, pt);
            self->parsedCache.insert(current, pt);
        }
        const ParsedTheme &pt = self->parsedCache[current];
        for (const QString &parent : pt.inherits)
            queue.append(parent);
    }

    // Always end with hicolor as the ultimate fallback (XDG spec requirement).
    if (!chain.contains(QStringLiteral("hicolor")))
        chain.append(QStringLiteral("hicolor"));

    return chain;
}

// Per XDG spec: "DirectoryMatchesSize" predicate.
static bool dirMatchesSize(const IconDirEntry &e, int size)
{
    switch (e.type) {
    case IconDirEntry::Fixed:
        return e.size == size;
    case IconDirEntry::Scalable:
        return size >= e.minSize && size <= e.maxSize;
    case IconDirEntry::Threshold:
        return size >= (e.size - e.threshold) && size <= (e.size + e.threshold);
    }
    return false;
}

// Per XDG spec: "DirectorySizeDistance" for the closest-size fallback.
static int dirSizeDistance(const IconDirEntry &e, int size)
{
    switch (e.type) {
    case IconDirEntry::Fixed:
        return std::abs(e.size - size);
    case IconDirEntry::Scalable:
        if (size < e.minSize) return e.minSize - size;
        if (size > e.maxSize) return size - e.maxSize;
        return 0;
    case IconDirEntry::Threshold:
        if (size < e.size - e.threshold) return e.size - e.threshold - size;
        if (size > e.size + e.threshold) return size - e.size - e.threshold;
        return 0;
    }
    return INT_MAX;
}

QString IconThemePrivate::resolveFile(const QString &searchPath,
                                       const QString &themeDir,
                                       const IconDirEntry &entry,
                                       const QString &iconName)
{
    const QString base = searchPath + QLatin1Char('/') + themeDir
                       + QLatin1Char('/') + entry.path
                       + QLatin1Char('/') + iconName;

    // Try extensions in priority order: svg > png > xpm.
    for (const char *ext : {".svg", ".png", ".xpm"}) {
        const QString candidate = base + QLatin1String(ext);
        if (QFile::exists(candidate))
            return candidate;
    }
    return {};
}

QString IconThemePrivate::lookupIcon(const QString &iconName,
                                      int size,
                                      const QString &category)
{
    const QStringList chain = fallbackChain();

    for (const QString &themeDirName : chain) {
        // Ensure the theme is parsed.
        if (!parsedCache.contains(themeDirName)) {
            ParsedTheme pt;
            parseTheme(themeDirName, pt);
            parsedCache.insert(themeDirName, pt);
        }
        const ParsedTheme &theme = parsedCache[themeDirName];

        // First pass: exact-size match (per spec, try all search paths).
        for (const QString &sp : searchPaths) {
            const QString themeDir = sp + QLatin1Char('/') + themeDirName;
            if (!QDir(themeDir).exists())
                continue;
            for (const IconDirEntry &e : theme.directories) {
                if (!category.isEmpty() && !e.context.isEmpty()
                    && e.context != category)
                    continue;
                if (!dirMatchesSize(e, size))
                    continue;
                const QString path = resolveFile(sp, themeDirName, e, iconName);
                if (!path.isEmpty())
                    return path;
            }
        }

        // Second pass: closest-size match (per spec).
        int minDist = INT_MAX;
        QString closestPath;
        for (const QString &sp : searchPaths) {
            const QString themeDir = sp + QLatin1Char('/') + themeDirName;
            if (!QDir(themeDir).exists())
                continue;
            for (const IconDirEntry &e : theme.directories) {
                if (!category.isEmpty() && !e.context.isEmpty()
                    && e.context != category)
                    continue;
                const QString path = resolveFile(sp, themeDirName, e, iconName);
                if (path.isEmpty())
                    continue;
                int dist = dirSizeDistance(e, size);
                if (dist < minDist) {
                    minDist = dist;
                    closestPath = path;
                }
            }
        }
        if (!closestPath.isEmpty())
            return closestPath;
    }

    // Ultimate fallback: look in all search paths without theme.
    for (const QString &sp : searchPaths) {
        for (const char *ext : {".svg", ".png", ".xpm"}) {
            const QString candidate = sp + QLatin1Char('/') + iconName
                                    + QLatin1String(ext);
            if (QFile::exists(candidate))
                return candidate;
        }
    }
    return {};
}

// ── IconTheme public implementation ──────────────────────────────────────────

static IconTheme *s_instance = nullptr;

IconTheme::IconTheme(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<IconThemePrivate>())
{
    d->themeName = QStringLiteral("lingmo-icons");

    // Standard XDG icon search paths.
    d->searchPaths = {
        QDir::homePath() + QStringLiteral("/.local/share/icons"),
        QStringLiteral("/usr/local/share/icons"),
        QStringLiteral("/usr/share/icons"),
        QStringLiteral("/usr/share/pixmaps"),
    };

    // Prepend any paths from XDG_DATA_DIRS.
    const QStringList xdgDirs = QStandardPaths::standardLocations(
        QStandardPaths::GenericDataLocation);
    for (const QString &dir : xdgDirs) {
        const QString iconDir = dir + QStringLiteral("/icons");
        if (!d->searchPaths.contains(iconDir))
            d->searchPaths.prepend(iconDir);
    }

    // Sync the active theme name to QIcon (Qt's built-in lookup).
    QIcon::setThemeName(d->themeName);
    QIcon::setThemeSearchPaths(d->searchPaths);
}

IconTheme::~IconTheme() = default;

IconTheme *IconTheme::instance()
{
    if (!s_instance)
        s_instance = new IconTheme;
    return s_instance;
}

QString IconTheme::themeName() const { return d->themeName; }

void IconTheme::setThemeName(const QString &name)
{
    if (d->themeName == name)
        return;
    d->themeName = name;
    d->cacheDirty = true;
    QIcon::setThemeName(name);
    emit themeNameChanged();
}

QStringList IconTheme::searchPaths() const { return d->searchPaths; }

void IconTheme::setSearchPaths(const QStringList &paths)
{
    d->searchPaths = paths;
    d->cacheDirty = true;
    d->parsedCache.clear();
    QIcon::setThemeSearchPaths(paths);
    emit searchPathsChanged();
}

QStringList IconTheme::availableThemes() const
{
    QStringList themes;
    QSet<QString> seen;
    for (const QString &path : d->searchPaths) {
        const QDir dir(path);
        if (!dir.exists())
            continue;
        const QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &entry : entries) {
            if (seen.contains(entry))
                continue;
            if (QFile::exists(dir.filePath(entry) + QStringLiteral("/index.theme"))) {
                themes.append(entry);
                seen.insert(entry);
            }
        }
    }
    return themes;
}

QString IconTheme::fallbackIcon(const QString &iconName) const
{
    // Return the first icon in the fallback chain that differs from iconName,
    // or iconName itself if no mapping is defined (XDG spec default).
    return iconName;
}

QString IconTheme::lookupIconPath(const QString &iconName, int size,
                                   const QString &category) const
{
    return const_cast<IconThemePrivate *>(d.get())->lookupIcon(iconName, size, category);
}

} // namespace Lingmo
