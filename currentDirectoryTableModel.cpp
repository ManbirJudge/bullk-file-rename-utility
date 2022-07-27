#include "currentDirectoryTableModel.h"

CurrentDirectoryTableModel::CurrentDirectoryTableModel() : QAbstractTableModel(0) {

};
CurrentDirectoryTableModel::CurrentDirectoryTableModel(const char *_path) : QAbstractTableModel(0) {
    CurrentDirectoryTableModel::setPath(_path);
};

void CurrentDirectoryTableModel::setPath(const char *path) {
    CurrentDirectoryTableModel::path = path;

    CurrentDirectoryTableModel::files.clear();
    CurrentDirectoryTableModel::_data.clear();

    for (const auto &file : fs::directory_iterator(path)) {
        // getting path
        std::string fPath = file.path().string();

        // setting somethings to get other data
        std::stringstream ss;

        struct stat fStat;  // to get stats of a file
        stat(fPath.c_str(), &fStat);

        std::string fName;
        std::string fSize;
        std::string fCreationTime;
        std::string fModificationTime;
        std::string fAccessedTime;
        std::string fStatus = "None";

        // getting data
        fName = file.path().filename().string();

        ss << fStat.st_size;
        ss >> fSize;

        fCreationTime = getStringTime(fStat.st_ctime, "%d-%m-%Y  %H:%M");
        fModificationTime = getStringTime(fStat.st_mtime, "%d-%m-%Y  %H:%M");
        fAccessedTime = getStringTime(fStat.st_atime, "%d-%m-%Y  %H:%M");

        // setting data
        CurrentDirectoryTableModel::files.push_back(fPath);
        CurrentDirectoryTableModel::_data.push_back(std::vector<QString>{
                                                        fName.c_str(),
                                                        fSize.c_str(),
                                                        fCreationTime.c_str(),
                                                        fModificationTime.c_str(),
                                                        fAccessedTime.c_str(),
                                                        fStatus.c_str()
                                                    });
    }
}

QVariant CurrentDirectoryTableModel::data(const QModelIndex &index, int role) const {
    switch (role)
    {
        case Qt::DisplayRole:
        {
            return CurrentDirectoryTableModel::_data[index.row()][index.column()];
            break;
        }
        case Qt::DecorationRole:
        {
            QString value = CurrentDirectoryTableModel::_data[index.row()][index.column()];

            if (index.column() == 0)
            {
                return fileIconProvider.icon(QFileInfo(QString(files[index.row()].c_str())));
            }
            else if (index.column() == 5)
            {
                if (value == "None")
                {
                    return QIcon(QPixmap("assets/icons/help.png"));
                }
                else if (value == "Ok")
                {
                    return QIcon(QPixmap("assets/icons/status_ok.png"));
                }
                else if (value == "Error")
                {
                    return QIcon(QPixmap("assets/icons/status_error.png"));
                }
                else {
                    return QVariant();
                }
            }
            break;
        }
        case Qt::TextAlignmentRole:
        {
            if (index.column() == 5)
            {
                return Qt::AlignCenter;
            }
            else {
                return QVariant();
            }
            break;
        }
        case 2009:
        {
            return QString(CurrentDirectoryTableModel::files[index.row()].c_str()).replace("\\", "/");
        }
        default:
        {
            return QVariant();
        }
    }

    return QVariant();
}

int CurrentDirectoryTableModel::rowCount(const QModelIndex &index) const {
    return CurrentDirectoryTableModel::_data.size();
}
int CurrentDirectoryTableModel::columnCount(const QModelIndex &index) const {
    return CurrentDirectoryTableModel::columns.size();
}
QVariant CurrentDirectoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return CurrentDirectoryTableModel::columns[section];
        }
    }

    return QVariant();
}
