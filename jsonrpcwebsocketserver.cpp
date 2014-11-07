#include "jsonrpcwebsocketserver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

#include <QWebSocket.h>

JsonRPCWebSocketServer::JsonRPCWebSocketServer(QObject *parent) :
    QObject(parent)
{
    _id = 0;
    _connected = false;
    _socket = new QWebSocket(this);

    QObject::connect(_socket, SIGNAL(message(QString)), SLOT(processMessage(QString)));
    QObject::connect(_socket, &QWebSocket::opened, [=]() {
        while (!_sendQueue.isEmpty()) {
            _socket->send(_sendQueue.dequeue());
        }

        _connected = true;
    });
    QObject::connect(_socket, &QWebSocket::closed, [=]() {
        _connected = false;
    });
}

void JsonRPCWebSocketServer::connect(const QUrl &url)
{
    _socket->connect(url);
}

void JsonRPCWebSocketServer::registerInterface(const QString &name, QObject *interface)
{
    _interfaces[name] = interface;
}

void JsonRPCWebSocketServer::call(const QString &interface, const QString &method, const QVariantList &args, std::function<void(QVariant)> callback)
{
    QVariantMap map;
    map["method"] = interface + "." + method;
    map["jsonrpc"] = "2.0";
    map["params"] = args;
    map["id"] = ++_id;

    QByteArray data = QJsonDocument::fromVariant(map).toJson(QJsonDocument::Compact);

    if (!_connected) {
        _sendQueue.enqueue(data);
    } else {
        _socket->send(data);
    }

    _callbacks[_id] = callback;
}

void JsonRPCWebSocketServer::processMessage(QString message)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning("Error parsing json document");
        return;
    }

    if (!doc.object().contains("jsonrpc")) {
        qWarning("Invalid json-rpc document");
        return;
    }

    if (doc.object().contains("method")) {
        QStringList method = doc.object().value("method").toString().split(".");
        if (method.size() < 2) {
            qWarning("Invalid method name");
            return;
        }

        QObject *interface = _interfaces.value(method.at(0));
        if (!interface) {
            qWarning("Interface not found");
            return;
        }

        const QMetaObject *metaObject = interface->metaObject();

        int methodIndex = -1;
        for (int i = 0; i < metaObject->methodCount(); i++) {
            QMetaMethod m = metaObject->method(i);
            if (m.name() == method.at(1)) {
                methodIndex = i;
                break;
            }
        }

        if (methodIndex == -1) {
            qWarning("Method not found in interface");
            return;
        }

        QtConcurrent::run([=]() {
            QVariantList requestArgs = doc.object().value("params").toArray().toVariantList();
            QList<QGenericArgument> args;

            for (int i = 0; i < requestArgs.size(); i++) {
                const QVariant &v = requestArgs.at(i);
                QGenericArgument arg(QMetaType::typeName(v.userType()), v.data());
                args.append(arg);
            }

            QVariant r;
            metaObject->method(methodIndex).invoke(interface,
                                                   Qt::DirectConnection,
                                                   Q_RETURN_ARG(QVariant, r),
                                                   args.value(0, QGenericArgument()),
                                                   args.value(1, QGenericArgument()),
                                                   args.value(2, QGenericArgument()),
                                                   args.value(3, QGenericArgument()),
                                                   args.value(4, QGenericArgument()),
                                                   args.value(5, QGenericArgument()),
                                                   args.value(6, QGenericArgument()),
                                                   args.value(7, QGenericArgument()),
                                                   args.value(8, QGenericArgument()),
                                                   args.value(9, QGenericArgument()));

            QVariantMap map;
            map["jsonrpc"] = "2.0";
            map["result"] = r;
            map["id"] = doc.object().value("id").toInt();

            _socket->send(QJsonDocument::fromVariant(map).toJson(QJsonDocument::Compact));
        });
    } else if (doc.object().contains("result")) {
        if (!doc.object().contains("id")) {
            qWarning("Invalid result");
            return;
        }

        int id = doc.object().value("id").toInt();

        if (_callbacks.contains(id)) {
            QtConcurrent::run(_callbacks[id], doc.object().value("result").toVariant());
        }
    }
}
