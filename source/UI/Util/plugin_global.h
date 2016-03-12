﻿#ifndef TPLUGIN_GLOBAL_H
#define TPLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QWidget>

#if defined(TPLUGIN_LIBRARY)
#  define TPLUGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TPLUGINSHARED_EXPORT Q_DECL_IMPORT
#endif


const int PLUGIN_INIT = 0;       // 初始化插件
const int PLUGIN_FINI = 1;      // 插件释放
const int PLUGIN_GUI  = 10;      // 插件GUI相关消息

class TPLUGINSHARED_EXPORT IMessage
{
public:
    virtual int  messageID() const = 0;
    virtual void * context() const = 0;
    virtual int    length() const = 0;
};

class TPLUGINSHARED_EXPORT IPlugin
{
public:
    IPlugin() {_name = "unknown";}
    virtual ~IPlugin(){}
    virtual const QString& getName() const { return _name; }
    virtual int init() = 0;
    virtual void fini()  = 0;
    virtual int onMessage(const IMessage* msg) = 0;
    virtual bool isHandleMessage(const IMessage* msg) = 0;
    virtual QWidget * createWidget() = 0;

protected:
    QString _name;
};

typedef IPlugin * (*fnCreatePlugin)();

extern "C" TPLUGINSHARED_EXPORT IPlugin * createPlugin();

#endif // TPLUGIN_GLOBAL_H