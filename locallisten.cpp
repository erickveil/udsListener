#include "locallisten.h"

LocalListen::LocalListen(QObject *parent) : QObject(parent)
{

}

LocalListen::~LocalListen()
{
    if (_server.isListening()) { _server.close(); }
}

void LocalListen::init()
{
    if (Name == "") { Name = "b_socket"; }
    QLocalServer::removeServer(Name);
    _server.setSocketOptions(QLocalServer::WorldAccessOption);

    if (_isSignalsConnected) { return; }
    connect(&_server,
            SIGNAL(newConnection()),
            this,
            SLOT(eventNewConnection()));
    _isSignalsConnected = true;
}

void LocalListen::init(QString name, std::function<void (QByteArray)> parseCb,
                       std::function<void (QByteArray)> ackCb)
{
    Name = name;
    ParseCallback = parseCb;
    AckCallback = ackCb;

    init();
}

void LocalListen::start()
{
    if (!_isSignalsConnected) { init(); }
    _server.listen(Name);
}

void LocalListen::eventNewConnection()
{
    _receivedDataBuffer.clear();
    _connection = _server.nextPendingConnection();

    connect(_connection,
            SIGNAL(connected()),
            this,
            SLOT(eventConnected()));
    connect(_connection,
            SIGNAL(disconnected()),
            this,
            SLOT(eventDisconnected()));
    connect(_connection,
            SIGNAL(error(QLocalSocket::LocalSocketError)),
            this,
            SLOT(eventErrorOccurred(QLocalSocket::LocalSocketError)));
    connect(_connection,
            SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
            this,
            SLOT(eventStateChanged(QLocalSocket::LocalSocketState)));
    connect(_connection,
            SIGNAL(bytesWritten(qint64)),
            this,
            SLOT(eventIODeviceBytesWritten(qint64)));
    connect(_connection,
            SIGNAL(readChannelFinished()),
            this,
            SLOT(eventIODeviceReadChannelFinished()));
    connect(_connection,
            SIGNAL(readyRead()),
            this,
            SLOT(eventIODeviceReadyRead()));

}

void LocalListen::eventConnected()
{

}

void LocalListen::eventDisconnected()
{

}

void LocalListen::eventErrorOccurred(QLocalSocket::LocalSocketError errnum)
{
    Q_UNUSED(errnum);
    QTextStream errOut( stderr );
    errOut << "error: " << _connection->errorString();
    errOut.flush();
}

void LocalListen::eventStateChanged(QLocalSocket::LocalSocketState state)
{
    switch (state) {
    case QLocalSocket::UnconnectedState:
        //
        break;
    case QLocalSocket::ConnectingState:
        //
        break;
    case QLocalSocket::ConnectedState:
        //
        break;
    case QLocalSocket::ClosingState:
        //
        break;
    default:
        // unknown state
        break;
    }
}

void LocalListen::eventIODeviceBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);

    // after writing response, we are done
    _connection->close();
}

void LocalListen::eventIODeviceReadChannelFinished()
{
    if (ParseCallback) { ParseCallback(_receivedDataBuffer); }
}

void LocalListen::eventIODeviceReadyRead()
{
    _processReceivedBytes();
}

void LocalListen::_processReceivedBytes()
{
    // might need to one day loop here for large files if this works like TCP.
    _receivedDataBuffer = _connection->readAll();
    _connection->flush();

    if (AckCallback) { AckCallback(_receivedDataBuffer); }
    else {
        _connection->write(_receivedDataBuffer);
    }

    _connection->waitForBytesWritten();

}

