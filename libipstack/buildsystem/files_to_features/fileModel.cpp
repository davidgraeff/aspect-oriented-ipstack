/*
 * Copyright 2013 David Graeff
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE REGENTS AND CONTRIBUTORS, INC. OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fileModel.h"
#include <QDirIterator>
#include <QDebug>
#include <QColor>
#include <QMimeData>

FileModel::FileModel(const QString& base_directory, QObject *parent)
    : QAbstractItemModel(parent), rootItem(0), base_directory(QDir(base_directory)),cached_unused_files(0)
{
    createFileTree();
}

FileModel::~FileModel()
{
    delete rootItem;
}

void FileModel::createFileTree()
{
    cached_unused_files = 0;
    delete rootItem;
    rootItem = FileModelItem::createDir(base_directory.absolutePath(), 0);

    QStringList filter;
    filter << "*.ah" << "*.cpp" << "*.h";
    QDirIterator it(base_directory.absolutePath(),filter,QDir::Files|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    beginResetModel();
    while(it.hasNext()) {
        QFileInfo d(it.next());
        addFile(get_relative_dirs_list(d.absoluteDir()), d.fileName());
    }
    endResetModel();
}

void FileModel::removeFiles(const QStringList &files)
{
    if (files.isEmpty())
        return;

    beginResetModel();
    QListIterator<QString> it(files);
    while(it.hasNext()) {
        QFileInfo d(it.next());
        QStringList subDirsReverse = get_relative_dirs_list(d.absoluteDir());

        // Look for directory of current to-delete file
        FileModelItem* parent = rootItem;
        FileModelItem* currentItem;
        FileModelItem* temp = FileModelItem::createDir("", 0);
        while (subDirsReverse.size()) {
            const QString dirname = subDirsReverse.takeLast();
            temp->name = dirname;
            parent = parent->getItemByName(temp);
            if (!parent) { // We did not find this directory in out tree, look at next file
                break;
            }
        }
        if (parent) {
            temp->name = d.fileName();
            temp->isFile = true;
            currentItem = temp;
            for (;;) {
                int i = parent->indexOf(currentItem);
                if (i!=-1) {
                    delete parent->childs.takeAt(i);
                    --cached_unused_files;
                    if (parent->childs.isEmpty() && parent->parent) {
                        currentItem = parent;
                        parent = parent->parent;
                    } else
                        break;
                } else
                    break;
            }
        }
        delete temp;
    }
    endResetModel();
    emit unused_files_update(cached_unused_files);
}

void FileModel::addFiles(const QStringList &files)
{
    beginResetModel();
    QListIterator<QString> it(files);
    while(it.hasNext()) {
        QFileInfo d(it.next());
        addFile(get_relative_dirs_list(d.absoluteDir()), d.fileName());
    }
    endResetModel();
    emit unused_files_update(cached_unused_files);
}

int FileModel::get_unused_files_size() const
{
    return cached_unused_files;
}

void FileModel::addFile(QStringList subDirsReverse, const QString &file)
{
    FileModelItem* parent = rootItem;
    while (subDirsReverse.size()) {
        const QString dirname = subDirsReverse.takeLast();
        FileModelItem* temp = FileModelItem::createDir(dirname, 0);
        FileModelItem* found = parent->getItemByName(temp);
        if (!found) { // We did not find this directory in out tree, we create one
            temp->addToParent(parent);
            parent = temp;
        } else { // we found it and use it
            delete temp;
            parent = found;
        }
    }

    // Add file
    FileModelItem::createFile(file, parent);
    ++cached_unused_files;
}

QStringList FileModel::get_relative_dirs_list(QDir path)
{
    QStringList relative_dirs;
    while (path != base_directory) {
        relative_dirs.append(path.dirName());
        path.cdUp();
        if (path.isRoot()) {
            qFatal("get_relative_dirs_list failed");
        }
    }
    return relative_dirs;
}

int FileModel::columnCount(const QModelIndex &) const
{
    return 1;
}

Qt::ItemFlags FileModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if (!index.isValid())
        return defaultFlags | Qt::ItemIsDropEnabled;

    FileModelItem *item = static_cast<FileModelItem*>(index.internalPointer());
    if (item->isFile)
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

Qt::DropActions FileModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::DropActions FileModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

QStringList FileModel::mimeTypes() const
{
    QStringList types;
    types << "application/files.text.list";
    return types;
}

QMimeData *FileModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            FileModelItem *item = static_cast<FileModelItem*>(index.internalPointer());
            stream << item->full_absolute_path();
        }
    }

    mimeData->setData("application/files.text.list", encodedData);
    return mimeData;
}

bool FileModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row);Q_UNUSED(column);Q_UNUSED(parent);

    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("application/files.text.list"))
        return false;

    QByteArray encodedData = data->data("application/files.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        QString text;
        stream >> text;
        QFileInfo info(text);
        addFile(get_relative_dirs_list(info.absoluteDir()), info.fileName());
    }

    return true;
}

QVariant FileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    FileModelItem *item = static_cast<FileModelItem*>(index.internalPointer());

    if (role==Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return item->name;
        }
    } else
    if (role==Qt::ToolTipRole) {
        return item->name;
    } else
    if (role==Qt::ForegroundRole && item->isFile) {
        return QColor(Qt::darkGreen);
    }
    return QVariant();
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation,
                            int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return tr("Filename");
        }
    }
    else if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case 0: return tr("Filename");
        }
    }
    return QVariant();
}

QModelIndex FileModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    FileModelItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FileModelItem*>(parent.internalPointer());

    FileModelItem *childItem = parentItem->childs.at(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex FileModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    FileModelItem *childItem = static_cast<FileModelItem*>(index.internalPointer());
    FileModelItem *parentItem = childItem->parent;

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->getRow(), 0, parentItem);
}

int FileModel::rowCount(const QModelIndex &parent) const
{
    FileModelItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FileModelItem*>(parent.internalPointer());

    return parentItem->childs.size();
}
