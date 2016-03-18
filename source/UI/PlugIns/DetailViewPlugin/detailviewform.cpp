#include "detailviewform.h"
#include <QHBoxLayout>
#include <QLabel>
#include "cvsdatatreeview.h"
#include "progressform.h"
#include "failform.h"

DetailViewForm::DetailViewForm(QWidget *parent) : QTabWidget(parent)
{
    CVSDataTreeView* view = new CVSDataTreeView();
    setupUI();
}

DetailViewForm::~DetailViewForm()
{

}

void DetailViewForm::setupUI()
{
    _csvTreeView = new CVSDataTreeView();
    FailForm* failForm = new FailForm();

    ProgressForm* progressForm = new ProgressForm();
    this->addTab(_csvTreeView ,QString("Detail"));
    this->addTab(progressForm,QString("Progress"));
    this->addTab(failForm,QString("Fail Only"));
}

void DetailViewForm::listCsvData(const QVector<QString>& datas)
{
    _csvTreeView->setData(datas);
}

void DetailViewForm::procItemStart(int index, const QString& data)
{
    _csvTreeView->procItemStart(index, data);
}

void DetailViewForm::procItemEnd(int index, const QString& data)
{
    _csvTreeView->procItemEnd(index, data);
}
