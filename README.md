# libjsonrpcwebsocketserver
Bidirectional Qt JSON-RPC library over WebSocket

# Installing

You obviously need Qt5 to build the library. There are two ways to install the library, see as follow:

## Installing into system

You can install the library into your linux system running the following:

```shell
qmake PREFIX=/usr
make
make install
```

If you are using a qmake project add to your project file:

```
CONFIG += link_pkgconfig
PKGCONFIG += libjsonrpcwebsocketserver
```

## Embedding into your project

To embeded the library into your project without installing into the system, add the following to your qmake project file:

```
include(path/to/libjsonrpcwebsocketserver.pri)
```

# API

## JsonRPCWebSocketServer

### call(interface, method, args, callback)

Call a remote method.

Parameter | Type               | Description
--------- | ------------------ | -----------
interface | QString            | Interface name
method    | QString            | Method name
callback  | function(QVariant) | callback to be used to handle the response

Example:

```c++
JsonRPCWebSocketServer rpc;
rpc.connect("ws://localhost:80");
rpc.call("TestInterface", "testMethod", [] (QVariant result) {
    // result is the value returned by the remote method
});
```

### registerInterface(name, interface)

Register an interface in remote server.

Parameter | Type      | Description
--------- | --------- | -----------
name      | QString   | Interface name
interface | QObject * | QObject instance

Example:

```c++
class MyInterface: public QObject
{
    Q_OBJECT

public slots:
    int myMethod(x, y)
    {
        return x + y;
    }
};

MyInterface *myInterface = new MyInterface();

JsonRPCWebSocketServer rpc;
rpc.connect("ws://localhost:80");
rpc.registerInterface("MyInterface", myInterface);
```
