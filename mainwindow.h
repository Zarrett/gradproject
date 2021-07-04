#pragma once

#include <QMainWindow>
#include <thread>

#include "applicationcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void OnScanPressed();
    void OnCloseVideoPressed();
    void OnSaveUserPressed();
    void OnRecognisePressed();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<ApplicationController> controller;

};
