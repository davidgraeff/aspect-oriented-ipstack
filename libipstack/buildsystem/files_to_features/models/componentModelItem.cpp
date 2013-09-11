#include "componentModelItem.h"
#include "componentmodelfileitem.h"
#include "componentModel.h"
#include <QJsonArray>
#include <QDebug>

ComponentModelItem* ComponentModelItem::createComponent(const QDir &absolut_directory, ComponentModelItem* parent) {
    ComponentModelItem* item = new ComponentModelItem();
    item->directory = absolut_directory;
    item->cache_relative_directory = absolut_directory.dirName();
    item->parent = parent;
    item->type = ComponentModelItem::TYPE;

    int c=0;
    if (parent) {
        QString cache_relative_dir = absolut_directory.dirName();
        // Insertion sort iteration step
        for (int m=parent->childs.size();c<m;++c) {
            ComponentModelBaseItem* base = parent->childs[c];
            if (base->type == ComponentModelFileItem::TYPE)
                break; // stop on first file item, we insert before

            ComponentModelItem* item = (ComponentModelItem*)base;
            if (item->cache_relative_directory >= cache_relative_dir)
                break;
        }
        parent->childs.insert(c, item);
    }

    return item;
}

void ComponentModelItem::update_component_name()
{
    cache_component_name = vname;
    if (cache_component_name.isEmpty())
        cache_component_name = depends.left(40);
    else if (depends.size())
        cache_component_name += " (" + depends.left(40) + ")";

    if (cache_component_name.isEmpty())
        cache_component_name = "[unnamed component]";
}

void ComponentModelItem::toJSon(QJsonObject &jsonObject, ComponentModel *componentModel)
{
    if (vname.size())
        jsonObject.insert("vname", vname);
    if (depends.size())
        jsonObject.insert("depends", depends);
    QString relative_dir = componentModel->relative_directory(directory.absolutePath());
    if (relative_dir.size()>1)
        jsonObject.insert("subdir", relative_dir);

    QJsonArray files;
    QJsonArray subcomponents;
    foreach(ComponentModelBaseItem* subitem, childs) {
        if (subitem->type == ComponentModelFileItem::TYPE) {
            files.append(static_cast<ComponentModelFileItem*>(subitem)->filename);
        } else {
            QJsonObject subObj;
            static_cast<ComponentModelItem*>(subitem)->toJSon(subObj, componentModel);
            subcomponents.append(subObj);
        }
    }

    // files
    if (files.size()) {
        if (files.size()==1)
            jsonObject.insert("file", files[0]);
        else
            jsonObject.insert("files", files);
    }
    // components
    if (subcomponents.size())
        jsonObject.insert("comp", subcomponents);
}

void ComponentModelItem::addFiles(const QStringList &files, ComponentModelItem::add_files_enum subdirFlag)
{
    qDebug() << "addFiles" << cache_component_name << files;
}

QStringList ComponentModelItem::get_all_files(bool only_existing) {
    QStringList f;
    foreach(ComponentModelBaseItem* child, childs) {
        if (child->type==ComponentModelItem::TYPE) {
            f << static_cast<ComponentModelItem*>(child)->get_all_files(only_existing);
        } else if (child->type==ComponentModelFileItem::TYPE) {
            ComponentModelFileItem* fileitem = static_cast<ComponentModelFileItem*>(child);
            if (!only_existing || !fileitem->not_exist)
                f << (directory.absoluteFilePath(fileitem->filename));
        }
    }
    return f;
}

QStringList ComponentModelItem::set_directory(const QString& absolut_dir) {
    // Remove all childs
    QStringList files;
    files << get_all_files(true);
    qDeleteAll(childs);
    childs.clear();

    directory = QDir(absolut_dir);
    cache_relative_directory = QFileInfo(absolut_dir).absoluteDir().dirName();

    return files;
}

QString ComponentModelItem::get_directory()
{
    return directory.absolutePath();
}
