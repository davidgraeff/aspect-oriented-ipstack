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
#include "picojson.h"
#include "componentmodelbaseitem.h"

class ComponentModelItem;
class ComponentModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class ComponentModelItem;
public:
    ComponentModel(const QString& base_directory, const QString& featureToFilesRelationfile, QObject *parent = 0);
    ~ComponentModel();

    void save();
    void remove_non_existing_files();

    QStringList get_used_files();
    QModelIndexList get_missing_files();
    QStringList removeComponent(ComponentModelItem* c);
    void addComponent(ComponentModelItem* parent);

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

    /**
     * Return the string of the map at position "name". Return an empty string
     * if the map does not contain that key.
     */
    QString getMapString(const picojson::value::object& obj, std::string name);
    /**
     * Return a list of files either via the "files" key of the obj map or via
     * the "file" key. An empty list is returned if neither of those keys is existing.
     */
    QList<QString> get_files(const picojson::value::object& obj);
    /**
     * Parse a json object. The FeatureToFiles json objects are those with keys
     * like those: name, vname, file, files, subdir, comp.
     * Nodes are traversed recursively.
     */
    bool node(ComponentModelItem* current_item, const picojson::value::object& obj);
    /**
     * A "comp" element of a node is parsed via this method.
     * Usually a component contains several (sub-)nodes.
     */
    bool componentArray(ComponentModelItem* current_item, const picojson::value::array& obj);
private:
    const QDir base_directory;
    ComponentModelItem *rootItem;
    QStringList get_relative_dirs_list(QDir path);
};
