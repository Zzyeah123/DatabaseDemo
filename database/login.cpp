#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
}

login::~login()
{
    delete ui;
}

void login::on_pushButton_Login_clicked()
{
    //查找用户和密码

    //提示失败

    //成功进入页面
    emit sendLoginSuccess();
}

void login::on_pushButton_Close_clicked()
{
    exit(0);
}
