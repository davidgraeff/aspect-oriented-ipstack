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

#include "componentModel.h"
#include "componentModelItem.h"
#include "componentmodelfileitem.h"
#include <QDirIterator>
#include <QDebug>
#include <QColor>
#include <QMimeData>
#include <QQueue>
#include <QTreeWidgetItemIterator>
#include <QJsonDocument>
#include <QJsonArray>

ComponentModel::ComponentModel(const QString& base_directory, const QString &featureToFilesRelationfile, QObject *parent)
    : QAbstractItemModel(parent), base_directory(QDir(base_directory)), featureToFilesRelationfile(featureToFilesRelationfile)
{
    rootItem = ComponentModelItem::createComponent(base_directory,0);

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

ComponentModel::~ComponentModel()
{
    delete rootItem;
}

ComponentModelItem *ComponentModel::getRootItem()
{
    return rootItem;
}

bool ComponentModel::save(QIODevice *device, bool freeAfterUse)
{
    if (device==0) {
        device = new QFile(featureToFilesRelationfile+".try");
        device->open(QIODevice::WriteOnly);
        freeAfterUse = true;
    }

    if (!device->isOpen())
        return false;

    QJsonDocument jsonDoc;
    // if there is only one subitem use that as root
    if (rootItem->childs.size()==1 && rootItem->childs[0]->type==ComponentModelItem::TYPE) {
        QJsonObject rootJson;
        static_cast<ComponentModelItem*>(rootItem->childs[0])->toJSon(rootJson, this);
        jsonDoc.setObject(rootJson);
    } else {// for more items use an array in an object
        QJsonObject rootJson;
        rootItem->toJSon(rootJson, this);
        jsonDoc.setObject(rootJson);
    }
    device->write(jsonDoc.toJson().replace("    ","\t"));

    device->close();
    if (freeAfterUse)
        delete device;

    return true;
}

QStringList ComponentModel::get_used_files() const
{
    return rootItem->get_all_files(true);
}

QModelIndexList ComponentModel::get_missing_files() const
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
        if (static_cast<ComponentModelBaseItem*>(current_index.internalPointer())->type==ComponentModelFileItem::TYPE &&
                static_cast<ComponentModelFileItem*>(current_index.internalPointer())->not_exist) {
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

QString ComponentModel::relative_directory(const QString &absolute_path)
{
    QString b = base_directory.absolutePath();
    if (absolute_path.startsWith(b))
        return absolute_path.mid(b.size());
    return absolute_path;
}

void ComponentModel::update(ComponentModelBaseItem *item)
{
    Q_ASSERT(item);
    Q_ASSERT(item!=rootItem);
    QModelIndex index = createIndex(item->getRow(),0,item);
    emit dataChanged(index, index);
}

QModelIndex ComponentModel::indexOf(ComponentModelBaseItem *item) const
{
    if (!item)
        return QModelIndex();
    return createIndex(item->getRow(),0,item);
}


void ComponentModel::removeComponent(ComponentModelItem *item)
{
    Q_ASSERT(item);
    QStringList l = item->get_all_files(true);
    QModelIndex index = createIndex(item->getRow(),0,item);
    beginRemoveRows(index.parent(),index.row(),index.row());
    // Remove from parent
    {
        ComponentModelBaseItem* parent = item->parent;
        if (parent)
            parent->childs.removeOne(item);
    }
    // Remote item and childs
    delete item;
    endRemoveRows();
    emit removed_existing_files(l);
}

void ComponentModel::removeFile(ComponentModelFileItem *item)
{
    Q_ASSERT(item);
    // Return file name
    QStringList l;
    if (!item->not_exist)
        l << item->get_full_path();

    QModelIndex index = createIndex(item->getRow(),0,item);
    beginRemoveRows(index.parent(),index.row(),index.row());
    // Remove from parent
    {
        ComponentModelBaseItem* parent = item->parent;
        if (parent)
            parent->childs.removeOne(item);
    }
    // Remote item and childs
    delete item;
    endRemoveRows();
    emit removed_existing_files(l);
}

ComponentModelItem* ComponentModel::addComponent(ComponentModelItem *parent)
{
    if (!parent)
        parent = rootItem;

    ComponentModelItem* i = ComponentModelItem::createComponent(parent->get_directory(), parent);
    i->update_component_name();
//    QModelIndex index = createIndex(i->getRow(),0,i);
//    beginInsertRows(index.parent(),index.row(),index.row());
//    endInsertRows();
//    emit dataChanged(index.parent(),index);
    beginResetModel();
    endResetModel();
    return i;
}

void ComponentModel::clear(bool removeEntries)
{
    beginResetModel();
    if (removeEntries) {
        qDeleteAll(rootItem->childs);
        rootItem->childs.clear();
    }
    endResetModel();
}

int ComponentModel::columnCount(const QModelIndex &) const
{
    return 2;
}

Qt::ItemFlags ComponentModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if (!index.isValid())
        return defaultFlags | Qt::ItemIsDropEnabled;

    ComponentModelBaseItem *item = static_cast<ComponentModelBaseItem*>(index.internalPointer());
    if (item->type == ComponentModelItem::TYPE) {
        return Qt::ItemIsDropEnabled | defaultFlags;
    } else
        return Qt::ItemIsDragEnabled | defaultFlags;
}

bool ComponentModel::removeRows(int row, int count, const QModelIndex &parent)
{
    ComponentModelItem *item = static_cast<ComponentModelItem*>(parent.internalPointer());
    qDebug() << "remove" << item->cache_component_name << row;
    for (int r=row+count-1;r>=row;--r) {
        if (item->childs[r]->type == ComponentModelItem::TYPE)
            removeComponent((ComponentModelItem*)item->childs[r]);
        else
            removeFile((ComponentModelFileItem*)item->childs[r]);
    }
    return true;
}

Qt::DropActions ComponentModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::DropActions ComponentModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

QStringList ComponentModel::mimeTypes() const
{
    QStringList types;
    types << "application/files.text.list";
    return types;
}

QMimeData *ComponentModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (const QModelIndex &index, indexes) {
        if (index.isValid() && index.column()==0) {
            ComponentModelBaseItem *item = static_cast<ComponentModelBaseItem*>(index.internalPointer());
            if (item->type == ComponentModelFileItem::TYPE)
                stream << static_cast<ComponentModelFileItem*>(item)->get_full_path();
        }
    }

    mimeData->setData("application/files.text.list", encodedData);
    return mimeData;
}

bool ComponentModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row); Q_UNUSED(column);
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("application/files.text.list"))
        return false;

    QByteArray encodedData = data->data("application/files.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    ComponentModelBaseItem *item = static_cast<ComponentModelBaseItem*>(parent.internalPointer());
    if (item->type==ComponentModelFileItem::TYPE)
        item = item->parent;
    ComponentModelItem* target = (ComponentModelItem*)item;

    QStringList files;
    QString file;
    while (!stream.atEnd()) {
        stream >> file;
        files << file;
    }

    target->addFiles(files, ComponentModelItem::AllowOneSubdirCreateSubcomponents);
    emit dataChanged(parent, index(row,column,parent));

    // If we return true, Qt automatically calls removeRows of the source model
    return true;
}

QVariant ComponentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ComponentModelBaseItem *item = static_cast<ComponentModelBaseItem*>(index.internalPointer());
    const int column = index.column();
    if (item->type == ComponentModelFileItem::TYPE) {
        if (column==0) {
            if (role==Qt::DisplayRole || role==Qt::ToolTipRole) {
                return static_cast<ComponentModelFileItem*>(item)->filename;
            } else if (role==Qt::ForegroundRole && static_cast<ComponentModelFileItem*>(item)->not_exist) {
                return QColor(Qt::red);
            }
        }

    } else {
        ComponentModelItem* mItem = (ComponentModelItem*)item;
        if (role==Qt::DisplayRole) {
            if (column==0) {
                return mItem->cache_component_name;
            } else {
                return mItem->cache_relative_directory;
            }
        } else if (role==Qt::ToolTipRole) {
            return mItem->depends;
        }
    }
    return QVariant();
}

QVariant ComponentModel::headerData(int section, Qt::Orientation orientation,
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

QModelIndex ComponentModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ComponentModelBaseItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ComponentModelBaseItem*>(parent.internalPointer());

    ComponentModelBaseItem *childItem = parentItem->childs.at(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ComponentModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ComponentModelBaseItem *childItem = static_cast<ComponentModelBaseItem*>(index.internalPointer());
    ComponentModelBaseItem *parentItem = childItem->parent;

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->getRow(), 0, parentItem);
}

int ComponentModel::rowCount(const QModelIndex &parent) const
{
    ComponentModelBaseItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ComponentModelBaseItem*>(parent.internalPointer());

    return parentItem->childs.size();
}


bool ComponentModel::componentArray(ComponentModelItem *current_item, const QJsonArray& arr) {
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

bool ComponentModel::node(ComponentModelItem *current_item, const QJsonObject& obj) {
    bool result = true;
    // Determine (sub)directory
    QString subdir = obj.value("subdir").toString();
    QDir new_dir(current_item->get_directory());
    if (subdir.startsWith("/"))
        subdir.remove(0,1);
    if (subdir.size())
        new_dir = new_dir.absoluteFilePath(subdir);

    // Create new component
    ComponentModelItem* new_item = ComponentModelItem::createComponent(new_dir,current_item);
    new_item->depends = obj.value("depends").toString();
    new_item->vname = obj.value("vname").toString();
    new_item->update_component_name();

    // Add files
    QJsonArray files;
    if (obj.contains("file"))
        files.append(obj.value("file").toString());
    else if (obj.contains("files"))
        files = obj.value("files").toArray();

    for (auto i = files.begin(); i!=files.end();++i) {
        ComponentModelFileItem::createFile((*i).toString(), new_item);
    }

    // Subcomponents
    if (obj.contains("comp")) {
        result &= componentArray(new_item, obj.value("comp").toArray());
    }

    return result;
}
