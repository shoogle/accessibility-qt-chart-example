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

#include "accessiblepieview.h"

#include "piemodel.h"
#include "pieview.h"

#define ROWS m_pieview->model()->rowCount()
#define COLS m_pieview->model()->columnCount()

QAccessibleInterface* accessiblePieViewFactory(const QString &classname, QObject *object)
{
    if (object && object->isWidgetType() && classname == QLatin1String("PieView"))
        return new AccessiblePieView(static_cast<PieView*>(object));

    return nullptr;
}

AccessiblePieItem::AccessiblePieItem(PieView* pv, QModelIndex index)
: QAccessibleInterface()
{
    m_pieview = pv;
    m_index = index;
}

QAccessibleInterface* AccessiblePieItem::child(int index) const
{
    Q_UNUSED(index)
    return nullptr;
}

QAccessibleInterface* AccessiblePieItem::childAt(int x, int y) const
{
    Q_UNUSED(x)
    Q_UNUSED(y)
    return nullptr;
}

int AccessiblePieItem::childCount() const
{
    return 0;
}

int AccessiblePieItem::index() const
{
    return m_index.row() * COLS + m_index.column();
}

QAccessibleInterface* AccessiblePieItem::focusChild() const
{
    if (m_index.isValid() && m_pieview->currentIndex() == m_index)
        return const_cast<AccessiblePieItem*>(this);
    return nullptr;
}

int AccessiblePieItem::indexOfChild(const QAccessibleInterface*) const
{
    return -1;
}

bool AccessiblePieItem::isValid() const
{
    return m_pieview != nullptr
        && m_index.isValid()
        && m_index.data(AccessibleInterfaceRole).value<AccessiblePieItem*>() == this;
}

QString AccessiblePieItem::name() const
{
    QModelIndex categoryIndex = m_index.sibling(m_index.row(), 0);
    QModelIndex sliceIndex    = m_index.sibling(m_index.row(), 1);

    QString categoryName = categoryIndex.data().toString(); // e.g. "Scientific Research"
    double sliceValue    = sliceIndex.data().toDouble();    // e.g. "21"

    // convert value to percentage since this is a pie chart
    double percentage = sliceValue / m_pieview->total() * 100.0;
    QString slicePercentage = QLocale::system().toString(percentage, 'f', 1);

    // Ensure the returned name includes both the categoryName and the
    // slicePercentage. This is done so that blind users are not just
    // given a name or a number with no context.
    QString name = m_index.column() == 0
        ? QObject::tr("%1, %2%").arg(categoryName, slicePercentage)
        : QObject::tr("%1% %2").arg(slicePercentage, categoryName);

    // Ideally the screen reader should announce whether the item is selected
    // based on the value we return in `state()`, but not all screen readers
    // do this. Note: it is possible to select multiple items at once, and it
    // is also possible for the current item to be in an unselected state.
    // Hold Ctrl while navigating with the arrow keys to retain selection,
    // press Space to add or remove the current item from the selection. This
    // is the standard behaviour provided by QAbstractItemView.
#if defined(Q_OS_WIN)
    // Windows: Narrator and NVDA announce selection state. JAWS doesn't, but
    // there's no easy way to write code specifically for JAWS here.
#else
    // Orca (Linux) and VoiceOver (macOS) currently do not announce selection
    // state. Ideally this would be fixed in Qt or the respective screen
    // reader, but for now... HACK: Add selection state to the item name.
    if (!m_pieview->selectionModel()->isSelected(m_index))
        name = QObject::tr("%1 not selected").arg(name);
#endif
    return name;
}

QObject* AccessiblePieItem::object() const
{
    return nullptr;
}

QAccessibleInterface* AccessiblePieItem::parent() const
{
    return QAccessible::queryAccessibleInterface(m_pieview);
}

QRect AccessiblePieItem::rect() const
{
    return m_pieview->visualRect(m_index).translated(m_pieview->mapToGlobal(QPoint(0,0)));
}

// We probably don't need to implement this function. It is only done here for completeness.
QVector<QPair<QAccessibleInterface*, QAccessible::Relation>>
AccessiblePieItem::relations(QAccessible::Relation match) const
{
    QVector<QPair<QAccessibleInterface*, QAccessible::Relation>> rels;
    if (m_index.column() == 0) {
        // We are a category so return the slice of the pie that is labelled by us.
        if (match & QAccessible::Labelled)
            rels.append(qMakePair(parent()->child(index() + 1), QAccessible::Labelled));
    } else {
        // We are a slice of the pie so return the category that is our label.
        if (match & QAccessible::Label)
            rels.append(qMakePair(parent()->child(index() - 1), QAccessible::Label));
    }
    return rels;
}

QAccessible::Role AccessiblePieItem::role() const
{
    // See https://doc.qt.io/qt-5/qaccessible.html#Role-enum
    // We want the screen reader to say the name of the current item and
    // nothing else (i.e. not the name followed by "button" or "text").
#if defined(Q_OS_MACOS)
    // Good on macOS with VoiceOver.
    return QAccessible::StaticText;
    // VoiceOver gives unwanted additional output if ListItem is used, and it
    // doesn't work at all if the role is TreeItem or Cell.
#else
    // Good on Windows with Narrator, NVDA, or JAWS; and on Linux with Orca.
    return QAccessible::ListItem;
    // Orca is equally happy with the roles TreeItem or Cell, but these cause
    // unwanted additional ouput on Windows. StaticText causes unwanted
    // additional output on both Linux and Windows.
#endif
}

QAccessible::State AccessiblePieItem::state() const
{
    QAccessible::State itemState;
    // See https://doc.qt.io/qt-5/qaccessible-state.html for complete list of
    // possible states and their meanings. Only a few of these will actually
    // affect screen reader output, so we might not need to set all of them
    // depending on the value we return for `role()`. Note that some states
    // are not usable because they are commented-out in Qt source code. See
    // https://github.com/qt/qtbase/blob/5.15/src/gui/accessible/qaccessible.h

    itemState.active = true;
//  itemState.adjustable; // commented-out in Qt source
    itemState.animated = false;
    itemState.busy = false;
    itemState.checkable = false;
    itemState.checked = false;
    itemState.checkStateMixed = false;
    itemState.collapsed = false;
    itemState.defaultButton = false;
//  itemState.defunct; // does not exist, Qt source says use invalid state
    itemState.editable = false;
    itemState.expandable = false;
    itemState.expanded = false;
    itemState.extSelectable = m_pieview->selectionMode() == QAbstractItemView::ExtendedSelection;
    itemState.focusable = true;
    itemState.focused = m_index == m_pieview->selectionModel()->currentIndex();
    itemState.hasPopup = false;
    itemState.hotTracked = false;
    itemState.invalid = false;
//  itemState.invalidEntry; // commented-out in Qt source
    itemState.invisible = false;
    itemState.linked = false;
    itemState.marqueed = false;
    itemState.modal = false;
    itemState.movable = false;
    itemState.multiLine = false;
    itemState.multiSelectable = m_pieview->selectionMode() == QAbstractItemView::MultiSelection;
    itemState.offscreen = false;
    itemState.passwordEdit = false;
//  itemState.playsSound; // commented-out in Qt source
    itemState.pressed = true;
    itemState.readOnly = false;
    itemState.searchEdit = false;
    itemState.selectable = bool(m_index.flags() & Qt::ItemIsSelectable);
    itemState.selectableText = false;
    itemState.selected = m_pieview->selectionModel()->isSelected(m_index);
    itemState.selfVoicing = false;
    itemState.sizeable = false;
//  itemState.summaryElement; // commented-out in Qt source
    itemState.supportsAutoCompletion = false;
    itemState.traversed = false;
//  itemState.updatesFrequently; // commented-out in Qt source
    itemState.disabled = false;

    return itemState;
}

QString AccessiblePieItem::text(QAccessible::Text t) const
{
    // See https://doc.qt.io/qt-5/qaccessible.html#Text-enum
    switch (t) {
    case QAccessible::Name:
        // Assuming it had been set in the model, we could simply do this:
        //     return m_index.data(Qt::AccessibleTextRole).toString();
        // But instead we do something a bit more interesting.
        return name(); // Gives a name and a percentage of the pie.
    case QAccessible::Description:
        return m_index.data(Qt::AccessibleDescriptionRole).toString();
    case QAccessible::Value:
    case QAccessible::Help:
    default:
        return QString();
    }
}

void AccessiblePieItem::setText(QAccessible::Text t, const QString &text)
{
    switch (t) {
    case QAccessible::Description:
        m_pieview->model()->setData(m_index, text, Qt::AccessibleDescriptionRole);
        break;
    case QAccessible::Name:
    case QAccessible::Value:
    case QAccessible::Help:
    case QAccessible::Accelerator:
    default:
        Q_ASSERT(false);
    }
}

AccessiblePieView::AccessiblePieView(PieView* pv)
: QAccessibleWidget(pv, QAccessible::List, pv->accessibleName())
// We set role to QAccessible::List but other values are possible, see
// https://doc.qt.io/qt-5/qaccessible.html#Role-enum. The aim for the role is
// to give the user a hint about the kind of widget they are interacting with
// (i.e. that it consists of multiple items). We tested likely roles with
// various screen readers and found that the List role gives the best output.
//   Linux: Table or Tree roles work just as well as the List role with Orca.
//   macOS: Table and Tree are completely broken with VoiceOver.
//   Windows: Table and Tree work for the view but the output for items is not
//     ideal. See comments in `AccessiblePieItem::role()`.
{
    Q_ASSERT(pv);
    m_pieview = pv;
}

QAccessibleInterface* AccessiblePieView::child(QModelIndex index) const
{
    Q_ASSERT(index.isValid() && index.model() == m_pieview->model());
    auto iface = index.data(AccessibleInterfaceRole).value<AccessiblePieItem*>();
    if (!iface) {
        iface = new AccessiblePieItem(m_pieview, index);
        m_pieview->model()->setData(index, QVariant::fromValue(iface), AccessibleInterfaceRole);
        QAccessible::registerAccessibleInterface(iface);
    }
    return iface;
}

// QAccessibleWidget::child(int) only deals with widget children, so we must
// override it to return items in the view instead (items are not widgets).
// Same for other child-based functions like childAt(), focusChild(), etc.
QAccessibleInterface* AccessiblePieView::child(int index) const
{
    Q_ASSERT(0 <= index && index < childCount());
    return child(m_pieview->model()->index(index / COLS, index % COLS));
}

QAccessibleInterface* AccessiblePieView::childAt(int x, int y) const
{
    QModelIndex index = m_pieview->indexAt(m_pieview->mapFromGlobal(QPoint(x, y)));
    if (index.isValid())
        return child(index);
    return nullptr; // no child at (x,y)
}

int AccessiblePieView::childCount() const
{
    return ROWS * COLS;
}

QAccessibleInterface* AccessiblePieView::focusChild() const
{
    QModelIndex current = m_pieview->currentIndex();
    if (current.isValid())
        return child(current);
    return nullptr;
}

int AccessiblePieView::indexOfChild(const QAccessibleInterface* iface) const
{
    Q_ASSERT(iface && iface->isValid() && iface->parent() == this);
    Q_ASSERT(dynamic_cast<const AccessiblePieItem*>(iface) != nullptr);
    return static_cast<const AccessiblePieItem*>(iface)->index();
}

bool AccessiblePieView::isValid() const
{
    if (!m_pieview)
        return false;
    return QAccessibleWidget::isValid();
}

QObject* AccessiblePieView::object() const
{
    return m_pieview;
}

QAccessible::State AccessiblePieView::state() const
{
    // See https://doc.qt.io/qt-5/qaccessible-state.html for complete list of possible states and
    // their meanings. Note that some states are not usable because they are commented-out in Qt
    // source code. See https://github.com/qt/qtbase/blob/5.15/src/gui/accessible/qaccessible.h

    QAccessible::State viewState = QAccessibleWidget::state();

    // QAccessibleWidget::state() has already set some states for us based on widget properties,
    // see https://github.com/qt/qtbase/blob/5.15/src/widgets/accessible/qaccessiblewidget.cpp.
    // Of the remaining states, only a few actually affect screen reader output. The ones that
    // matter depend on the platform, screen reader, and the value we return for `role()`. Here
    // we set all of them, but you can probably get away with just setting the likely ones.

//  viewState.active; // set by QAccessibleWidget::state()
//  viewState.adjustable; // commented-out in Qt source
    viewState.animated = false;
    viewState.busy = false;
    viewState.checkable = false;
    viewState.checked = false;
    viewState.checkStateMixed = false;
    viewState.collapsed = false;
    viewState.defaultButton = false;
//  viewState.defunct; // does not exist, Qt source says use invalid state
    viewState.editable = false;
    viewState.expandable = false;
    viewState.expanded = false;
    viewState.extSelectable = m_pieview->selectionMode() == QAbstractItemView::ExtendedSelection;
//  viewState.focusable = true; // set by QAccessibleWidget::state()
//  viewState.focused = true; // set by QAccessibleWidget::state()
    viewState.hasPopup = false;
    viewState.hotTracked = false;
    viewState.invalid = false;
//  viewState.invalidEntry; // commented-out in Qt source
//  viewState.invisible; // set by QAccessibleWidget::state()
    viewState.linked = false;
    viewState.marqueed = false;
    viewState.modal = false;
//  viewState.movable = false; // set by QAccessibleWidget::state()
    viewState.multiLine = false;
    viewState.multiSelectable = m_pieview->selectionMode() == QAbstractItemView::MultiSelection;
    viewState.offscreen = false;
    viewState.passwordEdit = false;
//  viewState.playsSound; // commented-out in Qt source
    viewState.pressed = true;
    viewState.readOnly = false;
    viewState.searchEdit = false;
    viewState.selectable = true;
    viewState.selectableText = false;
    viewState.selected = true;
    viewState.selfVoicing = false;
//  viewState.sizeable = false; // set by QAccessibleWidget::state()
//  viewState.summaryElement; // commented-out in Qt source
    viewState.supportsAutoCompletion = false;
    viewState.traversed = false;
//  viewState.updatesFrequently; // commented-out in Qt source
//  viewState.disabled = false; // set by QAccessibleWidget::state()

    return viewState;
}

QString AccessiblePieView::text(QAccessible::Text t) const
{
    // See https://doc.qt.io/qt-5/qaccessible.html#Text-enum
    // macOS: We must return at least a Name or Description for the view
    // otherwise VoiceOver won't navigate past the first couple of items.
    switch (t) {
    case QAccessible::Name:
        return m_pieview->accessibleName();
    case QAccessible::Description:
        return m_pieview->accessibleDescription();
    case QAccessible::Help:
        return QObject::tr("Navigate with the mouse or arrow keys");
    case QAccessible::Value:
    case QAccessible::Accelerator:
    default:
        return QAccessibleWidget::text(t);
    }
}

void AccessiblePieView::setText(QAccessible::Text t, const QString &text)
{
    switch (t) {
    case QAccessible::Name:
        m_pieview->setAccessibleName(text);
        break;
    case QAccessible::Description:
        m_pieview->setAccessibleDescription(text);
        break;
    case QAccessible::Value:
    case QAccessible::Help:
    case QAccessible::Accelerator:
    default:
        Q_ASSERT(false);
    }
}
