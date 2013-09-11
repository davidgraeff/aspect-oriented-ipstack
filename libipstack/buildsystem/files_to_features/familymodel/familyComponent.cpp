#include "familyComponent.h"
#include "familyFile.h"
#include "familyModel.h"
#include <QJsonArray>
#include <QDebug>

FamilyComponent::FamilyComponent(FamilyModel *componentModel) : FamilyBaseItem(componentModel)
{

}

FamilyComponent* FamilyComponent::createComponent(FamilyModel *componentModel, const QDir &absolut_directory, FamilyComponent* parent) {
    FamilyComponent* item = new FamilyComponent(componentModel);
    item->set_directory(absolut_directory,false);
    item->parent = parent;
    item->type = FamilyComponent::TYPE;

    int c=0;
    if (parent) {
        QModelIndex parentModelIndex = componentModel->createIndex(parent->getRow(),0,parent);

        QString cache_relative_dir = absolut_directory.dirName();
        // Insertion sort iteration step
        for (int m=parent->childs.size();c<m;++c) {
            FamilyBaseItem* base = parent->childs[c];
            if (base->type == FamilyFile::TYPE)
                break; // stop on first file item, we insert before

            if (static_cast<FamilyComponent*>(base)->cache_relative_directory >= cache_relative_dir)
                break;
        }
        componentModel->beginInsertRows(parentModelIndex,c,c);
        parent->childs.insert(c, item);
        componentModel->endInsertRows();
    }

    return item;
}

void FamilyComponent::update_component_name()
{
    cache_component_name = vname;
    if (cache_component_name.isEmpty())
        cache_component_name = depends.left(40);
    else if (depends.size())
        cache_component_name += " (" + depends.left(40) + ")";

    if (cache_component_name.isEmpty())
        cache_component_name = "[unnamed component]";
}

void FamilyComponent::toJSon(QJsonObject &jsonObject)
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
    foreach(FamilyBaseItem* subitem, childs) {
        if (subitem->type == FamilyFile::TYPE) {
            files.append(static_cast<FamilyFile*>(subitem)->filename);
        } else {
            QJsonObject subObj;
            static_cast<FamilyComponent*>(subitem)->toJSon(subObj);
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

QList<FamilyFile*> FamilyComponent::addFiles(const QStringList &files, FamilyComponent::add_files_enum subdirFlag)
{
    QList<FamilyFile*> addedItems;
    QStringList rejected;
    foreach(const QString& file, files) {
        QString relative_filepath = directory.relativeFilePath(file);
        if (relative_filepath.startsWith("../")) {
            rejected << file;
        } else {
            if (subdirFlag==AllowOneSubdirCreateSubcomponents) {
                FamilyComponent* subcomp = findOrCreateRecursivly(this, QFileInfo(relative_filepath).path());
                Q_ASSERT(subcomp);
                qDebug() << "addfile: " << subcomp->cache_component_name << QFileInfo(relative_filepath).fileName();
                addedItems << FamilyFile::createFile(componentModel,QFileInfo(relative_filepath).fileName(),subcomp);
            } else if (subdirFlag == AllowMultipleSubdirLevels) {
                addedItems << FamilyFile::createFile(componentModel,relative_filepath,this);
            } else {
                Q_ASSERT(1==0);
            }
        }
    }

    if (rejected.size())
        emit componentModel->rejected_existing_files(directory, rejected);

    return addedItems;
}

FamilyComponent *FamilyComponent::findOrCreateRecursivly(FamilyComponent *current, QString relative_path)
{
    if (relative_path.isEmpty() || relative_path=="/" || relative_path==".")
        return current;

    int temp = relative_path.indexOf("/");
    QString next_subdir;
    // Determine next sub dir.
    if (temp==-1) {
        next_subdir = relative_path;
        relative_path = QString();
    } else {
        next_subdir = relative_path.mid(0,temp);
        relative_path = relative_path.mid(temp+1);
    }

//    qDebug() << "findOrCreateRecursivly" << relative_path << "next" << next_subdir;

    // Try to find a subcomponent with the desired subdirectory part stored in next_subdir.
    QString desiredDir = current->directory.absoluteFilePath(next_subdir);
    FamilyComponent* findExisting = 0;
    foreach(FamilyBaseItem* subitem, current->childs) {
        if (subitem->type == FamilyComponent::TYPE) {
            FamilyComponent* subComponent = static_cast<FamilyComponent*>(subitem);
            // Skip existing components with dependencies
            if (subComponent->depends.size())
                continue;
            if (subComponent->get_directory()==desiredDir) {
                findExisting = subComponent;
                break;
            }
        }
    }
    if (findExisting)
        return findOrCreateRecursivly(findExisting, relative_path);

    // No existing component found, create one
    current = createComponent(current->componentModel,desiredDir,current);
    current->vname = next_subdir;
    current->update_component_name();
    return findOrCreateRecursivly(current, relative_path);
}

QStringList FamilyComponent::get_all_files(bool only_existing, bool remove_files) {
    QStringList f;

    for(int i=0;i<childs.size();++i) {
        FamilyBaseItem* child = childs[i];
        if (child->type==FamilyComponent::TYPE) {
            f << static_cast<FamilyComponent*>(child)->get_all_files(only_existing);
        } else if (child->type==FamilyFile::TYPE) {
            FamilyFile* fileitem = static_cast<FamilyFile*>(child);
            if (!only_existing || !fileitem->not_exist)
                f << (directory.absoluteFilePath(fileitem->filename));
            if (remove_files) {
                childs.removeAt(i);
                delete fileitem;
                --i;
            }
        }
    }

    return f;
}

void FamilyComponent::set_directory(const QDir& absolut_dir, bool removeSubFiles) {
    if (removeSubFiles) {
        QStringList files;
        // Remove all childs
        files << get_all_files(true, true);
        qDeleteAll(childs);
        childs.clear();
        emit componentModel->removed_existing_files(files);
    }

    directory = absolut_dir;
    cache_relative_directory = componentModel->relative_directory(absolut_dir.absolutePath());
}

QDir FamilyComponent::get_directory() const
{
    return directory;
}

QString FamilyComponent::get_dependencies() const {
    return depends;
}

void FamilyComponent::set_dependencies(const QString &d) {
    depends = d;
}
