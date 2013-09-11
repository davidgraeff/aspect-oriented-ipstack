#include "familyBaseItem.h"
#include "familyComponent.h"

FamilyBaseItem::FamilyBaseItem(FamilyModel* componentModel) : componentModel(componentModel), parent(0)
{
}

int FamilyBaseItem::getRow() {
    if (parent) {
        return parent->childs.indexOf(this);
    } else
        return 0;
}

FamilyBaseItem::~FamilyBaseItem() {
    foreach(FamilyBaseItem* child, childs)
        child->parent = 0;
    qDeleteAll(childs);
    childs.clear();
}
