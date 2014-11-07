TEMPLATE = lib
QT += core
CONFIG += c++11

TARGET = jsonrpcwebsocketserver

SOURCES += jsonrpcwebsocketserver.cpp
HEADERS += jsonrpcwebsocketserver.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += qwebsocket
unix: target.path = $$PREFIX/lib

INSTALLS += target
