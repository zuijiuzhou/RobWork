/********************************************************************************
 * Copyright 2009 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#include "DynamicWorkCell.hpp"

#include "Body.hpp"

#include <rw/math/Vector3D.hpp>
#include <rw/models/Device.hpp>
#include <rw/models/WorkCell.hpp>

using namespace rw::math;
using namespace rw::models;
using namespace rw::kinematics;
using namespace rw::proximity;
using namespace rwlibs::simulation;

using namespace rwsim::dynamics;

DynamicWorkCell::DynamicWorkCell (WorkCell::Ptr workcell) :
    _workcell (workcell), _collisionMargin (0.001),
    _worldDimension (Vector3D<> (0, 0, 0), Vector3D<> (20, 20, 20)), _gravity (0, 0, -9.82)
{}

DynamicWorkCell::DynamicWorkCell (WorkCell::Ptr workcell, const DynamicWorkCell::BodyList& bodies,
                                  const DynamicWorkCell::BodyList& allbodies,
                                  const ConstraintList& constraints,
                                  const DynamicWorkCell::DeviceList& devices,
                                  const ControllerList& controllers) :
    _workcell (workcell),
    _bodies (bodies), _allbodies (allbodies), _constraints (constraints), _devices (devices),
    _controllers (controllers), _collisionMargin (0.001),
    _worldDimension (Vector3D<> (0, 0, 0), Vector3D<> (20, 20, 20)), _gravity (0, 0, -9.82)
{
    for (SimulatedController::Ptr b : _controllers) {
        if (!b->getControllerModel ()->isRegistered ())
            workcell->add (b->getControllerModel ());
        if (!b->isRegistered ())
            b->registerIn (workcell->getStateStructure ());
    }

    for (Constraint::Ptr b : _constraints) {
        workcell->getStateStructure ()->addData (b);
    }

    for (DynamicDevice::Ptr b : _devices) {
        b->registerIn (workcell->getStateStructure ());
    }

    for (Body::Ptr b : _allbodies) {
        _frameToBody[b->getBodyFrame ()] = b;
        b->registerIn (workcell->getStateStructure ());
    }
}

DynamicWorkCell::~DynamicWorkCell ()
{}

DynamicDevice::Ptr DynamicWorkCell::findDevice (const std::string& name) const
{
    for (const DynamicDevice::Ptr& ddev : _devices) {
        if (ddev->getModel ().getName () == name)
            return ddev;
    }
    return NULL;
}

bool DynamicWorkCell::inDevice (rw::core::Ptr< const Body > body) const
{
    // inspect name scope, if its the same as any device then
    const std::string& bname = body->getName ();
    for (DynamicDevice::Ptr dev : _devices) {
        const std::string& devname = dev->getName ();
        bool isInDev               = true;
        for (int i = 0; i < (int) devname.length (); i++)
            if (devname[i] != bname[i]) {
                isInDev = false;
                break;
            }
        if (isInDev)
            return true;
    }
    return false;
}

rwlibs::simulation::SimulatedController::Ptr
DynamicWorkCell::findController (const std::string& name) const
{
    for (rwlibs::simulation::SimulatedController::Ptr ctrl : _controllers) {
        if (ctrl->getControllerName () == name)
            return ctrl;
    }
    return NULL;
}

Body::Ptr DynamicWorkCell::findBody (const std::string& name) const
{
    for (const Body::Ptr& body : _bodies) {
        if (body->getName () == name)
            return body;
    }
    return NULL;
}

SimulatedSensor::Ptr DynamicWorkCell::findSensor (const std::string& name) const
{
    for (SimulatedSensor::Ptr sensor : _sensors) {
        if (sensor->getName () == name)
            return sensor;
    }
    return NULL;
}

Body::Ptr DynamicWorkCell::getBody (rw::core::Ptr<rw::kinematics::Frame> f)
{
    if (_frameToBody.find (f.get()) == _frameToBody.end ())
        return NULL;
    return _frameToBody[f.get()];
}

void DynamicWorkCell::addBody (Body::Ptr body)
{
    body->registerIn (_workcell->getStateStructure ());
    _frameToBody[body->getBodyFrame ()] = body;
    _allbodies.push_back (body);
    _bodies.push_back (body);
}

void DynamicWorkCell::addConstraint (Constraint::Ptr constraint)
{
    _workcell->getStateStructure ()->addData (constraint);
    _constraints.push_back (constraint);
}

void DynamicWorkCell::addSensor (rwlibs::simulation::SimulatedSensor::Ptr sensor)
{
    if (!sensor->getSensorModel ()->isRegistered ())
        _workcell->add (sensor->getSensorModel ());
    if (!sensor->isRegistered ())
        sensor->registerIn (_workcell->getStateStructure ());
    _sensors.push_back (sensor);
    _changedEvent.fire (SensorAddedEvent, boost::any (sensor));
}

Constraint::Ptr DynamicWorkCell::findConstraint (const std::string& name) const
{
    for (const Constraint::Ptr& constraint : _constraints) {
        if (constraint->getName () == name)
            return constraint;
    }
    return NULL;
}
