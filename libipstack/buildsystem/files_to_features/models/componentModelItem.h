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

#include "componentmodelbaseitem.h"
#include <QDir>
#include <QVariant>
#include <QJsonObject>

class ComponentModel;
class ComponentModelItem : public ComponentModelBaseItem {
    friend class ComponentModelFileItem;
public:
    static ComponentModelItem* createComponent(const QDir& absolut_directory, ComponentModelItem* parent);

    enum add_files_enum {AllowMultipleSubdirLevels,AllowOneSubdirCreateSubcomponents};
    enum {TYPE=1};
    QString depends;
    QString vname;
    void update_component_name();
    void toJSon(QJsonObject &jsonObject, ComponentModel *componentModel);

    // For model output
    QString cache_component_name; // column 0
    QString cache_relative_directory; // column 1

    void addFiles(const QStringList& files, add_files_enum subdirFlag);
    QStringList get_all_files(bool only_existing);

    /**
     * @brief set_absolute_directory
     * @param model
     * @param dir
     * @return List of no longer used files
     */
    QStringList set_directory(const QString& absolut_dir);
    QString get_directory();

    QString get_dependencies() {
        return depends;
    }

    void set_dependencies(const QString& d) {
        depends = d;
    }

private:
    QDir directory;
    ComponentModelItem() {}
};
