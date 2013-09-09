#include "componentModelItem.h"
#include "componentmodelfileitem.h"

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
        cache_component_name = "[component]";
}

QStringList ComponentModelItem::get_all_files() {
    QStringList f;
    foreach(ComponentModelBaseItem* child, childs) {
        if (child->type==ComponentModelItem::TYPE) {
            f << static_cast<ComponentModelItem*>(child)->get_all_files();
        } else if (child->type==ComponentModelFileItem::TYPE) {
            f << (directory.absoluteFilePath(static_cast<ComponentModelFileItem*>(child)->filename));
        }
    }
    return f;
}

QStringList ComponentModelItem::set_directory(const QString& absolut_dir) {
    // Remove all childs
    QStringList files;
    files << get_all_files();
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
