#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QMessageBox>
#include<QtSql>
#include<QVector>

void MainWindow::openTable()
{
    queryModel = new QSqlQueryModel(this); //连接model组件

    theSelection = new QItemSelectionModel(queryModel); // 关联选择模型

    queryModel->setQuery("select * from patientInfo"); //先查看
    if(queryModel->lastError().isValid()){
        QMessageBox::critical(this,"错误信息","打开数据表错误，错误信息\n"+queryModel->lastError().text(),
                              QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    //设置字段显示名
    queryModel->setHeaderData(0,Qt::Horizontal,"姓名");
    queryModel->setHeaderData(1,Qt::Horizontal,"出生日期");
    queryModel->setHeaderData(2,Qt::Horizontal,"性别");
    queryModel->setHeaderData(3,Qt::Horizontal,"病人编号");
    queryModel->setHeaderData(4,Qt::Horizontal,"病房号");


    //设置tableView的关联的模型
    ui->tableView->setModel(queryModel);
    ui->tableView->setSelectionModel(theSelection);


    //更新ui界面组件使能状态
    ui->actCheck->setEnabled(true);
    ui->actDelete->setEnabled(true);
    ui->actInsert->setEnabled(true);

}

void MainWindow::init() //创建表，做一些关于数据库的初始化操作
{
    QSqlQuery sql_query;
    QString initStr1 = "create table patientInfo(name varachar, birth data, sex varchar, patientid varchar, room varchar)";
    QString initStr2 = "create table patient(patientid varchar)";
    QString initStr3 = "create table equipment(patientid varchar, equipmentid varchar)";
    QString initStr4 = "create table patientData(patientid varchar, quipmentid varchar, time data , data float)";
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
    //对tableView做一些初始化设置，特别是选择的行为类别
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);
    //添加数据库
    DB = QSqlDatabase::addDatabase("QSQLITE");
    DB.setDatabaseName("ManagementPlatform.sqlite3");
    if(!DB.open()){
        QMessageBox::critical(this,"错误信息","打开数据库错误，错误信息\n",
                              QMessageBox::Ok, QMessageBox::NoButton);
    }
    //设置combobox
    ui->comboBox->addItem("按姓名搜索");
    ui->comboBox->addItem("按病人编号搜索");
    ui->comboBox->addItem("按病房号搜索");
    //调用两个辅助初始化的函数
    init();
    openTable();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actDelete_triggered()
{
    int currentRowNo = theSelection->currentIndex().row();
    QSqlRecord currentRec = queryModel->record(currentRowNo);//得到当前记录
    if(currentRec.isEmpty())
        return;

    QString patientId = currentRec.value("patientid").toString();
    QSqlQuery query;
    //qDebug()<<patientId;
    query.prepare("delete from patientInfo where patientid = :ID"); //准备执行删除操作
    query.bindValue(":ID",patientId);

    if(!query.exec()){
        QMessageBox::critical(this,"出错","删除数据出现错误\n"+query.lastError().text(),QMessageBox::Ok,QMessageBox::NoButton);
    }
    else{
        QString str = "select * from patientInfo";//删除完了以后执行一次select，起到刷新的作用
        queryModel->query().exec();
    }
}


void MainWindow::on_actInsert_triggered()
{
    QSqlQuery query;
    QString str;
    QByteArray ba;
    query.exec("select * from patientInfo where name = """); //不查询出实际记录，对界面的一个刷新

    query.prepare("insert into patientInfo (name,birth,sex,patientid,room) values (:name,:birth,:sex,:patientid,:room)");
    //逐个对sql语句进行赋值，然后直接执行
    str = ui->lineEditName->text();
    query.bindValue(":name",str);
    str = ui->lineEditBirth->text();
    query.bindValue(":birth",str);
    str = ui->lineEditSex->text();
    query.bindValue(":sex",str);
    str = ui->lineEditId->text();
    query.bindValue(":patientid",str);
    str = ui->lineEditDep->text();
    query.bindValue(":room",str);

    if(!query.exec()){
        QMessageBox::critical(this,"出错","插入数据错误\n"+query.lastError().text(),QMessageBox::Ok,QMessageBox::NoButton);
    }else{ //插入成功后刷新view界面
        QString s = "select * from patientInfo";
        queryModel->setQuery("select * from patientInfo");
    }
    //insert完成后，清空lineEdit等待下一次插入数据
    ui->lineEditName->clear();
    ui->lineEditBirth->clear();
    ui->lineEditSex->clear();
    ui->lineEditId->clear();
    ui->lineEditDep->clear();


}

void MainWindow::on_pushButton_clicked()
{
    int index = ui->comboBox->currentIndex();
    QString str = ui->lineEditSearch->text();
    QString sql;
    QSqlQuery query;
    //根据conbobox的选择对病人信息进行选择查询
    if(index == 0 ){
         sql = "name";
         query.prepare("select * from patientInfo where name = :name");
         query.bindValue(":name",str);
         queryModel->setQuery(QString("select * from patientInfo where name = \'%1\'").arg(str));
         return;
    }

   if(index == 1){
       query.prepare("select * from patientInfo where patientid = :patientid");
       query.bindValue(":patientid",str);
       queryModel->setQuery(QString("select * from patientInfo where patientid = \'%1\'").arg(str));
       return;
   }
    query.prepare("select * from patientInfo where room = :room");
    query.bindValue(":room",str);
    //query.exec();
    queryModel->setQuery(query);
}

void MainWindow::on_actEdit_triggered()
{//此函数用来“准备修改数据”，把选择的数据行映射到lineEdit中等待修改
    int n = theSelection->currentIndex().row();
    QSqlRecord currentRec = queryModel->record(n);

    ui->lineEditName->setText(currentRec.value("name").toString());
    ui->lineEditId->setText(currentRec.value("patientid").toString());
    ui->lineEditSex->setText(currentRec.value("sex").toString());
    ui->lineEditBirth->setText(currentRec.value("birth").toString());
    ui->lineEditDep->setText(currentRec.value("room").toString());
}

void MainWindow::on_actConfirm_triggered()
{ //此函数相当于把lineEdit里修改过后的数据保存到数据库中，以id作为主键
    QSqlQuery query; QSqlRecord currentRec = queryModel->record(theSelection->currentIndex().row());
    query.prepare("update patientInfo set name=:name, birth=:birth, sex=:sex, room=:room where patientid = :patientid");
    query.bindValue(":name",ui->lineEditName->text());
    query.bindValue(":birth",ui->lineEditBirth->text());
    query.bindValue(":sex",ui->lineEditSex->text());
    query.bindValue(":room",ui->lineEditDep->text());
    query.bindValue(":patientid",ui->lineEditId->text());
    qDebug()<<currentRec.value("patientid").toString();
    if (!query.exec())
        QMessageBox::critical(this, "错误", "数据更新错误\n"+query.lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);
    else
        queryModel->query().exec();//数据模型重新查询数据，更新tableView显示
}


void MainWindow::on_actCheck_triggered()
{//查看所有数据
    queryModel->setQuery("select * from patientInfo");
}

void MainWindow::on_actAdd_triggered()
{
    int n = theSelection->currentIndex().row();
    QSqlRecord currentRec = queryModel->record(n);
    QString id = currentRec.value("patientid").toString();
    CheckList.append(id);
    if(ui->add1->text() == ""){
        ui->add1->setText(id);
        ui->radio1->setChecked(true);
    }
    else if(ui->add2->text() == ""){
        ui->add2->setText(id);
        ui->radio2->setChecked(true);
    }
    else if(ui->add3->text() == ""){
        ui->add3->setText(id);
        ui->radio3->setChecked(true);
    }
}
void MainWindow::on_radio1_clicked()
{
    if(ui->radio1->isChecked()){
        ui->radio1->setChecked(false);
        foreach(QString s , CheckList){
            if(s == ui->add1->text()){
                CheckList.remove(CheckList.indexOf(s));
                break;
            }
        }
        ui->add1->setText("");
    }
}

void MainWindow::on_radio2_clicked()
{
    if(ui->radio2->isChecked()){
        ui->radio2->setChecked(false);
        foreach(QString s , CheckList){
            if(s == ui->add2->text()){
                CheckList.remove(CheckList.indexOf(s));
                break;
            }
        }
        ui->add2->setText("");
    }
}

void MainWindow::on_radio3_clicked()
{
    if(ui->radio3->isChecked()){
        ui->radio3->setChecked(false);
        foreach(QString s , CheckList){
            if(s == ui->add3->text()){
                CheckList.remove(CheckList.indexOf(s));
                break;
            }
        }
        ui->add3->setText("");
    }
}
