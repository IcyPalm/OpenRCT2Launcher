#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "platform.h"
#include "configuration.h"

#include <QDebug>
#include <QDir>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar->setHidden(true);
    connect(ui->launchButton, &QPushButton::clicked, this, &MainWindow::launch);

    connect(&updater, &Updater::installed, [this]{ ui->progressBar->setHidden(true); ui->launchButton->setEnabled(true); });
    connect(&updater, &Updater::error, [this](QString error){
        qDebug() << error; ui->errorLabel->setText(error); ui->progressBar->setHidden(true); ui->launchButton->setEnabled(true); });
    connect(&updater, &Updater::downloadProgress, [this](qint64 bytesReceived, qint64 bytesTotal){
        ui->progressBar->setHidden(false); ui->progressBar->setMaximum(bytesTotal); ui->progressBar->setValue(bytesReceived); });

    updater.download();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_optionsButton_clicked() {
    QDir dir = OPENRCT2_HOMEDIR;
    if (dir.cd(QStringLiteral(OPENRCT2_BASE))) {
        if (!dir.exists()) dir.mkpath(QStringLiteral(OPENRCT2_BASE));
    } else {
        dir.mkpath(QStringLiteral(OPENRCT2_BASE));
        dir.cd(QStringLiteral(OPENRCT2_BASE));
    }

    Configuration config(&updater.settings, dir.filePath(QStringLiteral("config.ini")));
    connect(&config, &Configuration::redownload, [this]{ ui->launchButton->setEnabled(false);
        ui->errorLabel->setText(QStringLiteral("")); updater.download(); ui->progressBar->setHidden(true); });
    config.exec();
}

void MainWindow::launch() {
    if (QProcess::startDetached(OPENRCT2_HOMEDIR.filePath(QStringLiteral(OPENRCT2_EXEC_LOCATION)), QStringList(), OPENRCT2_HOMEPATH)) {
        QApplication::quit();
    } else {
        ui->errorLabel->setText(tr("Unable to Launch Game"));
    }
}
