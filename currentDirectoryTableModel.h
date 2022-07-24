#ifndef CURRENTDIRECTORYTABLEMODEL_H
#define CURRENTDIRECTORYTABLEMODEL_H

#include <QAbstractTableModel>
#include <QtGui/QIcon>
#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <sys/stat.h>
#include <QAbstractFileIconProvider>

namespace fs = std::filesystem;

std::string getStringTime(long long time_, std::string format);
std::string getStringTime(long long time_, const char *format);

class FileIconProvider : public QAbstractFileIconProvider {

};

class CurrentDirectoryTableModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    const std::vector<QString> columns = std::vector<QString>{"Name", "Size", "Created", "Modified", "Accessed", "Status"};
    std::vector<std::vector<QString>> _data;
    const FileIconProvider fileIconProvider = FileIconProvider();

public:
    std::string path {""};
    std::vector<std::string> files;

    explicit CurrentDirectoryTableModel();
    explicit CurrentDirectoryTableModel(const char* _path);

    void setPath(const char *path);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &index = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &index = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

#endif
