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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pickdependencies.h"
#include "options.h"
#include "fileModel.h"
#include "componentModel.h"
#include "componentModelItem.h"
#include "componentmodelfileitem.h"
#include "qlistwidgetitemwithcompmodelindex.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QPainter>

MainWindow::MainWindow(Options* o, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), options(o)
{
    currentComponent=0;
    currentComponentFile=0;
    ui->setupUi(this);
    ui->btnShowProblems->setChecked(false);

    // models
    filemodel = new FileModel(QString::fromStdString(options->base_directory), this);
    filemodelProxy = new QSortFilterProxyModel(this);
    filemodelProxy->setSourceModel(filemodel);
    filemodelProxy->setDynamicSortFilter(true);
    filemodelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(ui->lineSearchUnusedFiles,&QLineEdit::textChanged,
            filemodelProxy, &QSortFilterProxyModel::setFilterFixedString);
    ui->treeFiles->setModel(filemodelProxy);

    componentModel = new ComponentModel(QString::fromStdString(options->base_directory),
                                        QString::fromStdString(options->featureToFilesRelationfile), this);
    componentModelProxy = new QSortFilterProxyModel(this);
    componentModelProxy->setSourceModel(componentModel);
    componentModelProxy->setDynamicSortFilter(true);
    componentModelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(ui->lineSearchComponents,&QLineEdit::textChanged,
            componentModelProxy, &QSortFilterProxyModel::setFilterFixedString);

    ui->treeComponents->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeComponents->setModel(componentModelProxy);
    ui->treeComponents->expand(componentModelProxy->index(0,0));

    on_actionSynchronize_with_file_system_triggered();
    update_problems();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_problems()
{
    int number_of_problems = 0;
    ui->listProblems->clear();

    if (filemodel->get_unused_files_size()) {
        ++number_of_problems;
        ui->listProblems->addItem(tr("%1 files in your project directory are not used by any component!").
                                  arg(filemodel->get_unused_files_size()));
    }

    QModelIndexList list_of_missing_files = componentModel->get_missing_files();
    foreach (const QModelIndex& index, list_of_missing_files) {
        QListWidgetItemWithCompModelIndex* problem_item = new QListWidgetItemWithCompModelIndex();
        problem_item->setText(tr("File missing: %1").
                              arg(static_cast<ComponentModelFileItem*>(index.internalPointer())->get_full_path()));
        problem_item->modelindex = index;
        ui->listProblems->addItem(problem_item);
        ++number_of_problems;
    }


    // update button icon
    if (number_of_problems){
        QImage p(20,20,QImage::Format_ARGB32);
        p.fill(Qt::transparent);
        QPainter pa(&p);
        QRectF r(0.0,0.0,20.0,20.0);
        pa.save();
        pa.setBrush(QColor(Qt::darkRed));
        pa.drawChord(r,0,5760);
        pa.restore();
        pa.setPen(QColor(Qt::white));
        QFont f(this->font());
        QTextOption to;
        to.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        f.setPixelSize(p.size().height()-4);
        pa.setFont(f);
        pa.drawText(r,QString::number(number_of_problems),to);
        ui->btnShowProblems->setIcon(QPixmap::fromImage(p));
    } else {
        ui->btnShowProblems->setIcon(QIcon::fromTheme("document-properties"));
    }
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionSave_triggered()
{
    componentModel->save();
}

void MainWindow::on_actionDetectRemoved_triggered()
{
    componentModel->remove_non_existing_files();
}

void MainWindow::on_actionSynchronize_with_file_system_triggered()
{
    filemodel->createFileTree();
    filemodel->removeFiles(componentModel->get_used_files());
    ui->treeFiles->expandAll();
}

void MainWindow::on_actionAdd_component_triggered()
{
    if (!currentComponent)
        return;

    componentModel->addComponent(currentComponent);
}

void MainWindow::on_actionRemove_selected_components_triggered()
{
    if (!currentComponent)
        return;

    QStringList unused_files = componentModel->removeComponent(currentComponent);
    filemodel->addFiles(unused_files);
    ui->treeFiles->expandAll();
}

void MainWindow::on_treeComponents_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        ui->btnChangeSubdir->setEnabled(false);
        ui->btnChangeDepends->setEnabled(false);
        return;
    }

    ui->btnChangeSubdir->setEnabled(true);
    ui->btnChangeDepends->setEnabled(true);

    ComponentModelBaseItem* b = static_cast<ComponentModelBaseItem*>(componentModelProxy->mapToSource(index).internalPointer());
    if (b->type == ComponentModelItem::TYPE) {
        currentComponent = (ComponentModelItem*)b;
        currentComponentFile = 0;

        ui->labelDepends->setText(currentComponent->depends);
        ui->labelPath->setText(currentComponent->get_directory());
    } else {
        currentComponentFile = (ComponentModelFileItem*)b;
        currentComponent = currentComponentFile->parent;

        ui->labelDepends->setText(currentComponent->depends);
        ui->labelPath->setText(currentComponentFile->get_full_path());
    }
}

void MainWindow::on_btnChangeSubdir_clicked()
{
    if (!currentComponent)
        return;

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                currentComponent->get_directory(),
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;

    /**
     * If we change the subdirectory, all files of this component (and child components)
     * have to be removed from the component model and have to be added to the unused-file model again.
     */
    QStringList unused_files = currentComponent->set_directory(dir);
    filemodel->addFiles(unused_files);
    ui->treeFiles->expandAll();
}

void MainWindow::on_btnChangeDepends_clicked()
{
    if (!currentComponent)
        return;

    PickDependencies* p = new PickDependencies(QString::fromStdString(options->kconfigfile));
    if (p->exec() == QDialog::Accepted) {
//            ui->txtDependencies->setPlainText(ui->txtDependencies->document()->toPlainText() +
//                                              QString("&") + p->list_of_selected_features.join(" and &") + QString(" "));
    }
    p->deleteLater();
}

void MainWindow::on_actionExpand_all_missing_files_only_triggered()
{
    ui->treeComponents->setUpdatesEnabled(false);
    QModelIndexList list_of_missing_files = componentModel->get_missing_files();
    foreach (const QModelIndex& index, list_of_missing_files) {
        QModelIndex parent = index.parent();
        while (parent.isValid()) {
            ui->treeComponents->expand(componentModelProxy->mapFromSource(parent));
            parent = parent.parent();
        }
    }
    ui->treeComponents->setUpdatesEnabled(true);
}

void MainWindow::on_actionClear_and_propose_component_structure_triggered()
{
    if (componentModel->rowCount()==0 ||
            QMessageBox::warning(this, tr("Erase all components?"),
                  tr("This will erase all components and propose a structure based on the kconfig fm file. Start from scratch?"),
                    QMessageBox::Yes|QMessageBox::Cancel,QMessageBox::Cancel) == QMessageBox::Yes) {

        ui->treeComponents->setUpdatesEnabled(false);

        ui->treeComponents->setUpdatesEnabled(true);
        on_actionSynchronize_with_file_system_triggered();
    }
}
