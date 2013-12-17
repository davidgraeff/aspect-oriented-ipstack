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

#include "familyBaseItem.h"
#include <QDir>
#include <QVariant>
#include <QJsonObject>

class FamilyModel;
class FamilyFile;
class FamilyComponent : public FamilyBaseItem {
public:
    ~FamilyComponent();
    friend class FamilyModel;
    enum {TYPE=1};
    static FamilyComponent* createComponent(FamilyModel *componentModel,
                                            const QDir& absolut_directory,
                                            FamilyComponent* parent,
                                            const QString& depends = QString(),
                                            const QString& vname = QString());

    void toJSon(QJsonObject &jsonObject);

    /// Add files to this component. If files are not accepted a
    /// familyModel rejected signal will be emitted. If you set
    /// subdirFlag to AllowMultipleSubdirLevels, file items are created with
    /// filenames that may include a part of a path, for instance: "ip/file.txt".
    /// In contrast if you use AllowOneSubdirCreateSubcomponents for every subdir
    /// part of the path a subcomponent is created. This is recommend!
    enum add_files_enum {AllowMultipleSubdirLevels,AllowOneSubdirCreateSubcomponents};
    void addFiles(const QStringList& files, add_files_enum subdirFlag);
    static FamilyComponent* findOrCreateRecursivly(FamilyComponent* current, QString relative_path);
    QStringList get_all_files(bool only_existing, bool remove_files = false);
    void get_dependencies_string(QStringList& l);

    /**
     * Set the directory of this component. Alyways use the absolute dir,
     * for saving, the dir will be converted to a relative one.
     * Will emit a signal of a list of no longer used files
     */
    void set_directory(const QDir &absolut_dir, bool removeSubFiles);
    /// Return absolut directory of this component
    QDir get_directory() const;

    inline QString get_dependencies() const { return depends; }
    void set_dependencies(const QString& d);

    inline QString get_vname() const { return vname; }
    void set_vname(const QString& n);

    inline QString get_component_name() const { return cache_component_name; }
    inline QString get_component_dir() const { return cache_relative_directory; }
private:
    // family model data
    QDir directory;
    QString depends;
    QString vname;

    // For model handling: We cache some values for fast access
    QString cache_component_name; // column 0
    QString cache_relative_directory; // column 1
    void update_component_name();


    // Hidden constructor
    FamilyComponent(FamilyModel *componentModel);
};
