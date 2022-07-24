#include "mainWindow.h"
#include "./ui_mainWindow.h"

struct SelectedFile {
    std::string filePath;
    std::string fileName;
    bool folder = false;
};

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

    connect(
        ui->renameBtn,
        SIGNAL(clicked()),
        this,
        SLOT(onRenameBtnCLicked())
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

void MainWindow::onRenameBtnCLicked() {
    // getting the selected files paths
    QList<QModelIndex> selectedIndexes = ui->currentDirectoryTable->selectionModel()->selectedIndexes();

    std::vector<int> selectedRows;
    std::vector<SelectedFile> selectedFiles;
    std::vector<std::string> renamedFiles;

    for (QModelIndex selectedIndex : selectedIndexes) {
        selectedRows.push_back(selectedIndex.row());

        SelectedFile selectedFile;

        selectedFile.filePath = ui->currentDirectoryTable->model()->data(selectedIndex, 2009).toString().toStdString();
        selectedFile.fileName = selectedFile.filePath.substr(selectedFile.filePath.find_last_of('\\') + 1, selectedFile.filePath.length());

        selectedFiles.push_back(selectedFile);
    }

    // getting rename options (from ui): TODO
    QString regexMatch = ui->regExMatchLineEdit->text();
    QString regexReplace = ui->regExReplaceLineEdit->text();
    bool regexIncludeExtension = ui->regExIncExtCheck->isChecked();

    QString nameType = ui->nameTypeCombo->currentText();

    QString wordToReplace = ui->replaceLineEdit->text();
    QString wordToReplaceWith = ui->replaceWithLineEdit->text();
    bool replaceMatchCase = ui->replaceMathCaseCheck->isChecked();
    bool replaceFirst = ui->replaceFirstCheck->isChecked();

    QString caseType = ui->caseTypeCombo->currentText();
    QString caseException = ui->caseExceptionEdit->text();

    int removeFirstN = ui->removeFirstNSpin->value();
    int removeLastN = ui->removeLastNSpin->value();
    int removeFrom = ui->removeFromSpin->value();
    int removeTo = ui->removeToSpin->value();
    QString charsToRemove = ui->removeCharsLineEdit->text();
    QString removeCropType = ui->removeCropCombo->currentText();
    QString removeCropText = ui->removeCropEdit->text();
    bool removeDigits = ui->removeDigitsCheck->isChecked();
    bool removeHigh = ui->removeHighCheck->isChecked();
    bool removeTrim = ui->removeTrimCheck->isChecked();
    bool removeDOrS = ui->removeDSCheck->isChecked();
    bool removeAccents = ui->removeAccentCheck->isChecked();
    bool removeChars = ui->removeCharsCheck->isChecked();
    bool removeSymbols = ui->removeSymbolsCheck->isChecked();
    QString removeLeadDots = ui->removeLeadDotsCombo->currentText();

    QString copyMovePartsFromType = ui->moveCopyPartsCombo_1->currentText();
    int copyMovePartsFrom = ui->moveCopyPartsSpin_1->value();
    QString copyMovePartsToType = ui->moveCopyPartsCombo_2->currentText();
    int copyMovePartsTo = ui->moveCopyPartsSpin_2->value();
    QString copyMovePartsSeperation = ui->moveCopyPartsSepEdit->text();

    QString addPrefix = ui->addPrefixEdit->text();
    QString addInsert = ui->addInsertEdit->text();
    int addInsertPos = ui->addAtPosSpin->value();
    QString addSuffix = ui->addSuffixEdit->text();
    bool addWordSpace = ui->addWordSpaceCheck->isChecked();

    QString autoDateMode = ui->autoDateModeCombo->currentText();
    QString autoDateType = ui->autoDateTypeCombo->currentText();
    int autoDateFormatIndex = ui->autoDateFmtCombo->currentIndex();
    QString autoDateSeperation = ui->autoDateSepEdit->text();
    QString autoDateSegmentation = ui->autoDateSegEdit->text();
    QString autoDateCustomFormat = ui->autoDateCustomEdit->text();
    bool autoDateCent = ui->autoDateCentCheck->isChecked();

    QString appendFolderNameType = ui->appendFolderNameCombo->currentText();
    QString appendFolderNameSeperation = ui->appendFolderNameSepEdit->text();

    QString numberingMode = ui->numberingModeCombo->currentText();
    int numberingStart = ui->numberingStartSpin->value();
    int numberingIncrement = ui->numberingIncrSpin->value();
    int numberingPadding = ui->numberingPadSpin->value();
    QString numberingSeperation = ui->numberingSepEdit->text();
    QString numberingType = ui->numberingTypeCombo->currentText();
    QString numberCase = ui->numberingCaseCombo->currentText();

    QString extensionType = ui->extensionCombo->currentText();

    // calculating renamed file names
    int currentNumberingValue = numberingStart;

    for (size_t i {}; i < selectedFiles.size(); i++)
    {
        SelectedFile selectFile = selectedFiles.at(i);

        // ---------- (1) Regex

        // ---------- (2) Name
    }

    // actualling renaming the files in the system: TODO
}

MainWindow::~MainWindow()
{
    delete ui;
}

