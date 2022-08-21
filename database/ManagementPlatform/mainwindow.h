#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLabel>

#include<QtSql>
#include<QDataWidgetMapper>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QSqlDatabase DB; //建立数据库连接
    QSqlTableModel  *tabModel;  //用于指定某一个数据表，作为数据表的数据模型

    QItemSelectionModel *theSelection; //选择模型，zaitabModel选择的字段发生变化、当前记录发生变化时发射信号

    QDataWidgetMapper   *dataMapper; //数据映射

    void init();
    void openTable();
    void refreshMainwindow();
private slots:
    void currentRowchangedslots(const QModelIndex &current, const QModelIndex &previous);
    void currentChangedslots(const QModelIndex &current, const QModelIndex &previous);



    void on_actSave_triggered();

    void on_actCancel_triggered();

    void on_actDelete_triggered();

    void on_actNext_triggered();

    void on_actInsert_triggered();

    void on_actPre_triggered();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
