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

#pragma once

#include <QPushButton>
#include <QComboBox>
#include <QSet>
#include <QMap>
#include <QDialog>

namespace Ui {
class PickDependencies;
}

class depend : public QObject {
    Q_OBJECT
    public:
    // Widget pointer with convienient access
    union {
        QComboBox* operatorw;
        QPushButton* dependency;
        QWidget* widget;
    } widget;
    // Type of this element. May be an operator (and, or) or a dependency string
    enum depend_type {AND,OR,DEPENDENCY} type;

    ///// For dependency string only /////
    bool negated_dependecy;
    void setNegated(bool b);
    QString dependname;

    static depend* createDependency(const QString& name);
    static depend* createOperator(depend_type type);

    ~depend();
    private:
    depend() {}
};

class FilterProxyModel;
class DependencyModel;
class PickDependencies : public QDialog
{
    Q_OBJECT
    
public:
    explicit PickDependencies(DependencyModel* dependsModel, QWidget *parent = 0);
    ~PickDependencies();

    /// Construct a string out of the dependency pattern object
    QString get_dependency_string();
    /// Set initial dependencies and construct pattern object
    void set_initial_selection(const QString& depends_pattern);

    QSet<QString> used_depends;
    QList<depend*> pattern;
    
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::PickDependencies *ui;
    FilterProxyModel* dependsModelProxy;
    DependencyModel* dependsModel;
};
