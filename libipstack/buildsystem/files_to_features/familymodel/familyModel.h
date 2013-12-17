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
#include <QIcon>

class FamilyBaseItem;
class FamilyComponent;
class FamilyFile;
class FamilyModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class FamilyComponent;
    friend class FamilyFile;
public:
    FamilyModel(const QString& base_directory, const QString& featureToFilesRelationfile, QObject *parent = 0);
    ~FamilyModel();
    FamilyComponent *getRootItem();

    /**
     * @brief Save the component and file structure to a file
     * @param device The target device
     * @return Return true if successfully saved
     */
    bool save(QIODevice* device=0, bool freeAfterUse = true);

    /**
     * Update model for the item. Call this after you have changed values
     * of item and want to have that reflected in views that use this model.
     * @param item Update the model at items positions.
     */
    void update(FamilyBaseItem* item);
    QModelIndex indexOf(FamilyBaseItem* item) const;

    /**
     * @brief get_used_files
     * @return Return used files
     */
    QStringList get_used_files() const;

    /**
     * @brief getDependStrings
     * @return Return the dependency string for each component of this model
     */
    QStringList get_dependencies() const;

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
    void removeComponent(FamilyComponent* item);
    void removeFile(FamilyFile *item);
    // Return added component
    FamilyComponent *addComponent(FamilyComponent* parent=0);
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
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

    ///////////////////// Model/View Drag&Drop /////////////////////
    Qt::DropActions supportedDropActions () const;
    Qt::DropActions supportedDragActions () const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );
private:
    const QDir base_directory;
    QString featureToFilesRelationfile;
    FamilyComponent *rootItem;
    mutable QList<FamilyFile*> moveFilesList;

    QIcon icon_missing;
    QIcon icon_file;
    QIcon icon_component;

    /**
     * Parse a json object. The FeatureToFiles json objects are those with keys
     * like those: name, vname, file, files, subdir, comp.
     * Nodes are traversed recursively.
     */
    bool node(FamilyComponent* current_item, const QJsonObject &obj);
    /**
     * A "comp" element of a node is parsed via this method.
     * Usually a component contains several (sub-)nodes.
     */
    bool componentArray(FamilyComponent* current_item, const QJsonArray &arr);

Q_SIGNALS:
    // This signal is emitted if files are removed from the family model
    // that does exist in the filesystem.
    void removed_existing_files(const QStringList& files);
    // This signal is emitted if files are rejected by the family model
    // although it does exist in the filesystem.
    void rejected_existing_files(const QDir &subdir, const QStringList& files);
    // This signal is emitted if files are added to the family model
    void added_files(const QStringList& files);
	void changed();
    void updateDependency(const QString& oldDepString, const QString& newDepString);
};
