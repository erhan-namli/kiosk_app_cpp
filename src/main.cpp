#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include "AppController.h"
#include "AppSettings.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Photo Scanner Kiosk");
    app.setOrganizationName("SCAN.RN");
    app.setApplicationVersion("1.0.0");

    // Load settings
    AppSettings settings;

    // Check for command line arguments
    for (int i = 1; i < argc; i++) {
        if (QString(argv[i]) == "--demo") {
            settings.demoMode = true;
        }
        if (QString(argv[i]) == "--scanner-only") {
            settings.scannerOnlyMode = true;
        }
        if (QString(argv[i]) == "--fullscreen") {
            settings.fullscreen = true;
        }
    }

    if (settings.demoMode) {
        qInfo() << "";
        qInfo() << "==================================================";
        qInfo() << "  DEMO MODE ACTIVE - No hardware needed!";
        qInfo() << "==================================================";
        qInfo() << "";
    } else if (settings.scannerOnlyMode) {
        qInfo() << "";
        qInfo() << "==================================================";
        qInfo() << "  SCANNER ONLY MODE - Payment skipped!";
        qInfo() << "==================================================";
        qInfo() << "";
    }

    // Create application controller
    AppController controller(&settings);

    // Initialize scanner and other components
    controller.initialize();

    // Create QML engine
    QQmlApplicationEngine engine;

    // Expose controller to QML
    engine.rootContext()->setContextProperty("appController", &controller);

    // Load main QML file
    const QUrl url(QStringLiteral("qrc:/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load QML";
        return -1;
    }

    qInfo() << "Application started successfully";
    qInfo() << "Scanner:" << (settings.demoMode ? "Demo Mode" : "Hardware");

    return app.exec();
}
