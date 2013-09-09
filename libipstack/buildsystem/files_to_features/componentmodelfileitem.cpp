#include "componentmodelfileitem.h"
#include "componentModelItem.h"
#include <QDebug>

ComponentModelFileItem::ComponentModelFileItem()
{
}

QString ComponentModelFileItem::get_full_path()
{
    if (parent)
        return static_cast<ComponentModelItem*>(parent)->directory.absoluteFilePath(filename);
    else
        return filename;
}

ComponentModelFileItem *ComponentModelFileItem::createFile(const QString& filename, ComponentModelItem* parent) {
    int c=0;
    // Insertion sort iteration step
    for (int m=parent->childs.size();c<m;++c) {
        ComponentModelBaseItem* base = parent->childs[c];
        if (base->type != ComponentModelFileItem::TYPE)
            continue; // ignore components

        const QString& existing_filename = static_cast<ComponentModelFileItem*>(base)->filename;
        if (existing_filename == filename)
            return 0; // already inside
        if (existing_filename > filename)
            break;
    }
    ComponentModelFileItem* item = new ComponentModelFileItem();
    parent->childs.insert(c, item);

    item->type = ComponentModelFileItem::TYPE;
    item->filename = filename;
    item->parent = parent;
    item->not_exist = !QFile(item->get_full_path()).exists();
    return item;
}
