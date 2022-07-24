#include "mainWindow.h"
#include "./ui_mainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // setting up directory model
    directoryModel->setRootPath("/");
    directoryModel->setFilter(QDir::Filters()
                                        .setFlag(QDir::Dirs)
                                       .setFlag(QDir::Drives)
                                       .setFlag(QDir::NoDotAndDotDot)
                                       .setFlag(QDir::AllDirs)
                                      );

    ui->directoryTreeView->setModel(directoryModel);

    ui->directoryTreeView->hideColumn(1);
    ui->directoryTreeView->hideColumn(2);
    ui->directoryTreeView->hideColumn(3);

    // setting up current directory model
    ui->currentDirectoryTable->setModel(MainWindow::currentDirectoryModel);
    ui->currentDirectoryTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->currentDirectoryTable->setColumnWidth(0, 230);
    ui->currentDirectoryTable->setColumnWidth(1, 100);
    ui->currentDirectoryTable->setColumnWidth(2, 125);
    ui->currentDirectoryTable->setColumnWidth(3, 125);
    ui->currentDirectoryTable->setColumnWidth(4, 125);

    // setting up values for dropdowns
    ui->nameTypeCombo->addItems(QStringList{"Keep", "Remove", "Fixed", "Reverse"});
    ui->caseTypeCombo->addItems(QStringList{"Same", "Lower", "Upper", "Title", "Sentence", "Title Enhanced"});

    ui->removeCropCombo->addItems(QStringList{"Before", "After"});
    ui->removeLeadDotsCombo->addItems(QStringList{"None", ".", "..", "Both"});


    ui->moveCopyPartsCombo_1->addItems(QStringList{"None", "Copy First n", "Copy Last n", "Move First n", "Move Last n"});
    ui->moveCopyPartsCombo_2->addItems(QStringList{"None", "To Start", "To End", "To Pos."});

    ui->autoDateModeCombo->addItems(QStringList{"None", "Prefix", "Suffix"});
    ui->autoDateTypeCombo->addItems(QStringList{"Creation", "Modified", "Accessed", "Current"});
    ui->autoDateFmtCombo->addItems(dateFormats);

    ui->appendFolderNameCombo->addItems(QStringList{"None", "Prefix", "Suffix"});

    ui->numberingModeCombo->addItems(QStringList{"None", "Prefix", "Suffix", "Pre. + Suff.", "Insert"});
    ui->numberingTypeCombo->addItems(QStringList{
                                         "Base 2 (Binary)", "Base 3", "Base 4", "Base 5", "Base 6", "Base 7",
                                         "Base 8 (Octa)", "Base 9", "Base 10 (Decimal)", "Base 11", "Base 12",
                                         "Base 13", "Base 14", "Base 15", "Base 16 (Hex)", "Base 17", "Base 18",
                                         "Base 19", "Base 20", "Base 21", "Base 22", "Base 23", "Base 24",
                                         "Base 25", "Base 26", "Base 27", "Base 28", "Base 29", "Base 30",
                                         "Base 31", "Base 32", "Base 33", "Base 34", "Base 35", "Base 36",
                                         "A-Z", "a-z", "Roman Numerals"});
    ui->numberingTypeCombo->setCurrentIndex(8);
    ui->numberingCaseCombo->addItems(QStringList{"Same", "Lower", "Upper"});

    ui->extensionCombo->addItems(QStringList{"Same", "Lower", "Upper", "Title", "Fixed", "Extra", "Remove"});

    // setting up signals
    connect(
        ui->currentPathLineEdit,
        SIGNAL(returnPressed()),
        this,
        SLOT(onCurrentLinePathEditReturnPressed())
    );


    connect(
        ui->directoryTreeView,
        SIGNAL(clicked(QModelIndex)),
        this,
        SLOT(onDirectoryTreeViewClicked(QModelIndex))
    );
    connect(
        ui->currentDirectoryTable,
        SIGNAL(doubleClicked(QModelIndex)),
        this,
        SLOT(onCurrentDirectoryTableDoubleClicked(QModelIndex))
    );
}

void MainWindow::onCurrentLinePathEditReturnPressed() {
    std::string newPath = ui->currentPathLineEdit->text().toStdString();

    if (pathExists(newPath) && isDir(newPath))
    {
        QModelIndex newPathIndex = MainWindow::directoryModel->index(QString(newPath.c_str()));
        std::string currentPath = MainWindow::directoryModel->filePath(newPathIndex).toStdString();

        ui->directoryTreeView->setCurrentIndex(newPathIndex);
        ui->currentPathLineEdit->setText(QString(currentPath.c_str()));

        ui->currentDirectoryTable->setModel(new CurrentDirectoryTableModel(currentPath.c_str()));
    }
    else {
        std::string currentPath = MainWindow::directoryModel->filePath(ui->directoryTreeView->currentIndex()).toStdString();
        ui->currentPathLineEdit->setText(QString(currentPath.c_str()));
    }
}
void MainWindow::onDirectoryTreeViewClicked(QModelIndex index) {
    std::string currentPath = MainWindow::directoryModel->filePath(index).toStdString();

    ui->currentPathLineEdit->setText(QString(currentPath.c_str()));
    ui->currentDirectoryTable->setModel(new CurrentDirectoryTableModel(currentPath.c_str()));
}
void MainWindow::onCurrentDirectoryTableDoubleClicked(QModelIndex index) {
    std::string newPath = ui->currentDirectoryTable->model()->data(index, 2009).toString().toStdString();

    if (isDir(newPath)) {
        QModelIndex newPathIndex = MainWindow::directoryModel->index(QString(newPath.c_str()));
        std::string currentPath = MainWindow::directoryModel->filePath(newPathIndex).toStdString();

        ui->directoryTreeView->setCurrentIndex(newPathIndex);
        ui->currentPathLineEdit->setText(QString(currentPath.c_str()));

        ui->currentDirectoryTable->setModel(new CurrentDirectoryTableModel(currentPath.c_str()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

