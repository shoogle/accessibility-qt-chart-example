//============================================================================
// Copyright (c) 2020, Peter Jonas
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//============================================================================

#ifndef PIEMODEL_H
#define PIEMODEL_H

#include <QStandardItemModel>

class PieView;

// Custom data roles to use alongside Qt::ItemDataRole.
// See https://doc.qt.io/qt-5/qt.html#ItemDataRole-enum
enum ItemDataRole {
    AccessibleInterfaceRole = Qt::UserRole,
    // AnotherRole,
    // YetAnotherRole,
};

// Custom item types to use alongside QStandardItem::ItemType.
// See https://doc.qt.io/qt-5/qstandarditem.html#ItemType-enum
enum ItemType {
    PieItemType = QStandardItem::UserType,
    // AnotherType,
    // YetAnotherType,
};

class PieItem : public QStandardItem
{
public:
    PieItem();
    ~PieItem() override;
    QStandardItem* clone() const override;
    void deleteAccessibleInterface();
    int type() const override;

protected:
    PieItem(const PieItem& other);
};

class PieModel : public QStandardItemModel
{
    Q_OBJECT

public:
    PieModel(int rows, int columns, QObject *parent = nullptr);
    bool removeRows(int startRow, int count, const QModelIndex &parent = QModelIndex());
};

#endif // PIEMODEL_H
