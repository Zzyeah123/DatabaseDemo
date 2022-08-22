#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLabel>
#include<QVector>
#include<QtSql>
#include<QDataWidgetMapper>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QVector<QString> CheckList;
private:
    QSqlDatabase DB; //建立数据库连接
    QSqlQueryModel *queryModel;//数据表的数据模型

    QItemSelectionModel *theSelection; //选择模型，在选择的字段发生变化、当前记录发生变化时发射信号

    QDataWidgetMapper   *dataMapper; //数据映射，可能没用

    void init();
    void openTable();
    void refreshMainwindow();
    void updateData(int n);
private slots:

    void on_actDelete_triggered();

    void on_actInsert_triggered();


    void on_pushButton_clicked();

    void on_actEdit_triggered();

    void on_actConfirm_triggered();

    void on_actCheck_triggered();

    void on_actAdd_triggered();

    void on_radio1_clicked();

    void on_radio2_clicked();

    void on_radio3_clicked();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
