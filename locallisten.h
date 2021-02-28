/**
 * locallisten.h
 * Erick Veil
 * 2021-02-28
 * Copyright 2021 Erick Veil
 */
#ifndef LOCALLISTEN_H
#define LOCALLISTEN_H

#include <functional>

#include <QByteArray>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QString>
#include <QTextStream>

/**
 * @brief The LocalListen class
 * This class sets up a local listener. Either use the init method with
 * arguments or set the public Attributes directly.
 * Uses callback functions so you don't have to fiddle with signals and
 * slots. Just assign Attributes and call `start()`.
 */
class LocalListen : public QObject
{
    Q_OBJECT

    QLocalServer _server;
    QByteArray _receivedDataBuffer;

    bool _isSignalsConnected = false;

    QLocalSocket *_connection = nullptr;

public:
    /**
     * @brief Name
     * Default is "b_socket" if not defined.
     * This is the name of the local domain socket that will be used.
     */
    QString Name;

    /**
     * @brief ParseCallback
     * If defined, this function will be called whenever new data is recieved.
     */
    std::function<void (QByteArray)> ParseCallback;

    /**
     * @brief AckCallback
     * If defined, this function will be called whenever new data is recieved.
     * Otherwise the listener will ack back with an echo.
     */
    std::function<void (QByteArray)> AckCallback;

    explicit LocalListen(QObject *parent = nullptr);
    ~LocalListen();

    /**
     * @brief init
     * This method will initialize the socket with whatever Attributes that
     * have been set, or the defaults if you haven't set any.
     * It connects all the signals and slots the first time it is run only.
     * Run it again after changing the attributes.
     */
    void init();

    /**
     * @brief init
     * @param name
     * @param parseCb
     * @param ackCb
     * Sets Attributes and calls its overloaded friend, `init()`.
     */
    void init(QString name, std::function<void (QByteArray)> parseCb,
              std::function<void (QByteArray)> ackCb);

    /**
     * @brief start
     * Starts the listener.
     * Must run an init method before calling this.
     */
    void start();

signals:

public slots:
    // QLocalServer
    void eventNewConnection();

    // QLocalSocket (_connection)
    void eventConnected();
    void eventDisconnected();
    void eventErrorOccurred(QLocalSocket::LocalSocketError errnum);
    void eventStateChanged(QLocalSocket::LocalSocketState state);

    // QIODevice (_connection)
    void eventIODeviceBytesWritten(qint64 bytes);
    void eventIODeviceReadChannelFinished();
    void eventIODeviceReadyRead();

private:
    void _processReceivedBytes();
};

#endif // LOCALLISTEN_H
