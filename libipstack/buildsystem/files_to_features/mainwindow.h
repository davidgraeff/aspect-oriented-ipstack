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

#include <QMainWindow>
#include <QSortFilterProxyModel>

namespace Ui {
class MainWindow;
}
class Options;
class FileModel;
class ComponentModel;
class ComponentModelItem;
class ComponentModelFileItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(Options* o, QWidget *parent = 0);
    ~MainWindow();
    void update_problems();
    
private slots:
    void on_actionQuit_triggered();
    void on_actionSave_triggered();
    void on_actionDetectRemoved_triggered();

    void on_actionSynchronize_with_file_system_triggered();

    void on_actionAdd_component_triggered();

    void on_actionRemove_selected_components_triggered();

    void on_treeComponents_clicked(const QModelIndex &index);

    void on_btnChangeSubdir_clicked();

    void on_btnChangeDepends_clicked();

    void on_actionExpand_all_missing_files_only_triggered();

    void on_actionClear_and_propose_component_structure_triggered();

private:
    Ui::MainWindow *ui;
    Options* options;
    FileModel* filemodel;
    QSortFilterProxyModel* filemodelProxy;
    ComponentModel* componentModel;
    ComponentModelItem* currentComponent;
    ComponentModelFileItem* currentComponentFile;
    QSortFilterProxyModel* componentModelProxy;
};
