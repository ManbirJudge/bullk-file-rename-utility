#include "mainWindow.h"
#include "./ui_mainWindow.h"

struct SelectedFile
{
    QString filePath;
    QString fileName;
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
                                  .setFlag(QDir::AllDirs));

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

    // setting up file system layout splitter
    // ui->fileSystemSplitter->setSizes(QList<int>{int(ui->fileSystemSplitter->width() * 0.2), int(ui->fileSystemSplitter->width() * 0.8)});
    ui->fileSystemSplitter->setStretchFactor(1, 4);

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
        SLOT(onCurrentLinePathEditReturnPressed()));
    connect(
        ui->currentPathLevelUpBtn,
        SIGNAL(clicked()),
        this,
        SLOT(onCurrentPathLevelUpBtnClicked()));
    connect(
        ui->currentPathBrowseBtn,
        SIGNAL(clicked()),
        this,
        SLOT(onCurrentPathBrowseBtnClicked()));


    connect(
        ui->directoryTreeView,
        SIGNAL(clicked(QModelIndex)),
        this,
        SLOT(onDirectoryTreeViewClicked(QModelIndex)));
    connect(
        ui->currentDirectoryTable,
        SIGNAL(doubleClicked(QModelIndex)),
        this,
        SLOT(onCurrentDirectoryTableDoubleClicked(QModelIndex)));

    connect(
        ui->renameBtn,
        SIGNAL(clicked()),
        this,
        SLOT(onRenameBtnCLicked()));

    connect(
        ui->checkOutOnGithubBtn,
        SIGNAL(clicked()),
        this,
        SLOT(onCheckOutOnGithubBtn())
    );
}

void MainWindow::onCurrentLinePathEditReturnPressed()
{
    std::string newPath = ui->currentPathLineEdit->text().toStdString();

    if (pathExists(newPath) && isDir(newPath))
    {
        QModelIndex newPathIndex = MainWindow::directoryModel->index(QString(newPath.c_str()));
        std::string currentPath = MainWindow::directoryModel->filePath(newPathIndex).toStdString();

        ui->directoryTreeView->setCurrentIndex(newPathIndex);
        ui->currentPathLineEdit->setText(QString(currentPath.c_str()));

        ui->currentDirectoryTable->setModel(new CurrentDirectoryTableModel(currentPath.c_str()));
    }
    else
    {
        std::string currentPath = MainWindow::directoryModel->filePath(ui->directoryTreeView->currentIndex()).toStdString();
        ui->currentPathLineEdit->setText(QString(currentPath.c_str()));
    }
}
void MainWindow::onCurrentPathLevelUpBtnClicked() {
    QString currentPath = ui->currentPathLineEdit->text();
    QString newPath;

    QStringList currentPathSegments = currentPath.split('/');
    currentPathSegments.pop_back();

    for (size_t i {}; i < currentPathSegments.size(); i++) {
        QString currentPathSegment = currentPathSegments.at(i);

        newPath += currentPathSegment;
        if (i != currentPathSegments.size() - 1 || currentPathSegment.endsWith(':')) newPath += '/';
    }

    if (pathExists(newPath.toStdString()))
    {
        QModelIndex newPathIndex = MainWindow::directoryModel->index(newPath);

        ui->directoryTreeView->setCurrentIndex(newPathIndex);
        ui->currentPathLineEdit->setText(newPath);

        ui->currentDirectoryTable->setModel(new CurrentDirectoryTableModel(newPath.toStdString().c_str()));
    }
}
void MainWindow::onCurrentPathBrowseBtnClicked() {
    QFileDialog directoryFileDialog (this);

    directoryFileDialog.setWindowTitle("Selected the Directory");
    directoryFileDialog.setViewMode(QFileDialog::Detail);
    directoryFileDialog.setFileMode(QFileDialog::Directory);
    directoryFileDialog.setOptions(QFileDialog::ShowDirsOnly);

    if (directoryFileDialog.exec()) {
        QString newPath = directoryFileDialog.selectedFiles()[0];

        if (pathExists(newPath.toStdString()))
        {
            QModelIndex newPathIndex = MainWindow::directoryModel->index(newPath);

            ui->directoryTreeView->setCurrentIndex(newPathIndex);
            ui->currentPathLineEdit->setText(newPath);

            ui->currentDirectoryTable->setModel(new CurrentDirectoryTableModel(newPath.toStdString().c_str()));
        }
    }
}
void MainWindow::onDirectoryTreeViewClicked(QModelIndex index)
{
    std::string currentPath = MainWindow::directoryModel->filePath(index).toStdString();

    ui->currentPathLineEdit->setText(QString(currentPath.c_str()));
    ui->currentDirectoryTable->setModel(new CurrentDirectoryTableModel(currentPath.c_str()));
}
void MainWindow::onCurrentDirectoryTableDoubleClicked(QModelIndex index)
{
    std::string newPath = ui->currentDirectoryTable->model()->data(index, 2009).toString().toStdString();

    if (isDir(newPath))
    {
        QModelIndex newPathIndex = MainWindow::directoryModel->index(QString(newPath.c_str()));
        std::string currentPath = MainWindow::directoryModel->filePath(newPathIndex).toStdString();

        ui->directoryTreeView->setCurrentIndex(newPathIndex);
        ui->currentPathLineEdit->setText(QString(currentPath.c_str()));

        ui->currentDirectoryTable->setModel(new CurrentDirectoryTableModel(currentPath.c_str()));
    }
}
void MainWindow::onCheckOutOnGithubBtn() {
    QDesktopServices::openUrl(QUrl("https://github.com/ManbirJudge/bullk-file-rename-utility.git"));
}

void MainWindow::onRenameBtnCLicked()
{
    // getting the selected files paths
    QList<QModelIndex> selectedIndexes = ui->currentDirectoryTable->selectionModel()->selectedIndexes();

    QList<int> selectedRows;
    std::vector<SelectedFile> selectedFiles;
    std::vector<SelectedFile> renamedFiles;

    for (QModelIndex selectedIndex : selectedIndexes)
    {
        if (selectedRows.indexOf(selectedIndex.row()) != -1) {
            continue;
        }

        selectedRows.push_back(selectedIndex.row());

        SelectedFile selectedFile;

        selectedFile.filePath = ui->currentDirectoryTable->model()->data(selectedIndex, 2009).toString();
        selectedFile.fileName = QString(selectedFile.filePath.toStdString().substr(selectedFile.filePath.toStdString().find_last_of('/') + 1, selectedFile.filePath.length()).c_str());
        selectedFile.folder = isDir(selectedFile.filePath.toStdString());

        selectedFiles.push_back(selectedFile);
    }

    // getting rename options (from ui)
    QString regexMatch = ui->regExMatchLineEdit->text();
    QString regexReplace = ui->regExReplaceLineEdit->text();
    bool regexIncludeExtension = ui->regExIncExtCheck->isChecked();
    // bool regexSimple = ui->regExSimpleCheck->isChecked();
    // bool regexV2 = ui->regExv2CheckBox->isChecked();

    int nameTypeIndex = ui->nameTypeCombo->currentIndex();
    QString nameFixed = ui->fixedNameEdit->text();

    QString wordToReplace = ui->replaceLineEdit->text();
    QString wordToReplaceWith = ui->replaceWithLineEdit->text();
    bool replaceMatchCase = ui->replaceMathCaseCheck->isChecked();
    // bool replaceFirst = ui->replaceFirstCheck->isChecked();

    int caseTypeIndex = ui->caseTypeCombo->currentIndex();
    // QString caseException = ui->caseExceptionEdit->text();

    int removeFirstN = ui->removeFirstNSpin->value();
    int removeLastN = ui->removeLastNSpin->value();
    int removeFrom = ui->removeFromSpin->value();
    int removeTo = ui->removeToSpin->value();
    QString charsToRemove = ui->removeCharsLineEdit->text();
    QString wordsToRemove = ui->removeWordsLineEdit->text();
    // QString removeCropType = ui->removeCropCombo->currentText();
    // QString removeCropText = ui->removeCropEdit->text();
    bool removeDigits = ui->removeDigitsCheck->isChecked();
    bool removeHigh = ui->removeHighCheck->isChecked();
    bool removeTrim = ui->removeTrimCheck->isChecked();
    bool removeDOrS = ui->removeDSCheck->isChecked();
    bool removeAccents = ui->removeAccentCheck->isChecked();
    bool removeChars = ui->removeCharsCheck->isChecked();
    bool removeSymbols = ui->removeSymbolsCheck->isChecked();
    int removeLeadDotsIndex = ui->removeLeadDotsCombo->currentIndex();

    QString copyMovePartsFromType = ui->moveCopyPartsCombo_1->currentText();
    int copyMovePartsFrom = ui->moveCopyPartsSpin_1->value();
    int copyMovePartsToTypeIndex = ui->moveCopyPartsCombo_2->currentIndex();
    int copyMovePartsTo = ui->moveCopyPartsSpin_2->value();
    QString copyMovePartsSeperation = ui->moveCopyPartsSepEdit->text();

    QString addPrefix = ui->addPrefixEdit->text();
    QString addInsert = ui->addInsertEdit->text();
    int addInsertPos = ui->addAtPosSpin->value();
    QString addSuffix = ui->addSuffixEdit->text();
    bool addWordSpace = ui->addWordSpaceCheck->isChecked();

    int autoDateModeIndex = ui->autoDateModeCombo->currentIndex();
    int autoDateTypeIndex = ui->autoDateTypeCombo->currentIndex();
    int autoDateFormatIndex = ui->autoDateFmtCombo->currentIndex();
    QString autoDateSeperation = ui->autoDateSepEdit->text();
    QString autoDateSegmentation = ui->autoDateSegEdit->text();
    QString autoDateCustomFormat = ui->autoDateCustomEdit->text();
    bool autoDateCent = ui->autoDateCentCheck->isChecked();

    int appendFolderNameTypeIndex = ui->appendFolderNameCombo->currentIndex();
    QString appendFolderNameSeperation = ui->appendFolderNameSepEdit->text();
    // int appendFolderNameLevel = ui->appendFolderNameLevelsSpin->value();

    int numberingModeIndex = ui->numberingModeCombo->currentIndex();
    int numberingInsertAtPos = ui->numberingAtSpin->value();
    int numberingStart = ui->numberingStartSpin->value();
    int numberingIncrement = ui->numberingIncrSpin->value();
    int numberingPadding = ui->numberingPadSpin->value();
    QString numberingSeperation = ui->numberingSepEdit->text();
    QString numberingType = ui->numberingTypeCombo->currentText();
    QString numberCase = ui->numberingCaseCombo->currentText();

    int extensionTypeIndex = ui->extensionCombo->currentIndex();
    QString extensionFixed = ui->extensionEdit->text();

    // calculating renamed file names
    int currentNumberingValue = numberingStart;

    QRegularExpression regex(regexMatch);

    for (size_t i{}; i < selectedFiles.size(); i++)
    {
        SelectedFile selectedFile = selectedFiles.at(i);

        if (selectedFile.folder) {
            // TODO: Implement renaming of folders too
            continue;
        }

        // -------------------- (1) Regex
        QString name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        QString ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        if (regexIncludeExtension)
        {
            selectedFile.fileName = selectedFile.fileName.replace(regex, regexReplace);
        }
        else
        {
            selectedFile.fileName = name.replace(regex, regexReplace) + ext;
        }

        // -------------------- (2) Name
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        switch (nameTypeIndex)
        {
        case 0:
            break;

        case 1:
            selectedFile.fileName = ext;
            break;

        case 2:
            selectedFile.fileName = nameFixed + ext;
            break;

        case 3:
            std::reverse(name.begin(), name.end());
            selectedFile.fileName = name + ext;
            break;
        }

        // -------------------- (3) Replace
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        if (replaceMatchCase)
        {
            selectedFile.fileName = name.replace(wordToReplace, wordToReplaceWith, Qt::CaseSensitive) + ext;
        }
        else
        {
            selectedFile.fileName = name.replace(wordToReplace, wordToReplaceWith, Qt::CaseInsensitive) + ext;
        }

        // -------------------- (4) Case
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        switch (caseTypeIndex)
        {
        case 0:
        {
            break;
        }
        case 1:
        {
            selectedFile.fileName = name.toLower() + ext;
            break;
        }

        case 2:
        {
            selectedFile.fileName = name.toUpper() + ext;
            break;
        }
        case 3:
        {
            // TODO: Implement title case
            break;
        }
        case 4:
        {
            // TODO: Implement sentence case
            break;
        }
        case 5:
        {
            // TODO: Implement enhancded title case
            break;
        }
        }

        // -------------------- (5) Remove
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        if (removeFirstN > 0)
        {
            name = name.mid(removeFirstN, -1);
        }
        if (removeLastN > 0)
        {
            name = name.mid(0, -removeLastN);
        }

        if (removeFrom > 0 && removeTo > 0 && removeFrom < removeTo) {
            name = name.mid(0, removeFrom - 1) + name.mid(removeTo, -1);
        }

        for (QString chr : charsToRemove.split(' ')) {  // TODO: add a check to see if all the words are actually characters meaning have length of 1 and 1 only
            name = name.replace(chr, "");
        }
        for (QString word : wordsToRemove.split(' ')) {
            name = name.replace(word, "");
        }

        if (removeDigits) {
            name = name.replace(QRegularExpression("[0-9]+"), "");
        }
        if (removeHigh) {
            name = name.replace(QRegularExpression("[A-Z]+"), "");
        }
        if (removeTrim) {
            // only remove trailing whitespace
            name = name.trimmed();
        }
        if (removeDOrS) {
            name = name.replace("  ", " ");
        }
        if (removeAccents) {
            name = name.normalized(QString::NormalizationForm_KD);
        }
        if (removeChars) {
            name = name.replace(QRegularExpression("[a-zA-Z]+"), "");
        }
        if (removeSymbols) {
            name = name.replace(QRegularExpression("[\\W]"), "");
        }

        switch (removeLeadDotsIndex)
        {
        case 0:
        {
            break;
        }
        case 1:
        {
            name = name.replace('.', "");
            break;
        }
        case 2:
        {
            name = name.replace("..", "");
            break;
        }
        case 3:
        {
            name = name.replace('.', "");
            name = name.replace("..", "");
            break;
        }
        }

        selectedFile.fileName = name + ext;

        // -------------------- (6) Move/copy parts
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        if (copyMovePartsFromType != "None" && copyMovePartsToTypeIndex != 0) {
            QString partToBeCopiedOrMoved;

            if (copyMovePartsFromType.endsWith("First n")) {
                partToBeCopiedOrMoved = name.mid(0, copyMovePartsFrom);
                if (copyMovePartsFromType.startsWith("Move")) name = name.mid(copyMovePartsFrom, -1);
            }
            else if (copyMovePartsFromType.endsWith("Last n")) {
                partToBeCopiedOrMoved = name.mid(name.length() - copyMovePartsFrom, -1);
                if (copyMovePartsFromType.startsWith("Move")) name = name.mid(0, name.length() - copyMovePartsFrom);
            }

            switch (copyMovePartsToTypeIndex) {
            case 1: {
                name = partToBeCopiedOrMoved + copyMovePartsSeperation + name;
                break;
            }
            case 2: {
                name = name + copyMovePartsSeperation + partToBeCopiedOrMoved;
                break;
            }
            case 3: {
                name = name.mid(0, copyMovePartsToTypeIndex - 1) + copyMovePartsSeperation + partToBeCopiedOrMoved + copyMovePartsSeperation + name.mid(copyMovePartsToTypeIndex - 1, 0);
                break;
            }
            }
        }

        selectedFile.fileName = name + ext;

        // -------------------- (7) Add
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        QString space = "";
        if (addWordSpace) {
            space = " ";
        }

        if (addPrefix.isEmpty() || addPrefix.isNull()) name = addPrefix + space + name;
        if (addInsertPos != 0) name = name.mid(0, addInsertPos - 1) + space + addInsert + space + name.mid(addInsertPos - 1, -1);
        if (addSuffix.isEmpty() || addSuffix.isNull()) name = name + space + addSuffix;

        selectedFile.fileName = name + ext;

        // -------------------- (8) Autodate
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        if (autoDateModeIndex != 0) {
            QString date;
            QString dateFormat;

            // getting date format
            QString _ = MainWindow::dateFormats[autoDateFormatIndex];

            if (_ == "Custom")
            {
                dateFormat = autoDateCustomFormat;
            }
            else
            {
                dateFormat = _.replace("-", autoDateSegmentation);
                if (autoDateCent) dateFormat.replace("%y", "%Y");
            }

            // getting date
            struct stat fStat;
            stat(selectedFile.filePath.toStdString().c_str(), &fStat);

            switch (autoDateTypeIndex)
            {
            case 0:
            {
                date = getStringTime(fStat.st_ctime, dateFormat.toStdString()).c_str();
                break;
            }
            case 1:
            {
                date = getStringTime(fStat.st_mtime, dateFormat.toStdString()).c_str();
                break;
            }
            case 2:
            {
                date = getStringTime(fStat.st_atime, dateFormat.toStdString()).c_str();
                break;
            }
            case 3:
            {
                date = getStringTime(-1, dateFormat.toStdString()).c_str();
                break;
            }
            }

            // dating the name
            switch (autoDateModeIndex) {
            case 1: {
                name = date + autoDateSeperation + name;
                break;
            }
            case 2: {
                name = name + autoDateSeperation + date;
                break;
            }
            }
        }

        selectedFile.fileName = name + ext;

        // -------------------- (9) Append folder name
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        if (appendFolderNameTypeIndex != 0) {
            QStringList _ = selectedFile.filePath.split("/");
            QString folderName = _.at(_.size() - 2);

            switch(appendFolderNameTypeIndex)
            {
            case 1:
            {
                selectedFile.fileName = folderName + appendFolderNameSeperation + name + ext;
                break;
            }
            case 2:
            {
                selectedFile.fileName = name + appendFolderNameSeperation + folderName + ext;
                break;
            }
            }
        }

        // -------------------- (10) Numbering
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        if (numberingModeIndex != 0) {
            QString numberingStr;

            if (numberingType.startsWith("Base"))
            {
                if (numberingType == "Base 2 (Binary)")
                {
                    numberingStr = intToBase(currentNumberingValue, 2).c_str();
                }
                else if (numberingType == "Base 8 (Octa)")
                {
                    numberingStr = intToBase(currentNumberingValue, 8).c_str();
                }
                else if (numberingType == "Base 10 (Decimal)")
                {
                    numberingStr = intToBase(currentNumberingValue, 10).c_str();
                }
                else if (numberingType == "Base 16 (Hex)")
                {
                    numberingStr = intToBase(currentNumberingValue, 16).c_str();
                }
                else {
                    numberingStr = intToBase(currentNumberingValue, numberingType.remove("Base").toInt()).c_str();
                }
            }
            else if (numberingType == "A-Z")
            {

            }
            else if (numberingType == "a-z")
            {

            }
            else if (numberingType == "Roman Numeral")
            {

            }

            switch (numberingModeIndex)
            {
            case 1:
            {
                name = numberingStr + numberingSeperation + name;
                break;
            }
            case 2:
            {
                name = name + numberingSeperation + numberingStr ;
                break;
            }
            case 3:
            {
                name = numberingStr + numberingSeperation + name + numberingSeperation + numberingStr;
                break;
            }
            case 4:
            {
                name = name.mid(0, numberingInsertAtPos - 1) + numberingSeperation + numberingStr + numberingSeperation + name.mid(numberingInsertAtPos, -1);
                break;
            }
            }
        }

        selectedFile.fileName = name + ext;

        // -------------------- (11) Extension
        name = selectedFile.fileName.mid(0, selectedFile.fileName.lastIndexOf('.'));
        ext = selectedFile.fileName.mid(selectedFile.fileName.lastIndexOf('.'), -1);

        switch (extensionTypeIndex)
        {
        case 0:
        {
            break;
        }
        case 1: {
            ext = ext.toLower();
            break;
        }
        case 2: {
            ext = ext.toUpper();
            break;
        }
        case 3: {
            // TODO: implement title case
            break;
        }
        case 4: {
            ext = "." + extensionFixed;
            break;
        }
        case 5: {
            ext = ext + "." + extensionFixed;
            break;
        }
        case 6: {
            ext = "";
            break;
        }
        }

        selectedFile.fileName = name + ext;

        // adding file to renamed files list
        renamedFiles.push_back(selectedFile);
        currentNumberingValue += numberingIncrement;
    }

    // actualling renaming the files in the system: TODO
    for (size_t i{}; i < renamedFiles.size(); i++) {
        qDebug() << renamedFiles[i].filePath.toStdString().c_str() << " -> " << renamedFiles[i].fileName;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
