#ifndef QLISTWIDGETITEMWITHCOMPMODELINDEX_H
#define QLISTWIDGETITEMWITHCOMPMODELINDEX_H

#include <QListWidgetItem>

class QListWidgetItemWithCompModelIndex : public QListWidgetItem
{
public:
    explicit QListWidgetItemWithCompModelIndex();
    QModelIndex modelindex;
signals:

public slots:

};

#endif // QLISTWIDGETITEMWITHCOMPMODELINDEX_H
