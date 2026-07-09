#pragma once
#include <LingmoIcon/LingmoIconExport.h>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSize>
#include <memory>

namespace Lingmo {

class IconThemePrivate;

// XDG icon theme introspection.
// Discovers available icon themes and their search paths.
class LINGMOICON_EXPORT IconTheme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY themeNameChanged)
    Q_PROPERTY(QStringList searchPaths READ searchPaths NOTIFY searchPathsChanged)

public:
    explicit IconTheme(QObject *parent = nullptr);
    ~IconTheme() override;

    static IconTheme *instance();

    QString themeName() const;
    void setThemeName(const QString &name);

    QStringList searchPaths() const;
    void setSearchPaths(const QStringList &paths);

    // List all available theme names
    QStringList availableThemes() const;

    // Get the fallback icon name for a given icon
    QString fallbackIcon(const QString &iconName) const;

Q_SIGNALS:
    void themeNameChanged();
    void searchPathsChanged();

private:
    std::unique_ptr<IconThemePrivate> d;
};

} // namespace Lingmo
