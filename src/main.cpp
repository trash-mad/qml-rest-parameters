#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "apiprovider.h"

#include <iostream>

/*****************************************************************************/

static void logHandler(
    QtMsgType type,
    const QMessageLogContext &context,
    const QString &msg) {
    Q_UNUSED(context);
    Q_UNUSED(type);
    std::cout<<msg.toStdString()<<"\n";
}

/*****************************************************************************/

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType(
        "com.tripolskypetr.quitejs",
        1, 0,
        "Api",
        ApiProvider::initSingletonType
    );

    #ifdef _WIN32
        /*
         * Not working correctly on linux and macOS
         * in QtCreator IDE Application Output
         *
         * just logging with separated console window
         * if you want, you can remove this ifdef switch
         * and CONFIG += console from project file
         */
        qInstallMessageHandler(logHandler);
    #endif

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}

/*****************************************************************************/
