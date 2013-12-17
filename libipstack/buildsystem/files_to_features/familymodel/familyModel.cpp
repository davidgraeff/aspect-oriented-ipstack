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

#include "familyModel.h"
#include "familyFile.h"
#include "familyComponent.h"
#include <QDirIterator>
#include <QDebug>
#include <QColor>
#include <QMimeData>
#include <QQueue>
#include <QTreeWidgetItemIterator>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>

Q_DECLARE_METATYPE(FamilyComponent*)

FamilyModel::FamilyModel(const QString& base_directory, const QString &featureToFilesRelationfile, QObject *parent)
    : QAbstractItemModel(parent), base_directory(QDir(base_directory)), featureToFilesRelationfile(featureToFilesRelationfile)
{
    rootItem = FamilyComponent::createComponent(this, base_directory,0);

    icon_missing = QIcon::fromTheme("dialog-warning");
    icon_component = QIcon::fromTheme("folder");
    icon_file = QIcon::fromTheme("text-x-generic");

    QJsonParseError err;
    QFile f(featureToFilesRelationfile);
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open featureToFilesRelation file!";
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(f.readAll(),&err);
    f.close();

    if (err.error != QJsonParseError::NoError) {
        qWarning() << err.errorString();
        return;
    }

    node(rootItem, jsonDoc.object());
}

FamilyModel::~FamilyModel()
{
    delete rootItem;
}

FamilyComponent *FamilyModel::getRootItem()
{
    return rootItem;
}

bool FamilyModel::save(QIODevice *device, bool freeAfterUse)
{
    if (device==0) {
        device = new QFile(featureToFilesRelationfile);
        device->open(QIODevice::WriteOnly);
        freeAfterUse = true;
    }

    if (!device->isOpen())
        return false;

    QJsonDocument jsonDoc;
    // if there is only one subitem use that as root
    if (rootItem->childs.size()==1 && rootItem->childs[0]->type==FamilyComponent::TYPE) {
        QJsonObject rootJson;
        static_cast<FamilyComponent*>(rootItem->childs[0])->toJSon(rootJson);
        jsonDoc.setObject(rootJson);
    } else {// for more items use an array in an object
        QJsonObject rootJson;
        rootItem->toJSon(rootJson);
        jsonDoc.setObject(rootJson);
    }
    device->write(jsonDoc.toJson().replace("    ","\t"));

    device->close();
    if (freeAfterUse)
        delete device;

    return true;
}

QStringList FamilyModel::get_used_files() const
{
    return rootItem->get_all_files(true);
}

QStringList FamilyModel::get_dependencies() const
{
    QStringList d;
    rootItem->get_dependencies_string(d);
    return d;
}

QModelIndexList FamilyModel::get_missing_files() const
{
    QModelIndexList list;
    QQueue<QModelIndex> queue;

    // add initial items to queue
    int rows = rowCount();
    for (int i=0;i<rows;++i)
        queue.enqueue(index(i,0));

    while (queue.size()) {
        // analyse current queue item
        QModelIndex current_index = queue.dequeue();
        // if current_index is a file item and is missing from the file system:
        if (static_cast<FamilyBaseItem*>(current_index.internalPointer())->type==FamilyFile::TYPE &&
                static_cast<FamilyFile*>(current_index.internalPointer())->not_exist) {
            // add to result list
            list.append(current_index);
        }
        // Add childs to queue
        rows = rowCount(current_index);
        for (int i=0;i<rows;++i)
            queue.enqueue(index(i,0, current_index));
    }

    return list;
}

QString FamilyModel::relative_directory(const QString &absolute_path)
{
    return base_directory.relativeFilePath(absolute_path);
}

void FamilyModel::update(FamilyBaseItem *item)
{
    Q_ASSERT(item);
    Q_ASSERT(item!=rootItem);
    QModelIndex index = createIndex(item->getRow(),0,item);
    emit dataChanged(index, index);
	emit changed();
}

QModelIndex FamilyModel::indexOf(FamilyBaseItem *item) const
{
    if (!item)
        return QModelIndex();
    return createIndex(item->getRow(),0,item);
}


void FamilyModel::removeComponent(FamilyComponent *item)
{
    Q_ASSERT(item);
    QStringList l = item->get_all_files(true);
    QModelIndex index = createIndex(item->getRow(),0,item);
    beginRemoveRows(index.parent(),index.row(),index.row());
    // Remove from parent
    {
        FamilyBaseItem* parent = item->parent;
        if (parent)
            parent->childs.removeOne(item);
    }
    // Remote item and childs
    delete item;
    endRemoveRows();
    emit removed_existing_files(l);
	emit changed();
}

void FamilyModel::removeFile(FamilyFile *item)
{
    Q_ASSERT(item);
    // Return file name
    QStringList l;
    if (!item->not_exist)
        l << item->get_full_path();

    QModelIndex index = createIndex(item->getRow(),0,(void*)item);
    beginRemoveRows(index.parent(),index.row(),index.row());
    // Remove from parent
    {
        FamilyBaseItem* parent = item->parent;
        if (parent)
            parent->childs.removeOne(item);
    }
    // Remote item and childs
//    qDebug() << "removeFile" << item->filename;
    delete item;
    endRemoveRows();
    emit removed_existing_files(l);
	emit changed();
}

FamilyComponent* FamilyModel::addComponent(FamilyComponent *parent)
{
    if (!parent)
        parent = rootItem;

    FamilyComponent* i = FamilyComponent::createComponent(this, parent->get_directory(), parent);
	emit changed();
    return i;
}

void FamilyModel::clear(bool removeEntries)
{
    beginResetModel();
    if (removeEntries) {
        qDeleteAll(rootItem->childs);
        rootItem->childs.clear();
    }
    endResetModel();
	emit changed();
}

int FamilyModel::columnCount(const QModelIndex &) const
{
    return 2;
}

Qt::ItemFlags FamilyModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if (!index.isValid())
        return defaultFlags | Qt::ItemIsDropEnabled;

    FamilyBaseItem *item = static_cast<FamilyBaseItem*>(index.internalPointer());
    if (item->type == FamilyComponent::TYPE) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    } else
        return Qt::ItemIsDragEnabled | defaultFlags;
}

bool FamilyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // First determine the parent FileModel item
    FamilyComponent *item;
    if (parent.isValid())
        item = static_cast<FamilyComponent*>(parent.internalPointer());
    else
        item = rootItem;
    Q_ASSERT(item);

    // Start removing from parent item
    for (int r=row+count-1;r>=row;--r) {
        if (item->childs[r]->type == FamilyComponent::TYPE)
            removeComponent((FamilyComponent*)item->childs[r]);
        else
            removeFile((FamilyFile*)item->childs[r]);
    }
    return true;
}

Qt::DropActions FamilyModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

Qt::DropActions FamilyModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

QStringList FamilyModel::mimeTypes() const
{
    QStringList types;
    types << "application/files.pointer.list";
    types << "text/uri-list";
    return types;
}

QMimeData *FamilyModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    const QByteArray encodedData;

    moveFilesList.clear();
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid() && index.column()==0) {
            FamilyBaseItem *item = static_cast<FamilyBaseItem*>(index.internalPointer());
            if (item->type == FamilyFile::TYPE)
                moveFilesList << static_cast<FamilyFile*>(item);
        }
    }

    mimeData->setData("application/files.pointer.list", encodedData);
    return mimeData;
}

bool FamilyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row); Q_UNUSED(column);
    if (action == Qt::IgnoreAction)
        return true;

    FamilyBaseItem *item = static_cast<FamilyBaseItem*>(parent.internalPointer());
    if (!item)
        item = rootItem;
    if (item->type==FamilyFile::TYPE)
        item = item->parent;
    Q_ASSERT(item);
    FamilyComponent* target = (FamilyComponent*)item;

    if (data->hasFormat("application/files.pointer.list")) {
        QStringList files;
        foreach(FamilyFile* source_file_entry, moveFilesList) {
            files << source_file_entry->get_full_path();
            removeFile(source_file_entry);
        }

        moveFilesList.clear();
        target->addFiles(files, FamilyComponent::AllowOneSubdirCreateSubcomponents);

        // If we return true, Qt automatically calls removeRows of the source model
        return false;
    } else if (data->hasFormat("text/uri-list") && action == Qt::CopyAction) {
        QList<QUrl> urls = data->urls();
        QStringList files;
        foreach(const QUrl& url, urls) {
            files << url.toLocalFile();
        }

        target->addFiles(files, FamilyComponent::AllowOneSubdirCreateSubcomponents);

        // If we return true, Qt automatically calls removeRows of the source model
        return true;
    }

    return false;
}

QVariant FamilyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    FamilyBaseItem *item = static_cast<FamilyBaseItem*>(index.internalPointer());
    const int column = index.column();
    if (item->type == FamilyFile::TYPE) {
        if (column==0) {
            if (role==Qt::DisplayRole || role==Qt::ToolTipRole) {
                return static_cast<FamilyFile*>(item)->filename;
            } else if (role==Qt::ForegroundRole && static_cast<FamilyFile*>(item)->not_exist) {
                return QColor(Qt::red);
            } else if (role==Qt::DecorationRole) {
                if (static_cast<FamilyFile*>(item)->not_exist)
                    return icon_missing;
                else
                    return icon_file;
            }
        }

    } else {
        FamilyComponent* mItem = (FamilyComponent*)item;
        if (role==Qt::DisplayRole) {
            if (column==0) {
                return mItem->cache_component_name;
            } else {
                return mItem->cache_relative_directory;
            }
        } else if (role==Qt::ToolTipRole && column==0) {
            return mItem->get_dependencies();
        } else if (role==Qt::DecorationRole && column==0) {
            return icon_component;
        }
    }
    return QVariant();
}

QVariant FamilyModel::headerData(int section, Qt::Orientation orientation,
                            int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Name (Dependencies)");
            case 1: return tr("Directory");
        }
    }
    else if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case 0: return tr("Filename");
        }
    }
    return QVariant();
}

QModelIndex FamilyModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    FamilyBaseItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FamilyBaseItem*>(parent.internalPointer());

    FamilyBaseItem *childItem = parentItem->childs.at(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex FamilyModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    FamilyBaseItem *childItem = static_cast<FamilyBaseItem*>(index.internalPointer());
    FamilyBaseItem *parentItem = childItem->parent;

    if (parentItem == rootItem)
        return QModelIndex();

    Q_ASSERT(parentItem);
    return createIndex(parentItem->getRow(), 0, parentItem);
}

int FamilyModel::rowCount(const QModelIndex &parent) const
{
    FamilyBaseItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FamilyBaseItem*>(parent.internalPointer());

    return parentItem->childs.size();
}


bool FamilyModel::componentArray(FamilyComponent *current_item, const QJsonArray& arr) {
    bool result = true;
    for (auto i = arr.begin(); i!=arr.end();++i) {
        const QJsonValue& value = *i;

        if (!value.isObject()) {
            qWarning() << "[OBJECT] expected in comp";
            return false;
        }
        result &= node(current_item, value.toObject());
        if (!result)
            return false;
    }
    return result;
}

bool FamilyModel::node(FamilyComponent *current_item, const QJsonObject& obj) {
    bool result = true;
    // Determine (sub)directory
    QString subdir = obj.value("subdir").toString();
    QDir new_dir(current_item->get_directory());
    if (subdir.startsWith("/"))
        subdir.remove(0,1);
    if (subdir.size())
        new_dir = new_dir.absoluteFilePath(subdir);

    // Create new component
    FamilyComponent* new_item = FamilyComponent::createComponent(this, new_dir,current_item,
                                                                 obj.value("depends").toString(),
                                                                 obj.value("vname").toString());
    // Add files
    QJsonArray files;
    if (obj.contains("file"))
        files.append(obj.value("file").toString());
    else if (obj.contains("files"))
        files = obj.value("files").toArray();

    for (auto i = files.begin(); i!=files.end();++i) {
        FamilyFile::createFile(this, (*i).toString(), new_item);
    }

    // Subcomponents
    if (obj.contains("comp")) {
        result &= componentArray(new_item, obj.value("comp").toArray());
    }

    return result;
}
