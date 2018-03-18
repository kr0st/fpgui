#include "historybrowserwindow.h"
#include "ui_historybrowserwindow.h"

HistoryBrowserWindow::HistoryBrowserWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryBrowserWindow)
{
    ui->setupUi(this);
}

HistoryBrowserWindow::~HistoryBrowserWindow()
{
    delete ui;
}
