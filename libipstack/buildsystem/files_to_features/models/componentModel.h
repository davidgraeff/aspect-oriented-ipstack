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
#include <QJsonObject>
#include <QJsonArray>
#include "componentmodelbaseitem.h"

class ComponentModelItem;
class ComponentModelFileItem;
class ComponentModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class ComponentModelItem;
public:
    ComponentModel(const QString& base_directory, const QString& featureToFilesRelationfile, QObject *parent = 0);
    ~ComponentModel();
    ComponentModelItem *getRootItem();

    /**
     * @brief Save the component and file structure to a file
     * @param device The target device
     * @return Return true if successfully saved
     */
    bool save(QIODevice* device=0, bool freeAfterUse = true);

    /**
     * Remove all file entries that do not belong to an existing file.
     * This also removes empty subcomponents.
     */
    void remove_non_existing_files();

    /**
     * Update model for the item. Call this after you have changed values
     * of item and want to have that reflected in views that use this model.
     * @param item Update the model at items positions.
     */
    void update(ComponentModelBaseItem* item);
    QModelIndex indexOf(ComponentModelBaseItem* item) const;

    /**
     * @brief get_used_files
     * @return Return used files
     */
    QStringList get_used_files() const;

    /**
     * @brief get_missing_files
     * @return Return missing files as QModelIndexList
     */
    QModelIndexList get_missing_files() const;

    /**
     * @brief Remove the base_directory part of absolute_path and
     * return the remaining relative path.
     * @param absolute_path
     * @return relative path
     */
    QString relative_directory(const QString& absolute_path);

    ///////////////////// Add/Remove /////////////////////
    QStringList removeComponent(ComponentModelItem* item);
    QStringList removeFile(ComponentModelFileItem* item);
    // Return added component
    ComponentModelItem *addComponent(ComponentModelItem* parent=0);
    /// if removeEntries is true all content will be erased and the model gets
    /// a reset signal. if removeEntries is false only the reset signal will be
    /// emitted.
    void clear(bool removeEntries = true);

    ///////////////////// Model/View /////////////////////
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    ///////////////////// Model/View Drag&Drop /////////////////////
    Qt::DropActions supportedDropActions () const;
    Qt::DropActions supportedDragActions () const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );
private:
    const QDir base_directory;
    QString featureToFilesRelationfile;
    ComponentModelItem *rootItem;

    /**
     * Parse a json object. The FeatureToFiles json objects are those with keys
     * like those: name, vname, file, files, subdir, comp.
     * Nodes are traversed recursively.
     */
    bool node(ComponentModelItem* current_item, const QJsonObject &obj);
    /**
     * A "comp" element of a node is parsed via this method.
     * Usually a component contains several (sub-)nodes.
     */
    bool componentArray(ComponentModelItem* current_item, const QJsonArray &arr);
};
