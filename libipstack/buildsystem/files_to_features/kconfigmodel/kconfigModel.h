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

class DependencyModel;
class DependencyModelItem {
public:
    QString name;
    QString text;
    QString description;
    DependencyModel* model;
    bool used;

    QString depends;
    int row;
    DependencyModelItem* parent;
    QList<DependencyModelItem*> childs;

    void insertSorted(DependencyModelItem* newItem) {
        childs.insert(binary_search_text_lower_bound(newItem->text), newItem);
    }

    DependencyModelItem() : used(false),row(-1) {}
    ~DependencyModelItem() {
        qDeleteAll(childs);
    }
private:
    int binary_search_text_lower_bound(const QString& text);
};

class DependencyModel : public QAbstractItemModel
{
	Q_OBJECT

public:
    DependencyModel(const QString& kconfig_input_filename, QObject *parent = 0);
    ~DependencyModel();

    void createModel();
    /**
     * @brief Mark all entries as unused again
     * This will not reread the file on disk. Use createModel() to synchronize
     * with the input file.
     */
    void resetAllUsed();
    /**
     * @brief Mark an entry as used.
     * @param Identify the model entry by specifing the dependencyName.
     */
    void markUsed(const QString& dependencyName, bool used);

    void updateDependency(const QString& oldDepString, const QString& newDepString);

    /// Return name of feature at position @index
    QString feature_name(const QModelIndex &current);
    QModelIndex indexOf(const QString& name);
    int count();
    int countUnused() { return count()-usedEntries; }
    DependencyModelItem* getRootItem();

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
					const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
private:
    DependencyModelItem *rootItem;
    QString kconfig_input_filename;
    DependencyModelItem* getItemByName(const QString& name);
    QMap<QString, DependencyModelItem*> itemList;
    unsigned usedEntries;
};
