#include "familyFile.h"
#include "familyComponent.h"
#include "familyModel.h"
#include <QDebug>

FamilyFile::FamilyFile(FamilyModel *componentModel) : FamilyBaseItem(componentModel)
{
}

QString FamilyFile::get_full_path() const
{
    if (parent)
        return static_cast<FamilyComponent*>(parent)->get_directory().absoluteFilePath(filename);
    else
        return filename;
}

FamilyFile *FamilyFile::createFile(FamilyModel *componentModel, const QString& filename, FamilyComponent* parent) {
    QModelIndex parentModelIndex = componentModel->createIndex(parent->getRow(),0,parent);
    int c=0;
    // Insertion sort iteration step
    for (int m=parent->childs.size();c<m;++c) {
        FamilyBaseItem* base = parent->childs[c];
        if (base->type != FamilyFile::TYPE)
            continue; // ignore components

        const QString& existing_filename = static_cast<FamilyFile*>(base)->filename;
        if (existing_filename == filename)
            return 0; // already inside
        if (existing_filename > filename)
            break;
    }
    FamilyFile* item = new FamilyFile(componentModel);
    componentModel->beginInsertRows(parentModelIndex,c,c);
    parent->childs.insert(c, item);
    componentModel->endInsertRows();

    item->type = FamilyFile::TYPE;
    item->filename = filename;
    item->parent = parent;
    item->not_exist = !QFile(item->get_full_path()).exists();
    return item;
}
