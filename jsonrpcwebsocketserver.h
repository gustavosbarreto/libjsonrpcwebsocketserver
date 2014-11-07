#ifndef JSONRPCWEBSOCKETSERVER_H
#define JSONRPCWEBSOCKETSERVER_H

#include <QObject>
#include <QQueue>
#include <QMap>

class QWebSocket;

class JsonRPCWebSocketServer: public QObject
{
    Q_OBJECT

public:
    JsonRPCWebSocketServer(QObject *parent = 0);

public slots:
    void connect(const QUrl &url);

    void registerInterface(const QString &name, QObject *interface);

    void call(const QString &interface, const QString &method,
              const QVariantList &args, std::function<void(QVariant)> callback = NULL);

private slots:
    void processMessage(QString message);

private:
    QWebSocket *_socket;

    int _id;
    bool _connected;
    QQueue<QByteArray> _sendQueue;
    QMap<int, std::function<void(QVariant)>> _callbacks;
    QMap<QString, QObject *> _interfaces;
};

#endif // JSONRPCWEBSOCKETSERVER_H
