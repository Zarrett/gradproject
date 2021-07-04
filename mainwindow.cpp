#include <iostream>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->scanButton, &QPushButton::pressed, this, &MainWindow::OnScanPressed);
    connect(ui->closeVideoOutputButton, &QPushButton::pressed, this, &MainWindow::OnCloseVideoPressed);
    connect(ui->saveButton, &QPushButton::pressed, this, &MainWindow::OnSaveUserPressed);
    connect(ui->recogniseButton, &QPushButton::pressed, this, &MainWindow::OnRecognisePressed);

    controller.reset(new ApplicationController(ui->labelVideo, ui->logLabel));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnScanPressed()
{
    ui->saveButton->setEnabled(true);
    controller->addUser();
}

void MainWindow::OnCloseVideoPressed()
{
    ui->saveButton->setEnabled(false);
    controller->StopDisplayingImage();
    ui->labelVideo->hide();
}

void MainWindow::OnSaveUserPressed()
{
    controller->makePhoto();
}

void MainWindow::OnRecognisePressed()
{
    ui->resLabel->setText("");
    bool res;

    std::thread authThread([&res, this]()
    {
        res = controller->authenticate();

        if(res)
            ui->resLabel->setText("success");
        else
            ui->resLabel->setText( "you fucking donkey");
    });

    authThread.detach();


}


