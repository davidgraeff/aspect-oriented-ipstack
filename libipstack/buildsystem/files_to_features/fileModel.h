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

#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QDir>

class FileModelItem {
public:
    QString name;

    bool isFile;

    int getRow() {
        if (parent) {
            return parent->childs.indexOf(this);
        } else
            return 0;
    }

    QString full_absolute_path() {
        if (parent)
            return parent->full_absolute_path() + "/" + name;
        else
            return name;
    }

    FileModelItem* parent;
    QList<FileModelItem*> childs;

    ~FileModelItem() {
        qDeleteAll(childs);
    }
    static FileModelItem* createFile(const QString& name, FileModelItem* parent) {
        FileModelItem* item = new FileModelItem();
        item->name = name; item->isFile = true;
        if (parent && parent->getItemByName(item)) { // already a child with this filename in parent
            delete item;
            return 0;
        }
        item->addToParent(parent);
        return item;
    }
    static FileModelItem* createDir(const QString& name, FileModelItem* parent) {
        FileModelItem* item = new FileModelItem();
        item->name = name; item->isFile = false;
        item->addToParent(parent);
        return item;
    }
    void addToParent(FileModelItem* parent) {
        this->parent = parent;
        if (parent) {
            int i=0;
            for (;i<parent->childs.size();++i) {
                if (parent->childs[i]->name >= name)
                    break;
            }
            parent->childs.insert(i, this);
        }
    }
    int indexOf(FileModelItem* itemWithName) {
        QList<FileModelItem*>::iterator it = qBinaryFind(childs.begin(), childs.end(), itemWithName,
                    [](const FileModelItem* const &a,const FileModelItem* const &b){return a->name < b->name;});

        if (it == childs.end()) {
            return -1;
        } else
            return it - childs.begin();
    }
    FileModelItem* getItemByName(FileModelItem* itemWithName) {
        QList<FileModelItem*>::iterator it = qBinaryFind(childs.begin(), childs.end(), itemWithName,
                    [](const FileModelItem* const &a,const FileModelItem* const &b){return a->name < b->name;});

        if (it == childs.end()) {
            return 0;
        } else
            return *it;
    }
private:
    FileModelItem() {}
};

class FileModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    FileModel(const QString& base_directory, QObject *parent = 0);
    ~FileModel();
    void createFileTree();
    void removeFiles(const QStringList& files);
    void addFiles(const QStringList& files);
    int get_unused_files_size() const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    Qt::DropActions supportedDropActions () const;
    Qt::DropActions supportedDragActions () const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );

private:
    FileModelItem *rootItem;
    const QDir base_directory;
    int cached_unused_files;
    void addFile(QStringList subDirsReverse, const QString& file);
    QStringList get_relative_dirs_list(QDir path);
Q_SIGNALS:
    void unused_files_update(int number);
};
