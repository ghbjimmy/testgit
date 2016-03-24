#ifndef SEQUENCERMGR_H
#define SEQUENCERMGR_H

#include <QVector>
#include <QObject>
#include "const.h"

#include "structdefine.h"

class SequencerRpc;
class ZmqCfgParser;

//sequencer 管理器
class SequencerMgr : public QObject
{
    Q_OBJECT

public:
    SequencerMgr();
    ~SequencerMgr();

    bool initByCfg(ZmqCfgParser* cfg);
    bool startAll();
    void stopAll();

    //加载csv
    QVector<int> loadProfile(const QString& csvFilePath);

    //获取csv内存
    bool getCsvContent(QVector<QString>& items);


signals:
    void isAliveSignal(int id, bool isAlive, bool isShow);
    void itemStartSignal(int index, const QString& item);
    void itemEndSignal(int index, const QString& item);

private:
    SequencerRpc* _sequencers[SEQ_NUM];
};

#endif // SEQUENCERMGR_H
