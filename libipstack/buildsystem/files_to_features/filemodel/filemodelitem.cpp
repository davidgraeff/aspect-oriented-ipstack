#include "filemodelitem.h"
#include "fileModel.h"
#include <QDebug>

int FileModelItem::getRow() {
    if (parent) {
        return parent->childs.indexOf(this);
    } else
        return 0;
}

QString FileModelItem::full_absolute_path() const {
    if (parent)
        return parent->full_absolute_path() + "/" + name;
    else
        return name;
}

FileModelItem::~FileModelItem() {
    // Remove from files hash list
    if (isFile) {
        filemodel->all_files.remove(name, this);
    }
    qDeleteAll(childs);
}

FileModelItem *FileModelItem::createFile(FileModel *filemodel, const QString &name, FileModelItem *parent) {
    FileModelItem* item = new FileModelItem();
    item->filemodel = filemodel;
    item->name = name; item->isFile = true;
    if (parent && parent->getItemByName(name)) { // already a child with this filename in parent
        delete item;
        return 0;
    }
    item->parent = parent;
    if (parent) {
        parent->addChild(item);
    }
    return item;
}

FileModelItem *FileModelItem::createDir(FileModel *filemodel, const QString &name, FileModelItem *parent) {
    FileModelItem* item = new FileModelItem();
    item->filemodel = filemodel;
    item->name = name; item->isFile = false;
    item->parent = parent;
    if (parent) {
        parent->addChild(item);
    }
    return item;
}

void FileModelItem::addChild(FileModelItem *item) {
    childs.insert(binary_search(item->name,true),item);
}

FileModelItem *FileModelItem::getItemByName(const QString &filename) {
    return childs.value(binary_search(filename), 0);
}

QStringList FileModelItem::get_all_files() const
{
    QStringList files;
    if (isFile)
        files << full_absolute_path();
    else {
        foreach (const FileModelItem* item, childs) {
            files << item->get_all_files();
        }
    }
    return files;
}

int FileModelItem::binary_search(const QString &filename, bool lower_bound_only) {
    int half=0, begin=0, middle=0, n = childs.size();
    while (n>0) {
        half = n >> 1;
        middle = begin + half;
        if (childs[middle]->name < filename) {
            begin = middle + 1;
            n -= half + 1;
        } else {
            n = half;
        }
    }
    // begin is lower bound or element
    if (lower_bound_only)
        return begin;

    if (begin >= childs.size() || childs[begin]->name != filename)
        return -1;

    return begin;
}
