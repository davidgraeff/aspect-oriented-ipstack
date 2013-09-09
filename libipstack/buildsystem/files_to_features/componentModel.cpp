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
#include <iostream>     // std::ios, std::istream, std::cout
#include <fstream>      // std::filebuf
#include <sstream>      // std::stringstream

ComponentModel::ComponentModel(const QString& base_directory, const QString &featureToFilesRelationfile, QObject *parent)
    : QAbstractItemModel(parent), base_directory(QDir(base_directory))
{
    rootItem = ComponentModelItem::createComponent(base_directory,0);

    // read feature_to_files_relation file
    std::filebuf fb;
    if (!fb.open (featureToFilesRelationfile.toLatin1().constData(),std::ios::in)) {
        std::cerr << "Failed to open featureToFilesRelation file!\n";
        exit(EXIT_FAILURE);
    }

    std::istream featureToFilesRelation_FileStream(&fb);
    picojson::value featureToFilesRelation;
    featureToFilesRelation_FileStream >> featureToFilesRelation;
    fb.close();

    std::string err = picojson::get_last_error();
    if (! err.empty()) {
        std::cerr << "Failed to parse featureToFilesRelation file!\n";
        std::cerr << err << std::endl;
        exit(EXIT_FAILURE);
    }

    // check if the type of the value is "object"
    if (! featureToFilesRelation.is<picojson::object>()) {
        std::cerr << "featureToFilesRelation::JSON is not an object" << std::endl;
        exit(EXIT_FAILURE);
    }

    node(rootItem, featureToFilesRelation.get<picojson::object>());
}

ComponentModel::~ComponentModel()
{
    delete rootItem;
}

void ComponentModel::save()
{

}

void ComponentModel::remove_non_existing_files()
{

}

QStringList ComponentModel::get_used_files()
{
    return rootItem->get_all_files();
}

QModelIndexList ComponentModel::get_missing_files()
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

QStringList ComponentModel::removeComponent(ComponentModelItem *c)
{
    QStringList l = c->get_all_files();
    beginResetModel();
    delete c;
    endResetModel();
    return l;
}

void ComponentModel::addComponent(ComponentModelItem *parent)
{
    ComponentModelItem::createComponent(QString(), parent);
}

QStringList ComponentModel::get_relative_dirs_list(QDir path)
{
    QStringList relative_dirs;
    while (path != base_directory) {
        relative_dirs.append(path.dirName());
        path.cdUp();
    }
    return relative_dirs;
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
    if (item->type == ComponentModelItem::TYPE)
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
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
        if (index.isValid()) {
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
        // info.fileName()
        //TODO
    }

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


bool ComponentModel::componentArray(ComponentModelItem *current_item, const picojson::value::array& obj) {
    bool result = true;
    for (picojson::value::array::const_iterator i = obj.begin(); i != obj.end(); ++i) {
        const picojson::value& value = *i;

        if (!value.is<picojson::object>()) {
            std::cerr << "[OBJECT] expected in comp" << std::endl;
            return false;
        }
        result &= node(current_item, value.get<picojson::object>());
        if (!result)
            return false;
    }
    return result;
}

QList<QString> ComponentModel::get_files(const picojson::value::object& obj) {
    QList<QString> files;
    if (obj.count("file")) {
        files.append(QString::fromStdString(obj.at("file").get<std::string>()));
    } else if (obj.count("files")) {
        const picojson::value::array& arr = obj.at("files").get<picojson::array>();
        for (picojson::value::array::const_iterator i = arr.begin(); i != arr.end(); ++i) {
            files.append(QString::fromStdString(i->get<std::string>()));
        }
    }
    return files;
}

QString ComponentModel::getMapString(const picojson::value::object& obj, std::string name) {
    auto i = obj.find(name);
    if (i==obj.end())
        return QString();
    else
        return QString::fromStdString(i->second.get<std::string>());
}

bool ComponentModel::node(ComponentModelItem *current_item, const picojson::value::object& obj) {
    bool result = true;
    // Determine (sub)directory
    QString subdir = getMapString(obj, "subdir");
    QDir new_dir(current_item->get_directory());
    if (subdir.startsWith("/"))
        subdir.remove(0,1);
    if (subdir.size())
        new_dir = new_dir.absoluteFilePath(subdir);

    // Create new component
    ComponentModelItem* new_item = ComponentModelItem::createComponent(new_dir,current_item);
    new_item->depends = getMapString(obj, "depends");
    new_item->vname = getMapString(obj, "vname");
    new_item->update_component_name();

    // Add files
    QList<QString> files = get_files(obj);
    for (auto i = files.begin(); i!=files.end();++i) {
         ComponentModelFileItem::createFile(*i, new_item);
    }

    // Subcomponents
    if (obj.count("comp")) {
        const picojson::value& value = obj.at("comp");
        if (!value.is<picojson::array>()) {
            std::cerr << "[ARRAY] expected as comp" << std::endl;
            return false;
        }
        result &= componentArray(new_item, value.get<picojson::array>());
    }

    return result;
}
