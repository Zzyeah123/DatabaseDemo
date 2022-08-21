#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QMessageBox>
#include<QtSql>

void MainWindow::openTable()
{
    tabModel = new QSqlTableModel(this,DB); //
    tabModel->setTable("patientInfo");
    tabModel->setEditStrategy(QSqlTableModel::OnManualSubmit); //数据保存方式

    if(!(tabModel->select())){
        QMessageBox::critical(this,"错误信息","打开数据表错误，错误信息\n"+tabModel->lastError().text(),
                              QMessageBox::Ok, QMessageBox::NoButton);
    }

    tabModel->setHeaderData(tabModel->fieldIndex("name"),Qt::Horizontal,"姓名");
    tabModel->setHeaderData(tabModel->fieldIndex("birth"),Qt::Horizontal,"出生日期");
    tabModel->setHeaderData(tabModel->fieldIndex("sex"),Qt::Horizontal,"性别");
    tabModel->setHeaderData(tabModel->fieldIndex("patientid"),Qt::Horizontal,"病人编号");
    tabModel->setHeaderData(tabModel->fieldIndex("room"),Qt::Horizontal,"病房号");

    theSelection = new QItemSelectionModel(tabModel); // 关联选择模型
    //选择当前项变化
    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(currentChangedslots(QModelIndex,QModelIndex)));
    //选择行变化
    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(currentRowchangedslots(QModelIndex,QModelIndex)));

    //设置tableView的关联的模型
    ui->tableView->setModel(tabModel);
    ui->tableView->setSelectionModel(theSelection);

    //创建数据映射
    dataMapper = new QDataWidgetMapper();
    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    dataMapper->addMapping(ui->lineEditName,0); //姓名联系
    dataMapper->addMapping(ui->lineEditBirth,1);
    dataMapper->addMapping(ui->lineEditSex,2);
    dataMapper->addMapping(ui->lineEditId,3);
    dataMapper->addMapping(ui->lineEditDep,4);

    dataMapper->toFirst();

    //更新ui界面组件使能状态
    ui->actSave->setEnabled(true);
    ui->actCancel->setEnabled(true);
    ui->actCheck->setEnabled(true);
    ui->actDelete->setEnabled(true);
    ui->actInsert->setEnabled(true);
    ui->actNext->setEnabled(true);
    ui->actPageDown->setEnabled(true);
    ui->actPageUp->setEnabled(true);
    ui->actPre->setEnabled(true);

}

void MainWindow::init() //创建两个表，一个存储病人的基本信息，另一个与病人的各项身体指标数据相关
{
    QSqlQuery sql_query;
    QString initStr1 = "create table patientInfo(name varachar, birth data, sex varchar, patientid varchar, room varchar)";
    QString initStr2 = "create table patient(patientid varchar)";
    sql_query = DB.exec("DROP TABLE patientInfo");
    sql_query = DB.exec("drop table patient");
    qDebug()<<"drop table petient";
    if(!sql_query.exec(initStr1)){
        qDebug()<<"Error: failed to create table patientInfo!"<<sql_query.lastError();
    }else{
        qDebug()<<"table patientInfo created successfully";
    }
    if(!sql_query.exec(initStr2)){
        qDebug()<<"Error: failed to create table patient!"<<sql_query.lastError();
    }else{
        qDebug()<<"table patient created successfully";
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->centralWidget);

    //
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);

    DB = QSqlDatabase::addDatabase("QSQLITE");
    //qDebug()<<QApplication::applicationDirPath(); //当前的目录
    DB.setDatabaseName("test.sqlite3");

    ui->comboBox->addItem("按姓名搜索");
    ui->comboBox->addItem("按病人编号搜索");
    ui->comboBox->addItem("按病房号搜索");

}

void MainWindow::currentChangedslots(const QModelIndex &current, const QModelIndex &previous)
{
    ui->actSave->setEnabled(tabModel->isDirty()); //有未保存修改时可用，说明数据被动过了，save按钮打开
    ui->actCancel->setEnabled(tabModel->isDirty());
}

void MainWindow::currentRowchangedslots(const QModelIndex &current, const QModelIndex &previous)
{
    ui->actDelete->setEnabled(current.isValid());

    if(!current.isValid()){
        ui->lineEditBirth->clear();
        ui->lineEditName->clear();
        ui->lineEditSex->clear();
        ui->lineEditId->clear();
        ui->lineEditDep->clear();
    }

    dataMapper->setCurrentIndex(current.row());

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actSave_triggered()
{
    bool ret = tabModel->submitAll();
    if(!ret){
        QMessageBox::information(this, "提示", "数据保存出错，错误信息\n"+tabModel->lastError().text(),QMessageBox::Ok,QMessageBox::NoButton);
    }else{
        ui->actSave->setEnabled(false);
        ui->actCancel->setEnabled(false);
    }
}

void MainWindow::on_actCancel_triggered()
{
    tabModel->revertAll();
    ui->actSave->setEnabled(false);
    ui->actCancel->setEnabled(false);
}

void MainWindow::on_actDelete_triggered()  // wait to confirmed
{
    int currentRowNo = theSelection->currentIndex().row();
    QSqlRecord currentRec = tabModel->record(currentRowNo);//得到当前记录
    if(currentRec.isEmpty())
        return;

    QString patientId = currentRec.value("patientid").toString();
    QSqlQuery query;
    query.prepare("delete from patientInfo where patientid = :ID");
    query.bindValue(":ID",patientId);

    if(!query.exec()){
        QMessageBox::critical(this,"出错","删除数据出现错误\n"+query.lastError().text(),QMessageBox::Ok,QMessageBox::NoButton);
    }
    else{
        QString str = "select * from patientInfo";
        tabModel->select();
    }
}

void MainWindow::on_actNext_triggered() //后一条记录 wait to confirmed
{
    dataMapper->toNext();
    refreshMainwindow();
}

void MainWindow::on_actInsert_triggered() //wait to confirmed
{
    QSqlQuery query;
    QString str;
    QByteArray ba;
    query.exec("select * from patientInfo where name = """); //不查询出实际记录

    QSqlRecord currentRecord = query.record(); //获取当前记录 实际为空记录

    query.prepare("insert into patientInfo (name,birth,sex,patientid,department) values (:name,:birth,:sex,:patient,:room)");
    str = ui->lineEditName->text(); ba = str.toLatin1();
    query.bindValue(":name",ba.data());
    str = ui->lineEditBirth->text(); ba = str.toLatin1();
    query.bindValue(":birth",ba.data());
    str = ui->lineEditSex->text(); ba = str.toLatin1();
    query.bindValue(":sex",ba.data());
    str = ui->lineEditId->text(); ba = str.toLatin1();
    query.bindValue(":patientid",ba.data());
    str = ui->lineEditDep->text(); ba = str.toLatin1();
    query.bindValue(":room",ba.data());

    if(!query.exec()){
        QMessageBox::critical(this,"出错","插入数据错误\n"+query.lastError().text(),QMessageBox::Ok,QMessageBox::NoButton);
    }else{ //插入成功后刷新view界面
        QString s = "select * from patientInfo";
        tabModel->select();
    }


}

void MainWindow::refreshMainwindow()
{
    int index = dataMapper->currentIndex();
    QModelIndex currentIndex = tabModel->index(index,1);
    theSelection->clearSelection();
    theSelection->setCurrentIndex(currentIndex,QItemSelectionModel::Select); //设置刚插入的行为当前选择行

}


void MainWindow::on_actPre_triggered()
{
    dataMapper->toPrevious();
    refreshMainwindow();
}
