#include "filemodelitem.h"
#include <QDebug>

int FileModelItem::getRow() {
    if (parent) {
        return parent->childs.indexOf(this);
    } else
        return 0;
}

QString FileModelItem::full_absolute_path() {
    if (parent)
        return parent->full_absolute_path() + "/" + name;
    else
        return name;
}

FileModelItem::~FileModelItem() {
    qDeleteAll(childs);
}

FileModelItem *FileModelItem::createFile(const QString &name, FileModelItem *parent) {
    FileModelItem* item = new FileModelItem();
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

FileModelItem *FileModelItem::createDir(const QString &name, FileModelItem *parent) {
    FileModelItem* item = new FileModelItem();
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

FileModelItem *FileModelItem::removeChild(const QString &filename) {
    int i = binary_search(filename);
    if (i==-1) {
        return 0;
    } else {
        return childs.takeAt(i);
    }
}

FileModelItem *FileModelItem::getItemByName(const QString &filename) {
    return childs.value(binary_search(filename), 0);
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
