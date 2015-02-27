# libjsonrpcwebsocketserver
Qt JSON-RPC library over WebSocket

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
