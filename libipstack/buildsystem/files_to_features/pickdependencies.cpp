/*
 * Copyright 2013 David Graeff
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE REGENTS AND CONTRIBUTORS, INC. OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pickdependencies.h"
#include "ui_pickdependencies.h"
#include "kconfigmodel/kconfigModel.h"
#include "filterproxymodel.h"
#include <QDebug>
#include <QHBoxLayout>

PickDependencies::PickDependencies(const QString& kconfig_input_filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PickDependencies)
{
    ui->setupUi(this);
    ui->frame->setLayout(new QVBoxLayout());
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    dependsModel = new DependencyModel(kconfig_input_filename,this);
    dependsModelProxy = new FilterProxyModel(this);
    dependsModelProxy->setSourceModel(dependsModel);
    dependsModelProxy->setDynamicSortFilter(true);
    dependsModelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(ui->lineSearch,&QLineEdit::textChanged,
            dependsModelProxy, &FilterProxyModel::setFilterFixedString);
    ui->treeView->setModel(dependsModelProxy);
    ui->treeView->expandAll();
}

PickDependencies::~PickDependencies()
{
    delete ui;
}

QString PickDependencies::get_dependency_string()
{
    QString result;
    for (int i=0;i<pattern.size();++i) {
        if (pattern[i]->type==depend::DEPENDENCY) {
            if (pattern[i]->negated_dependecy)
                result += " not";
            result += " &" + pattern[i]->dependname;
        } else
            result += " " + pattern[i]->widget.operatorw->currentText();
    }
    return result.trimmed();
}

void PickDependencies::set_initial_selection(const QString &depends_pattern)
{
    if (depends_pattern.trimmed().isEmpty())
        return;

    QStringList list_of_selected_features = depends_pattern.split(' ');

    if (list_of_selected_features.isEmpty() ||
            !list_of_selected_features[0].startsWith("&") ||
            list_of_selected_features[0].trimmed().toLower()=="and")
        return;

    depend* n;
    QHBoxLayout* layout = 0;
    bool negated = false;
    for (int it=0;it<list_of_selected_features.size();++it) {
        if (!layout)
            layout = new QHBoxLayout();

        const QString& w = list_of_selected_features[it];
        if (w.startsWith("&")) {
            // create dependency button
            n = depend::createDependency(w.mid(1));
            n->setNegated(negated);
            pattern.append(n);
            layout->addWidget(n->widget.widget);
            negated = false;

            // select item in tree view
            QModelIndex compModelIndex = dependsModelProxy->mapFromSource(dependsModel->indexOf(w.mid(1)));
            ui->treeView->scrollTo(compModelIndex);
            ui->treeView->selectionModel()->select(compModelIndex,QItemSelectionModel::Select);

            // add hbox layout to main layout + stretch element
            layout->addStretch();
            static_cast<QVBoxLayout*>(ui->frame->layout())->addLayout(layout);
            layout = 0;
        } else if (w.toLower()=="and") {
            n = depend::createOperator(depend::AND);
            pattern.append(n);
            layout->addWidget(n->widget.widget);
        } else if (w.toLower()=="or") {
            n = depend::createOperator(depend::OR);
            pattern.append(n);
            layout->addWidget(n->widget.widget);
        } else if (w.toLower()=="not") {
            negated = true;
        }


    }
}

void PickDependencies::on_buttonBox_accepted()
{
    accept();
}

void PickDependencies::on_buttonBox_rejected()
{
    rejected();
}

void PickDependencies::on_treeView_clicked(const QModelIndex &)
{
    QSet<QString> list_of_selected_features;
    QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes();
    foreach (QModelIndex index, list)
        if (index.column()==0)
            list_of_selected_features << dependsModel->feature_name(dependsModelProxy->mapToSource(index));

    // get removed features
    QSet<QString> removed_features(used_depends);
    removed_features -= list_of_selected_features;
    // get added features
    list_of_selected_features -= used_depends;

    // New set of features
    used_depends.subtract(removed_features).unite(list_of_selected_features);

    // Remove widgets
    for (int i=pattern.size()-1;i>=0;--i) {
        if (removed_features.contains(pattern[i]->dependname)) {
            delete pattern.takeAt(i);
            if (i-1>0) {
                --i;
                delete pattern.takeAt(i);
            }
        }
    }
    // Remove last operators
    if (pattern.size() && (pattern.last()->type==depend::AND || pattern.last()->type==depend::OR)) {
        delete pattern.takeLast();
    }
    // Remove first operator, too
    if (pattern.size() && (pattern.first()->type==depend::AND || pattern.first()->type==depend::OR)) {
        delete pattern.takeFirst();
    }

    // Add widgets and operator
    depend* n;
    auto it = list_of_selected_features.begin();
    for (;it!=list_of_selected_features.end();++it) {
        QHBoxLayout* layout = new QHBoxLayout();
        if (pattern.size()) {
            n = depend::createOperator(depend::AND);
            pattern.append(n);
            layout->addWidget(n->widget.widget);
        }
        n = depend::createDependency(*it);
        pattern.append(n);
        layout->addWidget(n->widget.widget);
        static_cast<QVBoxLayout*>(ui->frame->layout())->addLayout(layout);

        static_cast<QVBoxLayout*>(ui->frame->layout())->addStretch();
    }

}


void depend::setNegated(bool b) {
    if (type!=DEPENDENCY)
        return;
    negated_dependecy = b;
    widget.dependency->setChecked(b);
}

depend *depend::createDependency(const QString &name) {
    depend* d = new depend();
    d->dependname = name;
    d->type = DEPENDENCY;
    d->negated_dependecy = false;
    d->widget.dependency = new QPushButton(name);
    d->widget.dependency->connect(d->widget.dependency,
                                  &QPushButton::toggled,
                                  d,
                                  &depend::setNegated);
    d->widget.dependency->setCheckable(true);
    return d;
}

depend *depend::createOperator(depend::depend_type type) {
    depend* d = new depend();
    d->type = type;
    d->negated_dependecy = false;
    d->widget.operatorw = new QComboBox();
    d->widget.operatorw->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->widget.operatorw->addItems(QStringList()<<"and"<<"or");
    d->widget.operatorw->setCurrentIndex((int)type);
    return d;
}

depend::~depend() {
    if (widget.dependency)
        delete widget.dependency;
}
