#include "familyBaseItem.h"
#include "familyComponent.h"

FamilyBaseItem::FamilyBaseItem(FamilyModel* componentModel) : componentModel(componentModel), parent(0)
{
}

int FamilyBaseItem::getRow() const {
    if (parent) {
        return parent->childs.indexOf((FamilyBaseItem*)this);
    } else
        return 0;
}

const QList<FamilyBaseItem *> &FamilyBaseItem::getChilds() const
{
    return childs;
}

QList<FamilyBaseItem *> &FamilyBaseItem::getChilds()
{
    return childs;
}

FamilyBaseItem::~FamilyBaseItem() {
    foreach(FamilyBaseItem* child, childs)
        child->parent = 0;
    qDeleteAll(childs);
    childs.clear();
}
