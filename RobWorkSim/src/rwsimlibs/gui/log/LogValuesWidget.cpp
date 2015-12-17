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

#include "LogValuesWidget.hpp"

#include "../../../rwsim/log/LogValues.hpp"
#include "ui_LogValuesWidget.h"


using namespace rw::common;
using namespace rw::graphics;
using namespace rw::kinematics;
using namespace rw::math;
using namespace rw::models;
using namespace rwsim::dynamics;
using namespace rwsim::log;
using namespace rwsimlibs::gui;

LogValuesWidget::LogValuesWidget(rw::common::Ptr<const LogValues> entry, QWidget* parent):
	SimulatorLogEntryWidget(parent),
	_ui(new Ui::LogValuesWidget()),
	_values(entry)
{
	_ui->setupUi(this);

	QStringList headerLabels;
	headerLabels.push_back("Label");
	headerLabels.push_back("Value");
	_ui->_values->setHorizontalHeaderLabels(headerLabels);

	_ui->_values->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

LogValuesWidget::~LogValuesWidget() {
}

void LogValuesWidget::setDWC(rw::common::Ptr<const DynamicWorkCell> dwc) {
}

void LogValuesWidget::setEntry(rw::common::Ptr<const SimulatorLog> entry) {
	const rw::common::Ptr<const LogValues> set = entry.cast<const LogValues>();
	if (!(set == NULL))
		_values = set;
	else
		RW_THROW("LogValuesWidget (setEntry): invalid entry!");
}

rw::common::Ptr<const SimulatorLog> LogValuesWidget::getEntry() const {
	return _values;
}

void LogValuesWidget::updateEntryWidget() {
	_ui->_description->setText(QString::fromStdString(_values->getDescription()));
	_ui->_values->setRowCount(_values->size());
	_ui->_values->setSortingEnabled(false);
	for (std::size_t i = 0; i < _values->size(); i++) {
		std::string label = "No Label";
		if (_values->size() >= i) {
			if (_values->getLabel(i) != "")
				label = _values->getLabel(i);
		}
		_ui->_values->setItem(i,0,new QTableWidgetItem(QString::fromStdString(label)));
		_ui->_values->setItem(i,1,new QTableWidgetItem(QString::number(_values->getValue(i))));
	}
	_ui->_values->setSortingEnabled(true);
}

void LogValuesWidget::showGraphics(rw::common::Ptr<GroupNode> root, rw::common::Ptr<SceneGraph> graph) {
}

std::string LogValuesWidget::getName() const {
	return "Values";
}

LogValuesWidget::Dispatcher::Dispatcher() {
}

LogValuesWidget::Dispatcher::~Dispatcher() {
}

SimulatorLogEntryWidget* LogValuesWidget::Dispatcher::makeWidget(rw::common::Ptr<const SimulatorLog> entry, QWidget* parent) const {
	const rw::common::Ptr<const LogValues> tentry = entry.cast<const LogValues>();
	if (!(tentry == NULL))
		return new LogValuesWidget(tentry, parent);
	RW_THROW("LogValuesWidget::Dispatcher (makeWidget): invalid entry!");
	return NULL;
}

bool LogValuesWidget::Dispatcher::accepts(rw::common::Ptr<const SimulatorLog> entry) const {
	if (!(entry.cast<const LogValues>() == NULL))
		return true;
	return false;
}