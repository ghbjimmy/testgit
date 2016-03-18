#ifndef STATEMACHINEMGR_H
#define STATEMACHINEMGR_H

#include <QObject>
#include "const.h"

class StateMachineRpc;
class ZmqCfgParser;

class StateMachineMgr : public QObject
{
    Q_OBJECT

public:
    StateMachineMgr();
    ~StateMachineMgr();

    bool initByCfg(ZmqCfgParser* cfg);
    bool startAll();
    void stopAll();

signals:
    void smIsAliveSignal(int id, bool isAlive, bool isShow);

private:
    StateMachineRpc* _sms[SM_NUM]; //SM_NUM默认1
};

#endif // TESTENGINEMGR_H
