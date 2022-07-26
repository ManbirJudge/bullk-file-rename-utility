#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/QDir>
#include <QtWidgets/qfilesystemmodel.h>
#include <regex>

#include "currentDirectoryTableModel.h"
#include "Utils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QStringList dateFormats = {
        "%d-%m-%y",
        "%d-%m-%y %H-%M",
        "%d-%m-%y %H-%M-%S",
        "%m-%d-%y",
        "%m-%d-%y %H-%M",
        "%m-%d-%y %H-%M-%S",
        "%y-%m-%d",
        "%y-%m-%d %H-%M",
        "%y-%m-%d %H-%M-%S",
        "Custom"
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onCurrentLinePathEditReturnPressed();

    void onDirectoryTreeViewClicked(QModelIndex index);
    void onCurrentDirectoryTableDoubleClicked(QModelIndex index);

    void onRenameBtnCLicked();

private:
    QFileSystemModel *directoryModel = new QFileSystemModel();
    CurrentDirectoryTableModel *currentDirectoryModel = new CurrentDirectoryTableModel();

    Ui::MainWindow *ui;
};
#endif
