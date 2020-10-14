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

#ifndef ACCESSIBLEPIEVIEW_H
#define ACCESSIBLEPIEVIEW_H

#include <QAccessibleInterface>
#include <QPersistentModelIndex>

class PieView;

QAccessibleInterface* accessiblePieViewFactory(const QString &classname, QObject *object);

class AccessiblePieItem : public QAccessibleInterface
{
    friend class AccessiblePieView; // allow access to index()

public:
    AccessiblePieItem(PieView* pv, QModelIndex index);
    QAccessibleInterface* child(int index) const override;
    QAccessibleInterface* childAt(int x, int y) const override;
    int childCount() const override;
    int indexOfChild(const QAccessibleInterface*) const override;
    bool isValid() const override;
    QObject* object() const override;
    QAccessibleInterface* parent() const override;
    QRect rect() const override;
    QAccessible::Role role() const override;
    QAccessible::State state() const override;
    QString text(QAccessible::Text t) const override;
    void setText(QAccessible::Text t, const QString &text) override;

private:
    int index() const;
    QString name() const;

    QPersistentModelIndex m_index;
    PieView* m_pieview;
};
Q_DECLARE_METATYPE(AccessiblePieItem*); // enable storing in QVariant

class AccessiblePieView : public QAccessibleInterface
{
public:
    AccessiblePieView(PieView* pv);
    QAccessibleInterface* child(int index) const override;
    QAccessibleInterface* childAt(int x, int y) const override;
    int childCount() const override;
    int indexOfChild(const QAccessibleInterface* iface) const override;
    bool isValid() const override;
    QObject* object() const override;
    QAccessibleInterface* parent() const override;
    QRect rect() const override;
    QAccessible::Role role() const override;
    QAccessible::State state() const override;
    QString text(QAccessible::Text t) const override;
    void setText(QAccessible::Text t, const QString &text) override;

private:
    QAccessibleInterface* child(QModelIndex index) const;
    PieView* m_pieview;
};

#endif // ACCESSIBLEPIEVIEW_H
