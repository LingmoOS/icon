#include <LingmoIcon/IconTheme.h>
#include <QTest>
class tst_IconTheme : public QObject {
    Q_OBJECT
private Q_SLOTS:
    void testInstance() {
        auto *t = Lingmo::IconTheme::instance();
        QVERIFY(t != nullptr);
        QCOMPARE(Lingmo::IconTheme::instance(), t);
    }
    void testDefaultTheme() {
        auto *t = Lingmo::IconTheme::instance();
        QVERIFY(!t->themeName().isEmpty());
    }
    void testSetTheme() {
        auto *t = Lingmo::IconTheme::instance();
        t->setThemeName(QStringLiteral("test-theme"));
        QCOMPARE(t->themeName(), QStringLiteral("test-theme"));
        t->setThemeName(QStringLiteral("lingmo-icons"));
    }
    void testSearchPaths() {
        auto *t = Lingmo::IconTheme::instance();
        QVERIFY(!t->searchPaths().isEmpty());
    }
    void testFallbackIcon() {
        auto *t = Lingmo::IconTheme::instance();
        QCOMPARE(t->fallbackIcon(QStringLiteral("test")), QStringLiteral("test"));
    }
};
QTEST_MAIN(tst_IconTheme)
#include "tst_IconTheme.moc"
