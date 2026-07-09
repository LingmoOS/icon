#include <LingmoIcon/IconLoader.h>
#include <LingmoIcon/IconTheme.h>
#include <QTest>
class tst_IconLoader : public QObject {
    Q_OBJECT
private Q_SLOTS:
    void testFromTheme() {
        // May return null icon if theme not installed, should not crash
        const QIcon icon = Lingmo::IconLoader::fromTheme(QStringLiteral("nonexistent-icon"));
        Q_UNUSED(icon);
    }
    void testHasIcon() {
        // Should not crash
        Lingmo::IconLoader::hasIcon(QStringLiteral("nonexistent-icon"));
    }
    void testPixmap() {
        const QPixmap pm = Lingmo::IconLoader::pixmap(QStringLiteral("nonexistent"), QSize(32, 32));
        QVERIFY(!pm.isNull());
    }
};
QTEST_MAIN(tst_IconLoader)
#include "tst_IconLoader.moc"
