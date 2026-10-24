#pragma once
#include <LingmoIcon/IconTheme.h>
#include <QHash>
#include <QStringList>
#include <QVector>

namespace Lingmo {

// One directory entry parsed from a theme's index.theme file.
struct IconDirEntry {
    QString path;       // e.g. "16x16/actions"
    int size = 0;       // Nominal size
    int minSize = 0;    // For Scalable/Threshold types
    int maxSize = 0;
    int threshold = 2;  // Default per spec
    QString context;    // "actions", "apps", "mimetypes", ...
    enum Type { Fixed, Scalable, Threshold } type = Threshold;
};

// A fully parsed icon theme.
struct ParsedTheme {
    QString name;           // Directory name (the theme id)
    QString internalName;   // [Icon Theme] Name= value
    QStringList inherits;  // [Icon Theme] Inherits= value
    QList<IconDirEntry> directories;
};

class IconThemePrivate
{
public:
    QString themeName;
    QStringList searchPaths;

    // Cache of parsed themes, keyed by theme directory name.
    QHash<QString, ParsedTheme> parsedCache;
    bool cacheDirty = true;

    // Parse a single theme's index.theme from any search path that contains it.
    // Returns false if the theme is not found or has no index.theme.
    bool parseTheme(const QString &themeDirName, ParsedTheme &out);

    // Build the full fallback chain for the current theme, including "hicolor".
    QStringList fallbackChain() const;

    // Look up an icon file across the chain.
    // category is the directory context ("actions", "apps", ...) or empty for any.
    QString lookupIcon(const QString &iconName, int size, const QString &category);

    // Find the best-matching directory entry for the requested size.
    static const IconDirEntry *bestEntry(const ParsedTheme &theme,
                                          int size, const QString &category);

    // Resolve an icon filename within a theme directory entry.
    static QString resolveFile(const QString &searchPath, const QString &themeDir,
                                const IconDirEntry &entry, const QString &iconName);
};

} // namespace Lingmo
