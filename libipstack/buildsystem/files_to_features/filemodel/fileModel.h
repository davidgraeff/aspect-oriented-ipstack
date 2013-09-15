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

class FileModelItem;
class FileModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class FileModelItem;
public:
    FileModel(const QString& base_directory, QObject *parent = 0);
    ~FileModel();
public Q_SLOTS:
    /// This will read the filesystem and create a copy of the structure.
    void createFileTree();
    /// Remove files from this model. Filenames have to be the full absolute path.
    void removeFiles(const QStringList& files);
    /// Add files. Full absolute path names are required.
    void addFiles(const QStringList& files);
public:
    /// Return amount of files in this model. Usually called after "removeFiles".
    int get_unused_files_size() const;

    /// Return the file paths of a filename.
    QList<FileModelItem *> get_file_path(const QString& filename);

    FileModelItem* getRootItem();

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    Qt::DropActions supportedDropActions() const;
    Qt::DropActions supportedDragActions () const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;

private:
    QMultiMap<QString,FileModelItem*> all_files;
    FileModelItem *rootItem;
    const QDir base_directory;
    void addFile(QStringList subDirsReverse, const QString& file);
    QStringList get_relative_dirs_list(QDir path);
    void removeFileAndEmptyDirs(FileModelItem* parent, QString filename);
Q_SIGNALS:
    void unused_files_update(int number);
};
