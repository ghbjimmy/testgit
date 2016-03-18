#include "mainwindow.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QSplitter>
#include <QLibrary>
#include <QMenuBar>
#include <QFileDialog>
#include <QApplication>

#include "libaryparser.h"
#include "pluginSubjecter.h"
#include "util.h"
#include "const.h"
#include "message.h"
#include "zmq.h"
#include "qlog.h"

#include "command.h"
#include "sequencermgr.h"
#include "zmqcfgparser.h"
#include "testenginemgr.h"
#include "statemachinemgr.h"

#include <thread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

static void sendMessageCallBack(const IMessage* msg)
{
    MainWindow::getInstance()->dispatchMessage(msg);
}

MainWindow* MainWindow::_instance = NULL;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    _pluginSubjecter = new PluginSubjecter();

    bool flag = false;
    QString path = "D:\\Work\\tm_platform_new\\source\\UI\\bin\\PlugIns\\DetailViewPlugin\\debug\\DetailViewPlugin.dll";
    flag = loadLibary(path);

    path = "D:\\Work\\tm_platform_new\\source\\UI\\bin\\PlugIns\\ScopeViewPlugin\\debug\\ScopeViewPlugin.dll";
    flag = loadLibary(path);

    path = "D:\\Work\\tm_platform_new\\source\\UI\\bin\\PlugIns\\InteractionViewPlugin\\debug\\InteractionViewPlugin.dll";
    flag = loadLibary(path);

    setupUI();
    _instance = this;
}

MainWindow::~MainWindow()
{
    if (_pluginSubjecter != NULL)
    {
        delete _pluginSubjecter;
        _pluginSubjecter = NULL;
    }

    foreach (LibaryParser* parser, _libParsers)
    {
        delete parser;
        parser = NULL;
    }

    _libParsers.clear();
}

MainWindow* MainWindow::getInstance()
{
    return _instance;
}

bool MainWindow::init()
{
    _zmqCfgParse = new ZmqCfgParser();
    if (!_zmqCfgParse->parse("D:\\Work\\tm_platform_v2_1\\LuaDriver\\Driver\\config\\zmqports.json"))
    {
        LogMsg(Error, "parse config file failed.");
        return false;
    }

    _sequencerMgr = new SequencerMgr();
    if (!_sequencerMgr->initByCfg(_zmqCfgParse))
    {
        LogMsg(Error, "init by  config file failed.");
        return false;
    }

    if (!_sequencerMgr->startAll())
    {
        LogMsg(Error, "start all sequencer failed.");
        return false;
    }

    connect(_sequencerMgr, SIGNAL(isAliveSignal(int,bool,bool)), this, SLOT(onSeqIsAlive(int,bool,bool)));
    connect(_sequencerMgr, SIGNAL(itemStartSignal(int,const QString&)), this, SLOT(onItemStart(int,const QString&)));
    connect(_sequencerMgr, SIGNAL(itemEndSignal(int,const QString&)), this, SLOT(onItemEnd(int,const QString&)));

    _engineMgr = new TestEngineMgr();
    if (!_engineMgr->initByCfg(_zmqCfgParse))
    {
        LogMsg(Error, "init by  config file failed.");
        return false;
    }

    if (!_engineMgr->startAll())
    {
        LogMsg(Error, "start all enginer failed.");
        return false;
    }

    connect(_engineMgr, SIGNAL(isAliveSignal(int,bool,bool)), this, SLOT(onEngIsAlive(int,bool,bool)));

    _smMgr = new StateMachineMgr();
    if (!_smMgr->initByCfg(_zmqCfgParse))
    {
        LogMsg(Error, "init by  config file failed.");
        return false;
    }

    if (!_smMgr->startAll())
    {
        LogMsg(Error, "start all enginer failed.");
        return false;
    }

    connect(_smMgr, SIGNAL(isAliveSignal(int,bool,bool)), this, SLOT(onSmIsAlive(int,bool,bool)));
    return true;
}

bool MainWindow::loadLibary(const QString& path)
{
    LibaryParser* parser = new LibaryParser();
    if (!parser->parse(path))
        return false;

    IPlugin * pPlugin = parser->getPlugin();
    if (0 != pPlugin->init())
    {
        delete parser;
        parser = NULL;
        return false;
    }

    pPlugin->registerSendMsgCallBack(sendMessageCallBack);
    _libParsers.append(parser);
    _pluginSubjecter->attach(pPlugin);

    return true;
}

QWidget* createTitleWgt()
{
    QLabel* nameLabel = new QLabel();
    nameLabel->setText("Calibration");
    UIUtil::setFontSize(nameLabel, 30, true);
    UIUtil::setTextColor(nameLabel, Qt::blue);
    QLabel* imgLbl = new QLabel();
    QPixmap pix= QPixmap(":/Resources/image001.png");
    imgLbl->setPixmap(pix);

    QLabel* text = new QLabel("1.29");

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(imgLbl);
    QHBoxLayout* h1 = new QHBoxLayout();
    //h1->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    h1->addStretch(1);
    h1->addWidget(text);
    v1->addLayout(h1);

    QHBoxLayout* h2 = new QHBoxLayout();
    h2->addWidget(nameLabel);
    h2->addStretch(1);
    h2->addLayout(v1);
    h2->addSpacerItem(new QSpacerItem(40,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QWidget* titleWgt = new QWidget();

    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addLayout(h2);
    titleWgt->setLayout(v2);

    UIUtil::setBgColor(titleWgt, Qt::gray);
    return titleWgt;
}

QWidget* MainWindow::createDetailViewWgt()
{
    QLabel* lbl = new QLabel();
    lbl->setFixedHeight(10);
    lbl->setText("V1.0");
    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addStretch(1);
    h1->addWidget(lbl);
    h1->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QWidget* viewWgt = NULL;
    IPlugin * pPlugin = _pluginSubjecter->getPlugin(DetailViewPluginName);
    if (pPlugin == NULL)
        viewWgt = new QWidget();
    else
        viewWgt = pPlugin->createWidget();

    QWidget* wgt = new QWidget();
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addLayout(h1);
    v1->addWidget(viewWgt);
    wgt->setLayout(v1);
    v1->setSpacing(1);
    v1->setContentsMargins(QMargins(0,0,0,0));
    return wgt;
}

QWidget* MainWindow::createScopeViewWgt()
{
    QWidget* viewWgt = NULL;
    IPlugin * pPlugin = _pluginSubjecter->getPlugin(ScopeViewPluginName);
    if (pPlugin == NULL)
        viewWgt = new QWidget();
    else
        viewWgt = pPlugin->createWidget();

    return viewWgt;
}

QWidget* MainWindow::createInteractionViewWgt()
{
    QWidget* viewWgt = NULL;
    IPlugin * pPlugin = _pluginSubjecter->getPlugin(InteractionViewPluginName);
    if (pPlugin == NULL)
        viewWgt = new QWidget();
    else
        viewWgt = pPlugin->createWidget();

    return viewWgt;
}

QLabel* createNumLabel(int num)
{
    QLabel* numlbl = new QLabel(QString::number(num));
    UIUtil::setBgColor(numlbl, QColor(0,255,0));

    numlbl->setAlignment(Qt::AlignCenter);
    numlbl->setFixedSize(16, 16);
    numlbl->setAutoFillBackground(true);

    return numlbl;
}

QWidget* MainWindow::createStatusWgt()
{
    QWidget* statusWgt = new QWidget();
    QHBoxLayout* h1 = new QHBoxLayout();
    QLabel* seqLbl = new QLabel("Sequencer:");
    h1->addWidget(seqLbl);

    for (int i = 0; i < 6; ++i)
    {
        _seqLbl[i] = createNumLabel(i);
        h1->addWidget(_seqLbl[i]);
    }

    h1->addStretch(1);
    h1->setContentsMargins(QMargins(0,0,0,0));


    QHBoxLayout* h2 = new QHBoxLayout();
    QLabel* engineLbl = new QLabel("Engine:");
    h2->addWidget(engineLbl);
    for (int i = 0; i < 6; ++i)
    {
        _engineLbl[i] = createNumLabel(i);
        h2->addWidget(_engineLbl[i]);
    }

    h2->addStretch(1);
    h2->setContentsMargins(QMargins(0,0,0,0));


    QLabel* machineLbl = new QLabel("StateMachine:");
    QLabel* numLbl = createNumLabel(0);

    QHBoxLayout* h3 = new QHBoxLayout();
    h3->addWidget(machineLbl);
    h3->addWidget(numLbl);
    h3->addStretch(1);
    h3->setContentsMargins(QMargins(0,0,0,0));

    QHBoxLayout* h4 = new QHBoxLayout();
    h4->addLayout(h1);
    h4->addSpacerItem(new QSpacerItem(20,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    h4->addLayout(h2);
    h4->addSpacerItem(new QSpacerItem(20,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    h4->addLayout(h3);
    h4->addStretch(1);
    h4->setContentsMargins(QMargins(0,0,0,0));

    statusWgt->setLayout(h4);

    return statusWgt;
}


void MainWindow::setupUI()
{
    mTitleWgt = createTitleWgt();
    mTitleWgt->setFixedHeight(60);
    QWidget* mDetailViewWgt = createDetailViewWgt();
    QWidget* mScopeViewWgt = createScopeViewWgt();
    QWidget* statWidget = createStatusWgt();
    QVBoxLayout* v11 = new QVBoxLayout();
    v11->addWidget(mScopeViewWgt, 1);
    v11->addWidget(statWidget);
    //v11->setContentsMargins(0,0,0,0);

    QWidget* downWidget = new QWidget();
    downWidget->setLayout(v11);

    QWidget* mInteractionViewWgt = createInteractionViewWgt();
    mInteractionViewWgt->setFixedWidth(250);

    QSplitter* split = new QSplitter(Qt::Orientation::Vertical);
    split->addWidget(mDetailViewWgt);
    split->addWidget(downWidget);

    split->setStretchFactor(0, 1);

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(split,3);
    h1->addWidget(mInteractionViewWgt,1);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(mTitleWgt);
    v1->addLayout(h1);

    v1->setContentsMargins(QMargins(0,0,0,0));
    QWidget* centralWgt = new QWidget();
    centralWgt->setLayout(v1);
    this->setCentralWidget(centralWgt);

    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    QAction* loadAction = new QAction("Load CSV",this);
    QAction* loadScopeAction = new QAction("Load ScopeView",this);
    connect(loadAction,SIGNAL(triggered()),this,SLOT(onMenuAction()));
    connect(loadScopeAction,SIGNAL(triggered()),this,SLOT(onMenuAction()));
    fileMenu->addAction(loadAction);
    fileMenu->addAction(loadScopeAction);
    this->resize(1024,768);
}

void MainWindow::onMenuAction()
{
    QAction* action = static_cast<QAction*>(sender());
    if (NULL == action)
        return;
    if (action->text() == "Load CSV")
    {
        QString path = QFileDialog::getOpenFileName(this, tr("Open CSV File"), ".", tr("CSV Files(*.csv)"));
       // if(path.length() == 0)
       //     return;

        //load 命令
        QVector<int> failVecs = _sequencerMgr->loadProfile("/Users/mac/Desktop/test_plan__0225_12h_optical_fct_only.csv");
        if (!failVecs.empty())
        {
            LogMsg(Error, "load profile failed. failed count is %d", failVecs.size());
            //return;
        }

        //list 命令
        ListCsvFileMsg* listCsvMsg = new ListCsvFileMsg();
        QVector<QString> items;
        if (!_sequencerMgr->getCsvContent(items))
        {
            LogMsg(Error, "get content failed. %d");
            delete listCsvMsg;
            return;
        }
        listCsvMsg->setData(items);
        //发送结果到插件
        dispatchMessage(listCsvMsg);
        delete listCsvMsg;

    }
    else if (action->text() == "Load ScopeView")
    {
        LoadScopeViewMsg* loadScopeViewMsg = new LoadScopeViewMsg();
        dispatchMessage(loadScopeViewMsg);
    }

}

void MainWindow::dispatchMessage(const IMessage* msg)
{
    _pluginSubjecter->notity(msg);
}


void MainWindow::onSeqIsAlive(int index, bool isAlive, bool isShow)
{
    if (isAlive)
    {
        if (!isShow)
        {
            _seqLbl[index]->setText(QString::number(index));
            UIUtil::setBgColor(_seqLbl[index], Qt::green);

        }
        else
        {
            _seqLbl[index]->setText("");
            UIUtil::setBgColor(_seqLbl[index], Qt::gray);
        }

        LogMsg(Debug, "sequence %d is alvie : %d", index, isShow);
    }
    else
    {
        if (_seqLbl[index]->palette().background().color() != Qt::red)
        {
            _seqLbl[index]->setText(" ");
            UIUtil::setBgColor(_seqLbl[index], Qt::red);

            this->update();
            _seqLbl[index]->setText(QString::number(index));
        }
    }

    this->update();
   // QApplication::processEvents();
}

void MainWindow::onEngIsAlive(int index, bool isAlive, bool isShow)
{
    if (isAlive)
    {
        if (!isShow)
        {
            _engineLbl[index]->setText(QString::number(index));
            UIUtil::setBgColor(_engineLbl[index], Qt::green);

        }
        else
        {
            _engineLbl[index]->setText("");
            UIUtil::setBgColor(_seqLbl[index], Qt::gray);
        }

        LogMsg(Debug, "engine %d is alvie : %d", index, isShow);
    }
    else
    {
        if (_engineLbl[index]->palette().background().color() != Qt::red)
        {
            _engineLbl[index]->setText(" ");
            UIUtil::setBgColor(_engineLbl[index], Qt::red);

            this->update();
            _engineLbl[index]->setText(QString::number(index));
        }
    }

    this->update();
}

void MainWindow::onSmIsAlive(int index, bool isAlive, bool isShow)
{

}

void MainWindow::onItemStart(int index, const QString& itemJson)
{
    //item_start
    ProcItemStateMsg* procItemStateMsg = new ProcItemStateMsg();
    procItemStateMsg->setData(true, index, itemJson);

    //发送结果到插件
    dispatchMessage(procItemStateMsg);
    delete procItemStateMsg;
}

void MainWindow::onItemEnd(int index, const QString& itemJson)
{
    //item_start
    ProcItemStateMsg* procItemStateMsg = new ProcItemStateMsg();
    procItemStateMsg->setData(false, index, itemJson);

    //发送结果到插件
    dispatchMessage(procItemStateMsg);
    delete procItemStateMsg;
}
