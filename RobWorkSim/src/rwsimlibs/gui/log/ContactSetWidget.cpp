/********************************************************************************
 * Copyright 2015 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
 * Faculty of Engineering, University of Southern Denmark
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ********************************************************************************/

#include "ContactSetWidget.hpp"

#include "ui_ContactSetWidget.h"

#include <rw/graphics/GroupNode.hpp>
#include <rw/graphics/SceneGraph.hpp>
#include <rwsim/contacts/RenderContacts.hpp>
#include <rwsim/dynamics/DynamicWorkCell.hpp>
#include <rwsim/log/LogContactSet.hpp>

#include <QItemSelection>

using namespace rw::common;
using namespace rw::core;
using namespace rw::graphics;
using namespace rw::kinematics;
using namespace rw::math;
using namespace rwsim::contacts;
using namespace rwsim::dynamics;
using namespace rwsim::log;
using namespace rwsimlibs::gui;

ContactSetWidget::ContactSetWidget (rw::core::Ptr< const LogContactSet > entry, QWidget* parent) :
    SimulatorLogEntryWidget (parent), _ui (new Ui::ContactSetWidget ()), _contactSet (entry)
{
    _ui->setupUi (this);
    connect (_ui->_contactBodyPairs->selectionModel (),
             SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
             this,
             SLOT (contactSetPairsChanged (const QItemSelection&, const QItemSelection&)));
    connect (_ui->_contactTable->selectionModel (),
             SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
             this,
             SLOT (contactSetChanged (const QItemSelection&, const QItemSelection&)));

    QStringList headerLabels;
    headerLabels.push_back ("First");
    headerLabels.push_back ("Second");
    headerLabels.push_back ("Contacts");
    _ui->_contactBodyPairs->setHorizontalHeaderLabels (headerLabels);

    _ui->_contactTable->setColumnCount (3);
    headerLabels.clear ();
    headerLabels.push_back ("First");
    headerLabels.push_back ("Second");
    headerLabels.push_back ("Depth");
    _ui->_contactTable->setHorizontalHeaderLabels (headerLabels);

    connect (
        _ui->_scalingRadius, SIGNAL (valueChanged (double)), this, SLOT (scalingChanged (double)));
    connect (
        _ui->_scalingNormal, SIGNAL (valueChanged (double)), this, SLOT (scalingChanged (double)));
    connect (_ui->_showPointA, SIGNAL (stateChanged (int)), this, SLOT (showChanged (int)));
    connect (_ui->_showPointB, SIGNAL (stateChanged (int)), this, SLOT (showChanged (int)));
    connect (_ui->_showNormalA, SIGNAL (stateChanged (int)), this, SLOT (showChanged (int)));
    connect (_ui->_showNormalB, SIGNAL (stateChanged (int)), this, SLOT (showChanged (int)));
}

ContactSetWidget::~ContactSetWidget ()
{
    if (_root != NULL) {
        _root->removeChild ("Contacts");
    }
}

void ContactSetWidget::setDWC (rw::core::Ptr< const DynamicWorkCell > dwc)
{}

void ContactSetWidget::setEntry (rw::core::Ptr< const SimulatorLog > entry)
{
    const rw::core::Ptr< const LogContactSet > set = entry.cast< const LogContactSet > ();
    if (!(set == NULL))
        _contactSet = set;
    else
        RW_THROW ("_contactSet (setEntry): invalid entry!");
}

rw::core::Ptr< const SimulatorLog > ContactSetWidget::getEntry () const
{
    return _contactSet;
}

void ContactSetWidget::updateEntryWidget ()
{
    _ui->_contactDescription->setText (QString::fromStdString (_contactSet->getDescription ()));
    _ui->_contacts->setText (QString::number (_contactSet->size ()));
    typedef std::pair< std::string, std::string > FramePair;
    std::set< FramePair > pairs;
    for (const Contact& c : _contactSet->getContacts ()) {
        const std::string& nameA = c.getNameA ();
        const std::string& nameB = c.getNameB ();
        if (nameA < nameB)
            pairs.insert (std::make_pair (nameA, nameB));
        else
            pairs.insert (std::make_pair (nameB, nameA));
    }
    const int nrOfPairs = static_cast< int > (pairs.size ());
    if (pairs.size () > static_cast< std::size_t > (nrOfPairs))
        RW_THROW ("There are too many entries for the widget to handle!");
    _ui->_contactBodyPairs->setRowCount (nrOfPairs);
    int row = 0;
    _ui->_contactBodyPairs->setSortingEnabled (false);
    for (const FramePair& pair : pairs) {
        // Count how many contacts there are for this pair
        int contacts = 0;
        for (const Contact& c : _contactSet->getContacts ()) {
            if (c.getNameA () == pair.first && c.getNameB () == pair.second)
                contacts++;
            else if (c.getNameA () == pair.second && c.getNameB () == pair.first)
                contacts++;
        }
        // Note: setItem takes ownership of the QTableWidgetItems
        QTableWidgetItem* const itemA = new QTableWidgetItem (QString::fromStdString (pair.first));
        QTableWidgetItem* const itemB = new QTableWidgetItem (QString::fromStdString (pair.second));
        QTableWidgetItem* const itemC = new QTableWidgetItem (QString::number (contacts));
        _ui->_contactBodyPairs->setItem (row, 0, itemA);
        _ui->_contactBodyPairs->setItem (row, 1, itemB);
        _ui->_contactBodyPairs->setItem (row, 2, itemC);
        row++;
    }
    _ui->_contactBodyPairs->setSortingEnabled (true);
    if (nrOfPairs > 0)
        _ui->_contactBodyPairs->setRangeSelected (
            QTableWidgetSelectionRange (0, 0, nrOfPairs - 1, 2), true);
}

void ContactSetWidget::showGraphics (GroupNode::Ptr root, SceneGraph::Ptr graph)
{
    if (root == NULL && _root != NULL)
        _root->removeChild ("Bodies");
    _root  = root;
    _graph = graph;
}

std::string ContactSetWidget::getName () const
{
    return "Contact Set";
}

void ContactSetWidget::setProperties (const PropertyMap::Ptr properties)
{
    SimulatorLogEntryWidget::setProperties (properties);
    if (!_properties.isNull ()) {
        if (!_properties->has ("ContactSetWidget_PointRadius"))
            _properties->set< double > ("ContactSetWidget_PointRadius",
                                        _ui->_scalingRadius->value () / 1000.);
        if (!_properties->has ("ContactSetWidget_NormalLength"))
            _properties->set< double > ("ContactSetWidget_NormalLength",
                                        _ui->_scalingNormal->value () / 1000.);
        if (!_properties->has ("ContactSetWidget_ShowPointA"))
            _properties->set< bool > ("ContactSetWidget_ShowPointA",
                                      _ui->_showPointA->isChecked ());
        if (!_properties->has ("ContactSetWidget_ShowPointB"))
            _properties->set< bool > ("ContactSetWidget_ShowPointB",
                                      _ui->_showPointB->isChecked ());
        if (!_properties->has ("ContactSetWidget_ShowNormalA"))
            _properties->set< bool > ("ContactSetWidget_ShowNormalA",
                                      _ui->_showNormalA->isChecked ());
        if (!_properties->has ("ContactSetWidget_ShowNormalB"))
            _properties->set< bool > ("ContactSetWidget_ShowNormalB",
                                      _ui->_showNormalB->isChecked ());

        _ui->_scalingRadius->setValue (_properties->get< double > ("ContactSetWidget_PointRadius") *
                                       1000.);
        _ui->_scalingNormal->setValue (
            _properties->get< double > ("ContactSetWidget_NormalLength") * 1000.);
        _ui->_showPointA->setChecked (_properties->get< bool > ("ContactSetWidget_ShowPointA"));
        _ui->_showPointB->setChecked (_properties->get< bool > ("ContactSetWidget_ShowPointB"));
        _ui->_showNormalA->setChecked (_properties->get< bool > ("ContactSetWidget_ShowNormalA"));
        _ui->_showNormalB->setChecked (_properties->get< bool > ("ContactSetWidget_ShowNormalB"));
    }
}

void ContactSetWidget::contactSetPairsChanged (const QItemSelection&, const QItemSelection&)
{
    typedef std::pair< std::string, std::string > NamePair;
    const QModelIndexList indexes = _ui->_contactBodyPairs->selectionModel ()->selectedIndexes ();
    std::list< NamePair > names;
    foreach (QModelIndex index, indexes)
    {
        if (index.column () != 0)
            continue;
        const std::string nameA = _ui->_contactBodyPairs->item (index.row (), 0)
                                      ->data (Qt::DisplayRole)
                                      .toString ()
                                      .toStdString ();
        const std::string nameB = _ui->_contactBodyPairs->item (index.row (), 1)
                                      ->data (Qt::DisplayRole)
                                      .toString ()
                                      .toStdString ();
        names.push_back (NamePair (nameA, nameB));
    }
    std::vector< std::size_t > contactsToShow;
    for (std::size_t i = 0; i < _contactSet->size (); i++) {
        const Contact& c = _contactSet->getContact (i);
        bool show        = false;
        for (const NamePair& name : names) {
            const std::string& nameA = c.getNameA ();
            const std::string& nameB = c.getNameB ();
            if (nameA == name.first && nameB == name.second)
                show = true;
            else if (nameB == name.first && nameA == name.second)
                show = true;
            if (show)
                break;
        }
        if (show)
            contactsToShow.push_back (i);
    }
    const int nrOfContactsToShow = static_cast< int > (contactsToShow.size ());
    if (contactsToShow.size () > static_cast< std::size_t > (nrOfContactsToShow))
        RW_THROW ("There are too many entries for the widget to handle!");
    _ui->_contactTable->clearSelection ();
    _ui->_contactTable->setRowCount (nrOfContactsToShow);
    int row = 0;
    _ui->_contactTable->setSortingEnabled (false);
    for (const std::size_t i : contactsToShow) {
        const Contact& c         = _contactSet->getContact (i);
        const std::string& nameA = c.getNameA ();
        const std::string& nameB = c.getNameB ();
        const QString hover      = toQString (c);
        // Note: setItem takes ownership of the QTableWidgetItems
        QTableWidgetItem* itemA;
        QTableWidgetItem* itemB;
        QTableWidgetItem* itemC = new QTableWidgetItem (QString::number (c.getDepth ()));
        if (nameA < nameB) {
            itemA = new QTableWidgetItem (QString::fromStdString (nameA));
            itemB = new QTableWidgetItem (QString::fromStdString (nameB));
        }
        else {
            itemA = new QTableWidgetItem (QString::fromStdString (nameB));
            itemB = new QTableWidgetItem (QString::fromStdString (nameA));
        }
        itemA->setData (Qt::ToolTipRole, hover);
        itemB->setData (Qt::ToolTipRole, hover);
        itemC->setData (Qt::ToolTipRole, hover);
        if (c.getDepth () > 0) {
            itemA->setData (Qt::ForegroundRole, QColor (Qt::red));
            itemB->setData (Qt::ForegroundRole, QColor (Qt::red));
            itemC->setData (Qt::ForegroundRole, QColor (Qt::red));
        }
        else {
            itemA->setData (Qt::ForegroundRole, QColor (Qt::green));
            itemB->setData (Qt::ForegroundRole, QColor (Qt::green));
            itemC->setData (Qt::ForegroundRole, QColor (Qt::green));
        }
        itemA->setData (Qt::UserRole, QVariant::fromValue (i));
        _ui->_contactTable->setItem (row, 0, itemA);
        _ui->_contactTable->setItem (row, 1, itemB);
        _ui->_contactTable->setItem (row, 2, itemC);
        row++;
    }
    _ui->_contactTable->setSortingEnabled (true);
    if (nrOfContactsToShow > 0)
        _ui->_contactTable->setRangeSelected (
            QTableWidgetSelectionRange (0, 0, nrOfContactsToShow - 1, 2), true);
}

void ContactSetWidget::contactSetChanged (const QItemSelection&, const QItemSelection&)
{
    const QModelIndexList indexes = _ui->_contactTable->selectionModel ()->selectedIndexes ();
    std::vector< Contact > contactsPen;
    std::vector< Contact > contactsNon;
    foreach (QModelIndex index, indexes)
    {
        if (index.column () > 0)
            continue;
        const std::size_t i = index.data (Qt::UserRole).toUInt ();
        const Contact& c    = _contactSet->getContact (i);
        if (c.getDepth () > 0)
            contactsPen.push_back (c);
        else
            contactsNon.push_back (c);
    }
    _root->removeChild ("Contacts");
    _contactRenders.clear ();
    GroupNode::Ptr contactGroup         = ownedPtr (new GroupNode ("Contacts"));
    const RenderContacts::Ptr renderPen = ownedPtr (new RenderContacts ());
    const RenderContacts::Ptr renderNon = ownedPtr (new RenderContacts ());
    renderPen->setColorPoints (1, 0, 0);
    renderNon->setColorPoints (0, 1, 0);
    renderPen->setContacts (contactsPen);
    renderNon->setContacts (contactsNon);
    const DrawableNode::Ptr drawablePen =
        _graph->makeDrawable ("Penetration", renderPen, DrawableNode::Physical);
    const DrawableNode::Ptr drawableNon =
        _graph->makeDrawable ("Non-Penetration", renderNon, DrawableNode::Physical);
    GroupNode::addChild (drawablePen, contactGroup);
    GroupNode::addChild (drawableNon, contactGroup);
    GroupNode::addChild (contactGroup, _root);
    drawablePen->setVisible (true);
    drawableNon->setVisible (true);

    const double radius = _ui->_scalingRadius->value () / 1000.;
    const double normal = _ui->_scalingNormal->value () / 1000.;
    const bool pointA   = _ui->_showPointA->isChecked ();
    const bool pointB   = _ui->_showPointB->isChecked ();
    const bool normalA  = _ui->_showNormalA->isChecked ();
    const bool normalB  = _ui->_showNormalB->isChecked ();
    renderPen->setSphereRadius (radius);
    renderNon->setSphereRadius (radius);
    renderPen->setNormalLength (normal);
    renderNon->setNormalLength (normal);
    renderPen->showPoints (pointA, pointB);
    renderNon->showPoints (pointA, pointB);
    renderPen->showNormals (normalA, normalB);
    renderNon->showNormals (normalA, normalB);
    _contactRenders.push_back (renderPen);
    _contactRenders.push_back (renderNon);

    emit graphicsUpdated ();
}

void ContactSetWidget::scalingChanged (double d)
{
    if (!_properties.isNull ()) {
        if (QObject::sender () == _ui->_scalingRadius)
            _properties->set ("ContactSetWidget_PointRadius", d / 1000.);
        else if (QObject::sender () == _ui->_scalingNormal)
            _properties->set ("ContactSetWidget_NormalLength", d / 1000.);
    }

    const double radius = _ui->_scalingRadius->value () / 1000.;
    const double normal = _ui->_scalingNormal->value () / 1000.;
    for (const RenderContacts::Ptr& render : _contactRenders) {
        render->setSphereRadius (radius);
        render->setNormalLength (normal);
    }
    emit graphicsUpdated ();
}

void ContactSetWidget::showChanged (int state)
{
    if (!_properties.isNull ()) {
        if (QObject::sender () == _ui->_showPointA)
            _properties->set ("ContactSetWidget_ShowPointA", state == Qt::Checked);
        else if (QObject::sender () == _ui->_showPointB)
            _properties->set ("ContactSetWidget_ShowPointB", state == Qt::Checked);
        else if (QObject::sender () == _ui->_showNormalA)
            _properties->set ("ContactSetWidget_ShowNormalA", state == Qt::Checked);
        else if (QObject::sender () == _ui->_showNormalB)
            _properties->set ("ContactSetWidget_ShowNormalB", state == Qt::Checked);
    }

    const bool pointA  = _ui->_showPointA->isChecked ();
    const bool pointB  = _ui->_showPointB->isChecked ();
    const bool normalA = _ui->_showNormalA->isChecked ();
    const bool normalB = _ui->_showNormalB->isChecked ();
    for (const RenderContacts::Ptr& render : _contactRenders) {
        render->showPoints (pointA, pointB);
        render->showNormals (normalA, normalB);
    }
    emit graphicsUpdated ();
}

QString ContactSetWidget::toQString (const Vector3D<>& vec)
{
    std::stringstream str;
    str << vec;
    return QString::fromStdString (str.str ());
}

QString ContactSetWidget::toQString (const Contact& contact)
{
    const QString nameA = QString::fromStdString (contact.getNameA ());
    const QString nameB = QString::fromStdString (contact.getNameB ());
    return "Bodies: " + nameA + " - " + nameB + "<br/>" +
           "Points: " + toQString (contact.getPointA ()) + " - " +
           toQString (contact.getPointB ()) + "<br/>" +
           "Normal: " + toQString (contact.getNormal ()) + "<br/>" +
           "Depth: " + QString::number (contact.getDepth ());
}

ContactSetWidget::Dispatcher::Dispatcher ()
{}

ContactSetWidget::Dispatcher::~Dispatcher ()
{}

SimulatorLogEntryWidget*
ContactSetWidget::Dispatcher::makeWidget (rw::core::Ptr< const SimulatorLog > entry,
                                          QWidget* parent) const
{
    const rw::core::Ptr< const LogContactSet > contactSet = entry.cast< const LogContactSet > ();
    if (!(contactSet == NULL))
        return new ContactSetWidget (contactSet, parent);
    RW_THROW ("ContactSetWidget::Dispatcher (makeWidget): invalid entry!");
    return NULL;
}

bool ContactSetWidget::Dispatcher::accepts (rw::core::Ptr< const SimulatorLog > entry) const
{
    if (!(entry.cast< const LogContactSet > () == NULL))
        return true;
    return false;
}
