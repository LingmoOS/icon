#pragma once
#include <LingmoIcon/IconTheme.h>
#include <QStringList>
namespace Lingmo {
class IconThemePrivate {
public:
    QString themeName;
    QStringList searchPaths;
};
} // namespace Lingmo
