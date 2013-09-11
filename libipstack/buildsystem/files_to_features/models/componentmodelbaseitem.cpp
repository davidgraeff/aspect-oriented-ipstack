#include "componentmodelbaseitem.h"
#include "componentModelItem.h"

ComponentModelBaseItem::ComponentModelBaseItem()
{
}

int ComponentModelBaseItem::getRow() {
    if (parent) {
        return parent->childs.indexOf(this);
    } else
        return 0;
}

ComponentModelBaseItem::~ComponentModelBaseItem() {
    foreach(ComponentModelBaseItem* child, childs)
        child->parent = 0;
    qDeleteAll(childs);
    childs.clear();
}
