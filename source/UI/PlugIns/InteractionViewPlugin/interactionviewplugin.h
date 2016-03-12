#ifndef INTERACTIONVIEWPLUGIN_H
#define INTERACTIONVIEWPLUGIN_H

#include "../../Util/plugin_global.h"

class TPLUGINSHARED_EXPORT InteractionViewPlugin : public IPlugin
{

public:
    InteractionViewPlugin();


public:
    virtual int init();
    virtual void fini();
    virtual int onMessage(const IMessage* msg);
    virtual bool isHandleMessage(const IMessage* msg);
    virtual QWidget * createWidget();

private:
    QWidget* _widget;
};

#endif // INTERACTIONVIEWPLUGIN_H
