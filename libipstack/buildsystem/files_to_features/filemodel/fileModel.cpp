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
#include "filemodelitem.h"
#include <QDirIterator>
#include <QDebug>
#include <QColor>
#include <QMimeData>
#include <QUrl>

FileModel::FileModel(const QString& base_directory, QObject *parent)
    : QAbstractItemModel(parent), base_directory(QDir(base_directory))
{
    rootItem = FileModelItem::createDir(this, this->base_directory.absolutePath(), 0);
}

FileModel::~FileModel()
{
    delete rootItem;
}

void FileModel::createFileTree()
{
    all_files.clear();
    delete rootItem;
    rootItem = FileModelItem::createDir(this, base_directory.absolutePath(), 0);

    QStringList filter;
    filter << "*.ah" << "*.cpp" << "*.h";
    QDirIterator it(base_directory.absolutePath(),filter,QDir::Files|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    beginResetModel();
    while(it.hasNext()) {
        QFileInfo d(it.next());
        addFile(get_relative_dirs_list(d.absoluteDir()), d.fileName());
    }
    endResetModel();
    emit unused_files_update(all_files.size());
}

void FileModel::removeFiles(const QStringList &files)
{
    if (files.isEmpty())
        return;

    QListIterator<QString> it(files);
    while(it.hasNext()) {
        QFileInfo d(it.next());
        QStringList subDirsReverse = get_relative_dirs_list(d.absoluteDir());

        // Look for directory of current to-delete file
        FileModelItem* parent = rootItem;
        while (subDirsReverse.size()) {
            parent = parent->getItemByName(subDirsReverse.takeLast());
            if (!parent) { // We did not find this directory in out tree, look at next file
                break;
            }
        }
        if (parent) {
            removeFileAndEmptyDirs(parent,d.fileName());
        }
    }
    emit unused_files_update(all_files.size());
}

void FileModel::removeFileAndEmptyDirs(FileModelItem* parent, QString filename)
{
    FileModelItem* currentItem;
    int index;
    for (;;) {
        if ((index = parent->binary_search(filename))!=-1) {
            Q_ASSERT(index<parent->childs.size());
            currentItem = parent->childs[index];
            beginRemoveRows(createIndex(index,0,currentItem).parent(),index,index);
            // Delete item (removes automatically from hash list and deletes children)
            parent->childs.removeAt(index);
            delete currentItem;
            endRemoveRows();
            // Look for empty parent dirs
            if (parent->childs.isEmpty()) {
                filename = parent->name;
                if (parent->parent)
                    parent = parent->parent;
                else
                    parent = rootItem;
            } else
                break;
        } else
            break;
    }
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
    emit unused_files_update(all_files.size());
}

int FileModel::get_unused_files_size() const
{
    return all_files.size();
}

QList<FileModelItem*> FileModel::get_file_path(const QString &filename)
{
    return all_files.values(filename);
}

FileModelItem *FileModel::getRootItem()
{
    return rootItem;
}

void FileModel::addFile(QStringList subDirsReverse, const QString &file)
{
    FileModelItem* parent = rootItem;
    while (subDirsReverse.size()) {
        const QString dirname = subDirsReverse.takeLast();
        FileModelItem* found = parent->getItemByName(dirname);
        if (!found) { // We did not find this directory in out tree, we create one
            parent = FileModelItem::createDir(this, dirname, parent);
        } else { // we found it and use it
            parent = found;
        }
    }

    // Add file
    all_files.insertMulti(file, FileModelItem::createFile(this,file, parent));
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
        return defaultFlags;
    else
        return Qt::ItemIsDragEnabled | defaultFlags;
}

Qt::DropActions FileModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

Qt::DropActions FileModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

QStringList FileModel::mimeTypes() const
{
    QStringList types;
    types << "text/uri-list";
    return types;
}

QMimeData *FileModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QList<QUrl> urls;

    foreach (const QModelIndex &index, indexes) {
        if (index.isValid() && index.column()==0) {
            FileModelItem *item = static_cast<FileModelItem*>(index.internalPointer());
            if (item->isFile)
                urls << QUrl::fromLocalFile(item->full_absolute_path());
            else {
                QStringList files = item->get_all_files();
                foreach(const QString& file, files)
                    urls << QUrl::fromLocalFile(file);
            }

        }
    }

    mimeData->setUrls(urls);
    return mimeData;
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
