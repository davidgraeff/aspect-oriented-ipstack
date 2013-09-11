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

#include <QListWidgetItem>
#include <QPointer>

class ComponentModelBaseItem;
/**
 * We subclass the QListWidgetItem that is stored in the problems
 * list in the mainwindow, to add a pointer reference to the item.
 * This allows us to click on the list and if the component that
 * is referenced by this item is still valid we can for example
 * jump to that specific component in the component tree.
 */
class ProblemListItem : public QListWidgetItem
{
public:
    enum type_enum {UNUSED_FILES,REMOVED_FILE,MOVED_FILE,EMPTY_COMPONENT,COMPONENT_WITH_ONLY_FILES} type;
    explicit ProblemListItem(type_enum);
    QPointer<ComponentModelBaseItem> problem_component_item;
    QString maybe_moved;
signals:

public slots:

};
