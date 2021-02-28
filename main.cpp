#include <QCoreApplication>

#include "locallisten.h"
#include "QTextStream"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextStream output( stdout );
    output << "Starting listener.\n";
    output.flush();

    auto parseCb = [&] (QByteArray msg) {
        output << msg << "\n";
        output.flush();
    };

    QString name = "b_socket";

    LocalListen listener;
    listener.Name = name;
    listener.ParseCallback = parseCb;
    listener.init();
    listener.start();


    return a.exec();
}
