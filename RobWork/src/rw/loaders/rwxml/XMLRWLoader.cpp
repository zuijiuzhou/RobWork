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

#include "XMLRWLoader.hpp"

#include "XMLParserUtil.hpp"
#include "XMLRWParser.hpp"

#include <RobWorkConfig.hpp>
#include <rw/kinematics/Frame.hpp>
#include <rw/kinematics/State.hpp>
#include <rw/kinematics/StateStructure.hpp>
//#include <rw/kinematics/FrameType.hpp>
#include <rw/core/IOUtil.hpp>
#include <rw/core/Property.hpp>
#include <rw/core/StringUtil.hpp>
#include <rw/core/macros.hpp>
#include <rw/graphics/SceneDescriptor.hpp>
#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/loaders/GeometryFactory.hpp>
#include <rw/loaders/Model3DFactory.hpp>
#include <rw/loaders/colsetup/CollisionSetupLoader.hpp>
#include <rw/loaders/dom/DOMProximitySetupLoader.hpp>
#include <rw/math/Constants.hpp>
#include <rw/math/RPY.hpp>
#include <rw/math/Transform3D.hpp>
#include <rw/models/DHParameterSet.hpp>
#include <rw/models/DependentPrismaticJoint.hpp>
#include <rw/models/DependentRevoluteJoint.hpp>
#include <rw/models/Joint.hpp>
#include <rw/models/MobileDevice.hpp>
#include <rw/models/ParallelDevice.hpp>
#include <rw/models/ParallelLeg.hpp>
#include <rw/models/PrismaticJoint.hpp>
#include <rw/models/PrismaticSphericalJoint.hpp>
#include <rw/models/PrismaticUniversalJoint.hpp>
#include <rw/models/RevoluteJoint.hpp>
#include <rw/models/RigidObject.hpp>
#include <rw/models/SerialDevice.hpp>
#include <rw/models/SphericalJoint.hpp>
#include <rw/models/TreeDevice.hpp>
#include <rw/models/UniversalJoint.hpp>
#include <rw/proximity/CollisionSetup.hpp>
#include <rw/proximity/ProximitySetup.hpp>

#include <boost/lexical_cast.hpp>
#include <stack>
#include <memory>

using namespace rw::math;
using namespace rw::core;
using namespace rw::kinematics;
using namespace rw::models;
using namespace rw::graphics;
using namespace rw::loaders;
using namespace rw::proximity;
using namespace rw::geometry;
using namespace rw;

#define RW_DEBUGS(str)

namespace {

struct InitialAction
{
  public:
    virtual void setInitialState (rw::kinematics::State& state) = 0;
    virtual ~InitialAction () {}
};

struct DeviceInitState : public InitialAction
{
  private:
    rw::math::Q _q;
    rw::models::Device::Ptr _dev;

  public:
    DeviceInitState (rw::math::Q q, rw::models::Device::Ptr dev) : _q (q), _dev (dev) {}

    virtual ~DeviceInitState (){};

    void setInitialState (rw::kinematics::State& state) { _dev->setQ (_q, state); }
};

struct MovableInitState : public InitialAction
{
  private:
    MovableFrame* _frame;
    Transform3D<> _t3d;

  public:
    MovableInitState (MovableFrame* frame, Transform3D<> t3d) : _frame (frame), _t3d (t3d) {}

    virtual ~MovableInitState () {}

    void setInitialState (rw::kinematics::State& state) { _frame->setTransform (_t3d, state); }
};

// container for collision setups, filename and scoped name
typedef std::vector< DummyCollisionSetup > ColSetupList;
typedef std::vector< DummyProximitySetup > ProxSetupList;

struct DummySetup
{
  public:
    rw::core::Ptr<Frame> world;
    StateStructure* tree;

    std::map< std::string, Frame* > frameMap;
    std::map< std::string, DummyFrame* > dummyFrameMap;
    std::map< std::string, std::vector< Frame* > > toChildMap;
    std::map< Frame*, RigidObject::Ptr > objectMap;
    std::vector< InitialAction* > actions;
    std::shared_ptr< DummyWorkcell > dwc;

    ColSetupList colsetups;
    ProxSetupList proxsetups;
    std::map< std::string, Device::Ptr > devMap;

    rw::graphics::SceneDescriptor::Ptr scene;
    std::string wcFilename;
    std::vector< std::string > proxSetupFilenames;
};

void addPropertyToMap (const DummyProperty& dprop, core::PropertyMap& map)
{
    if (dprop._type == "string") {
        map.add (dprop._name, dprop._desc, dprop._val);
    }
    else if (dprop._type == "double") {
        try {
            double val = boost::lexical_cast< double > (dprop._val);
            map.add (dprop._name, dprop._desc, val);
        }
        catch (const std::exception& e) {
            RW_WARN ("Could not parse double property value: \""
                     << dprop._name << "\" from: \"" << dprop._val << "\"" << ". An error occured:\n " << std::string (e.what ()));
        }
    }
    else if (dprop._type == "Q") {
        std::stringstream istr (dprop._val);
        std::vector< double > res;
        while (!istr.eof ()) {
            double d;
            istr >> d;
            res.push_back (d);
        }
        Q val (res);
        map.add (dprop._name, dprop._desc, val);
    }
}

// the parent frame must exist in tree already
void addToStateStructure (rw::core::Ptr<Frame> parent, DummySetup& setup)
{
    std::vector< Frame* > children = setup.toChildMap[parent->getName ()];
    for (Frame* child : children) {
        DummyFrame* dframe = setup.dummyFrameMap[child->getName ()];
        RW_DEBUGS ("Adding: " << parent->getName () << "<--" << dframe->getName ());
        if (dframe->_isDaf)
            setup.tree->addDAF (child, parent);
        else
            setup.tree->addFrame (child, parent);
        addToStateStructure (child, setup);
    }
}

// the parent to name must exist in tree
void addToStateStructure (const std::string& name, DummySetup& setup)
{
    DummyFrame* dframe = setup.dummyFrameMap[name];
    RW_DEBUGS ("RefFrame : " << dframe->getRefFrame ());
    rw::core::Ptr<Frame> parent = setup.frameMap[dframe->getRefFrame ()];
    if (parent == NULL)
        RW_THROW ("PARENT IS NULL");

    rw::core::Ptr<Frame> child = setup.frameMap[dframe->getName ()];
    if (dframe->_isDaf)
        setup.tree->addDAF (child, parent);
    else
        setup.tree->addFrame (child, parent);
    addToStateStructure (child, setup);
}

std::string createScopedName (std::string str, std::vector< std::string > scope)
{
    std::string tmpstr;
    for (size_t i = 0; i < scope.size (); i++) {
        tmpstr += scope[i] + ".";
    }
    tmpstr += str;
    return tmpstr;
}

typedef std::map< std::string, Frame* > FrameMap;

Frame* addModelToFrame (DummyModel& model,  rw::core::Ptr<Frame> parent, StateStructure* tree, DummySetup& setup)
{
    rw::core::Ptr<Frame> modelframe                = parent;
    std::vector< std::string > scope = model._scope;

    for (size_t i = 0; i < model._geo.size (); i++) {
        std::ostringstream val;

        switch (model._geo[i]._type) {
            case PolyType:
                if (!StringUtil::isAbsoluteFileName (model._geo[i]._filename + ".tmp")) {
                    val << StringUtil::getDirectoryName (model._geo[i]._pos.file);
                }
                val << model._geo[i]._filename;
                break;
            case PlaneType: val << "#Plane"; break;
            case CubeType:
                val << "#Box " << model._geo[i]._x << " " << model._geo[i]._y << " "
                    << model._geo[i]._z;
                break;
            case SphereType:
                val << "#Sphere " << model._geo[i]._radius << " " << model._geo[i]._level;
                break;
            case ConeType:
                val << "#Cone " << model._geo[i]._radius << " " << model._geo[i]._z << " "
                    << model._geo[i]._level;
                break;
            case CylinderType:
                val << "#Cylinder " << model._geo[i]._radius << " " << model._geo[i]._z << " "
                    << model._geo[i]._level;
                break;
            case TubeType:
                // #Tube radius thickness height divisions
                val << "#Tube " << model._geo[i]._radius << " " << model._geo[i]._x << " "
                    << model._geo[i]._z << " " << model._geo[i]._level;
                break;
            case CustomType:
                val << "#Custom " << model._geo[i]._filename << " " << model._geo[i]._parameters;
                break;
            default: val << ""; break;
        }

        RigidObject::Ptr object;
        if (model._isDrawable || model._colmodel) {
            if (setup.objectMap.find (modelframe.get()) == setup.objectMap.end ())
                setup.objectMap[modelframe.get()] = ownedPtr (new RigidObject (modelframe));
            object = setup.objectMap[modelframe.get()];
        }

        // try {
        RW_DEBUGS ("Loading Model....");
        if (model._colmodel && model._isDrawable) {
            // the geom is to be used as both collision geometry and visualization model
            // TODO: this could be optimized, share data and such.
            Model3D::Ptr model3d = Model3DFactory::getModel (val.str (),
                                                             model._name,
                                                             !model._customMaterial,
                                                             Model3D::Material ("stlmat",
                                                                                float (model._r),
                                                                                float (model._g),
                                                                                float (model._b),
                                                                                float (model._a)));
            model3d->setTransform (model._transform);
            model3d->setName (model._name);
            // model->setFrame(modelframe);

            Geometry::Ptr geom = GeometryFactory::load (val.str (), true);
            geom->setName (model._name);
            geom->setTransform (model._transform);
            geom->setFrame (modelframe);

            std::ostringstream filePath;
            if (!StringUtil::isAbsoluteFileName (model._geo[i]._filename + ".tmp")) {
                filePath << StringUtil::getRelativeDirectoryName (
                    model._geo[i]._pos.file, StringUtil::getDirectoryName (setup.wcFilename));
            }
            filePath << model._geo[i]._filename;

            // For Polytype CAD models, we want to save the filepath for potential serialization
            // later.
            if (model._geo[i]._type == PolyType) {
                model3d->setFilePath (filePath.str ());
                geom->setFilePath (filePath.str ());
            }

            if (object != NULL) {
                object->addModel (model3d);
                object->addGeometry (geom);
            }
        }
        else if (model._colmodel) {
            // its only a collision geometry

            Geometry::Ptr geom = GeometryFactory::load (val.str (), true);
            geom->setName (model._name);
            geom->setTransform (model._transform);
            geom->setFrame (modelframe);

            std::ostringstream filePath;
            if (!StringUtil::isAbsoluteFileName (model._geo[i]._filename + ".tmp")) {
                filePath << StringUtil::getRelativeDirectoryName (
                    model._geo[i]._pos.file, StringUtil::getDirectoryName (setup.wcFilename));
            }
            filePath << model._geo[i]._filename;

            // For Polytype CAD models, we want to save the filepath for potential serialization
            // later.
            if (model._geo[i]._type == PolyType) {
                geom->setFilePath (filePath.str ());
            }

            if (object != NULL) {
                object->addGeometry (geom);
            }
        }
        else if (model._isDrawable) {
            // its only a drawable

            Model3D::Ptr model3d = Model3DFactory::getModel (val.str (),
                                                             val.str (),
                                                             !model._customMaterial,
                                                             Model3D::Material ("stlmat",
                                                                                float (model._r),
                                                                                float (model._g),
                                                                                float (model._b),
                                                                                float (model._a)));

            model3d->setName (model._name);
            model3d->setTransform (model._transform);

            std::ostringstream filePath;
            if (!StringUtil::isAbsoluteFileName (model._geo[i]._filename + ".tmp")) {
                filePath << StringUtil::getRelativeDirectoryName (
                    model._geo[i]._pos.file, StringUtil::getDirectoryName (setup.wcFilename));
            }
            filePath << model._geo[i]._filename;

            // For Polytype CAD models, we want to save the filepath for potential serialization
            // later.
            if (model._geo[i]._type == PolyType) {
                model3d->setFilePath (filePath.str ());
            }
            else {
                model3d->setFilePath (val.str ());
            }

            if (object != NULL) {
                object->addModel (model3d);
            }
        }
        //} catch (const std::exception& e){
        //}
    }
    return modelframe.get();
}

void addLimits (std::vector< DummyLimit >& limits, Joint::Ptr j)
{
    if (limits.size () == 0)
        return;

    std::vector< DummyLimit > posLimits;
    std::vector< DummyLimit > velLimits;
    std::vector< DummyLimit > accLimits;
    Q convFactor (limits.size ());
    for (std::size_t i = 0; i < limits.size (); i++) {
        convFactor[i] = 1;
        switch (limits[i]._type) {
            case PosLimitType: posLimits.push_back (limits[i]); break;
            case VelLimitType: velLimits.push_back (limits[i]); break;
            case AccLimitType: accLimits.push_back (limits[i]); break;
        }
    }

    if (j.cast< RevoluteJoint> () != NULL) {
        convFactor[0] = Deg2Rad;
    }
    else if (j.cast< SphericalJoint> () != NULL) {
        convFactor[0] = Deg2Rad;
        convFactor[1] = Deg2Rad;
        convFactor[2] = Deg2Rad;
    }
    else if (j.cast< PrismaticSphericalJoint > () != NULL) {
        convFactor[0] = Deg2Rad;
        convFactor[1] = Deg2Rad;
        convFactor[2] = Deg2Rad;
    }
    else if (j.cast< UniversalJoint > () != NULL) {
        convFactor[0] = Deg2Rad;
        convFactor[1] = Deg2Rad;
    }
    else if (j.cast< PrismaticUniversalJoint> () != NULL) {
        convFactor[0] = Deg2Rad;
        convFactor[1] = Deg2Rad;
    }

    Q minPos (posLimits.size ());
    Q maxPos (posLimits.size ());
    Q maxVel (velLimits.size ());
    Q maxAcc (accLimits.size ());
    for (std::size_t i = 0; i < posLimits.size (); i++) {
        minPos[i] = posLimits[i]._min * convFactor[i];
        maxPos[i] = posLimits[i]._max * convFactor[i];
    }
    for (std::size_t i = 0; i < velLimits.size (); i++) {
        maxVel[i] = velLimits[i]._max * convFactor[i];
    }
    for (std::size_t i = 0; i < accLimits.size (); i++) {
        maxAcc[i] = accLimits[i]._max * convFactor[i];
    }
    if (posLimits.size () > 0)
        j->setBounds (std::pair< Q, Q > (minPos, maxPos));
    if (velLimits.size () > 0)
        j->setMaxVelocity (maxVel);
    if (accLimits.size () > 0)
        j->setMaxAcceleration (maxAcc);
    return;
}

void addLimitsToFrame (std::vector< DummyLimit >& limits,  rw::core::Ptr<Frame> f)
{
    Joint::Ptr j = f.cast<Joint>();
    if (j == NULL)
        return;
    addLimits (limits, j);
}

Frame* createFrame (DummyFrame& dframe, DummySetup& setup)
{
    rw::core::Ptr<Frame> frame = NULL;

    if (dframe._isDepend) {
        std::map< std::string, Frame* >::iterator res =
            setup.frameMap.find (dframe.getDependsOn ());
        if (res == setup.frameMap.end ()) {
            const std::string dependOn =
                dframe.getDependsOn ().substr (dframe.getScoped ("").size ());
            std::vector< std::string > scope = dframe._scope;
            while (scope.size () > 0 && res == setup.frameMap.end ()) {
                res = setup.frameMap.find (createScopedName (dependOn, scope));
                scope.resize (scope.size () - 1);
            }
            if (res == setup.frameMap.end ()) {
                RW_WARN ("The frame: " << dframe.getName () << " Depends on an unknown frame: "
                                       << dframe.getDependsOn ());
                return NULL;
            }
        }
        // then the frame depends on another joint
        Joint* owner = dynamic_cast< Joint* > ((*res).second);
        if (owner == NULL) {
            RW_THROW ("The frame: " << dframe.getName () << " Depends on an frame: "
                                    << dframe._dependsOn << " which is not a Joint");
        }

        if (dframe._type == "Revolute") {
            DependentRevoluteJoint* const joint = new DependentRevoluteJoint (
                dframe.getName (), dframe._transform, owner, dframe._gain, dframe._offset);
            if (dframe._state != ActiveState || !owner->isActive ())
                joint->setActive (false);
            frame = joint;
        }
        else if (dframe._type == "Prismatic") {
            DependentPrismaticJoint* const joint = new DependentPrismaticJoint (
                dframe.getName (), dframe._transform, owner, dframe._gain, dframe._offset);
            if (dframe._state != ActiveState || !owner->isActive ())
                joint->setActive (false);
            frame = joint;
        }
        else {
            RW_THROW ("Error: The type of frame: " << dframe.getName ()
                                                   << " cannot depend on another joint!!");
        }
        // Accessor::frameType().set(*frame, rw::kinematics::FrameType::DependentJoint);
    }
    else if (dframe._type == "Fixed") {
        frame = new FixedFrame (dframe.getName (), dframe._transform);
        // Accessor::frameType().set(*frame, rw::kinematics::FrameType::FixedFrame);
    }
    else if (dframe._type == "Movable") {
        MovableFrame* mframe = new MovableFrame (dframe.getName ());
        frame                = mframe;
        // Accessor::frameType().set(*mframe, rw::kinematics::FrameType::MovableFrame);
        MovableInitState* init = new MovableInitState (mframe, dframe._transform);
        setup.actions.push_back (init);
    }
    else if (dframe._type == "Prismatic") {
        PrismaticJoint* j = new PrismaticJoint (dframe.getName (), dframe._transform);
        addLimits (dframe._limits, j);
        frame = j;
        // Accessor::frameType().set(*frame, rw::kinematics::FrameType::PrismaticJoint);
        if (dframe._state != ActiveState)
            j->setActive (false);
        // Accessor::activeJoint().set(*frame, true);
    }
    else if (dframe._type == "Revolute") {
        RevoluteJoint* j = new RevoluteJoint (dframe.getName (), dframe._transform);
        addLimits (dframe._limits, j);
        frame = j;
        // Accessor::frameType().set(*frame, rw::kinematics::FrameType::RevoluteJoint);

        if (dframe._state != ActiveState)
            j->setActive (false);
        // Accessor::activeJoint().set(*frame, true);
    }
    else if (dframe._type == "Spherical") {
        SphericalJoint* j = new SphericalJoint (dframe.getName (), dframe._transform);
        addLimits (dframe._limits, j);
        frame = j;

        if (dframe._state != ActiveState)
            j->setActive (false);
    }
    else if (dframe._type == "PrismaticSpherical") {
        PrismaticSphericalJoint* j =
            new PrismaticSphericalJoint (dframe.getName (), dframe._transform);
        addLimits (dframe._limits, j);
        frame = j;

        if (dframe._state != ActiveState)
            j->setActive (false);
    }
    else if (dframe._type == "Universal") {
        UniversalJoint* j = new UniversalJoint (dframe.getName (), dframe._transform);
        addLimits (dframe._limits, j);
        frame = j;

        if (dframe._state != ActiveState)
            j->setActive (false);
    }
    else if (dframe._type == "PrismaticUniversal") {
        PrismaticUniversalJoint* j =
            new PrismaticUniversalJoint (dframe.getName (), dframe._transform);
        addLimits (dframe._limits, j);
        frame = j;

        if (dframe._state != ActiveState)
            j->setActive (false);
    }
    else if (dframe._type == "EndEffector") {
        frame = new FixedFrame (dframe.getName (), dframe._transform);
        // Accessor::frameType().set(*frame, rw::kinematics::FrameType::FixedFrame);
    }
    else {
        RW_THROW ("FRAME is of illegal type!! " << dframe._type);
    }
    // add dhparam as property to the frame if dh params was specified
    if (dframe._hasDHparam) {
        DHParam& param = dframe._dhparam;

        if (param._dhtype == Revolute) {
            if (param._type == "HGP" && param._hgptype == "parallel") {
                rw::models::DHParameterSet dhset (
                    param._alpha, param._a, param._offset, param._b, true);
                DHParameterSet::set (dhset, frame);
            }
            else {
                rw::models::DHParameterSet dhset (
                    param._alpha, param._a, param._d, param._offset, param._type);
                DHParameterSet::set (dhset, frame);
            }
        }
        else if (param._dhtype == Prismatic) {
            if (param._type == "HGP" && param._hgptype == "parallel") {
                rw::models::DHParameterSet dhset (
                    param._alpha, param._a, param._beta, param._offset, true);
                DHParameterSet::set (dhset, frame);
            }
            else {
                rw::models::DHParameterSet dhset (
                    param._alpha, param._a, param._offset, param._theta, param._type);
                DHParameterSet::set (dhset, frame);
            }
        }
    }

    // remember to add the frame to the frame map
    setup.dummyFrameMap[dframe.getName ()] = &dframe;
    setup.frameMap[frame->getName ()]      = frame.get();
    setup.toChildMap[dframe.getRefFrame ()].push_back (frame.get());
    RW_DEBUGS ("Frame created: " << frame->getName () << " --> " << dframe.getRefFrame ());
    return frame.get();
}

void addFrameProps (DummyFrame& dframe, DummySetup& setup)
{
    rw::core::Ptr<Frame> frame = setup.frameMap[dframe.getName ()];
    for (size_t i = 0; i < dframe._properties.size (); i++) {
        const DummyProperty& dprop = dframe._properties[i];
        addPropertyToMap (dprop, frame->getPropertyMap ());
        // frame->getPropertyMap().add(dprop._name, dprop._desc, dprop._val);
    }
    for (size_t i = 0; i < dframe._models.size (); i++) {
        addModelToFrame (dframe._models[i], frame, setup.tree, setup);
    }
}

/**
 * @brief adds all device context defined properties to the frame
 */
void addDevicePropsToFrame (DummyDevice& dev, const std::string& name, DummySetup& setup)
{
    rw::core::Ptr<Frame> frame = setup.frameMap[name];
    // add properties specified in device context
    std::vector< std::shared_ptr< rw::core::Property< std::string > > > proplist =
        dev._propMap[frame->getName ()];

    for (size_t j = 0; j < proplist.size (); j++) {
        frame->getPropertyMap ().add (proplist[j]->getIdentifier (),
                                      proplist[j]->getDescription (),
                                      proplist[j]->getValue ());
    }

    // add models specified in device context
    std::vector< DummyModel > modellist = dev._modelMap[frame->getName ()];
    for (size_t j = 0; j < modellist.size (); j++) {
        addModelToFrame (modellist[j], frame, setup.tree, setup);
    }

    // add limits to frame
    std::vector< DummyLimit > limits = dev._limitMap[frame->getName ()];
    addLimitsToFrame (limits, frame);
}

Device::Ptr createDevice (DummyDevice& dev, DummySetup& setup)
{
    Device::Ptr model = NULL;
    if (dev._type == SerialType) {
        std::vector< Frame* > chain;
        // add the rest of the chain
        for (DummyFrame& dframe : dev._frames) {
            chain.push_back (createFrame (dframe, setup));
        }
        // next add the device and model properties to the frames
        addToStateStructure (chain[0]->getName (), setup);
        // lastly add any props
        for (DummyFrame& dframe : dev._frames) {
            addFrameProps (dframe, setup);
            addDevicePropsToFrame (dev, dframe.getName (), setup);
        }
        // State state( tree );
        State state = setup.tree->getDefaultState ();
        model = ownedPtr (new SerialDevice (chain.front (), chain.back (), dev.getName (), state));
    }
    else if (dev._type == ParallelType) {
        // a parallel device is composed of a number of serial chains
        std::vector< std::vector< Frame* > > chains;
        std::map< std::string, std::size_t > nameToChainIndex;
        std::vector< Frame* > chain;
        std::map< std::string, Frame* > addFramesMap;
        rw::core::Ptr<Frame> parent = createFrame (dev._frames[0], setup);    // base
        RW_ASSERT (parent);
        chain.push_back (parent.get());
        addFramesMap[parent->getName ()] = parent.get();
        std::string chainName;
        for (size_t i = 1; i < dev._frames.size (); i++) {
            rw::core::Ptr<Frame> frame = NULL;
            bool newLeg;
            if (dev._frames[i].getRefFrame () == dev._frames[i - 1].getName ()) {
                newLeg = false;
                // Start new leg anyway if other frames also refer to the same reference frame
                for (std::size_t j = i + 1; j < dev._frames.size () && !newLeg; j++) {
                    if (i >= 2 && dev._frames[j].getRefFrame () == dev._frames[i - 1].getName ()) {
                        newLeg = true;
                    }
                }
                // Start new leg anyway if scope changes
                if (!chainName.empty ()) {
                    for (std::size_t j = i + 1; j < dev._frames.size () && !newLeg; j++) {
                        if (dev._frames[j].getRefFrame () != dev._frames[j - 1].getName ())
                            break;
                        if (dev._frames[j]._scope.back () == chainName ||
                            dev._frames[j]._scope.size () <= 1)
                            break;
                        newLeg = true;
                    }
                }
            }
            else {
                newLeg = true;
            }
            if (!newLeg) {
                // the last frame was parent frame
                frame = createFrame (dev._frames[i], setup);
                if (dev._frames[i]._scope.back () != dev._name)
                    chainName = dev._frames[i]._scope.back ();
            }
            else {
                // a new serial leg has started, find the parent frame
                nameToChainIndex[chainName] = chains.size ();
                chainName                   = "";
                chains.push_back (chain);
                parent = NULL;
                std::map< std::string, Frame* >::iterator res =
                    addFramesMap.find (dev._frames[i].getRefFrame ());

                if (res == addFramesMap.end ()) {
                    RW_WARN ("Error: the frame \""
                             << dev._frames[i].getName ()
                             << "\" references to a non existing or illegal frame!!"
                             << dev._frames[i].getRefFrame ());
                    RW_ASSERT (false);
                }
                else {
                    parent = (*res).second;
                    frame  = createFrame (dev._frames[i], setup);
                    chain.clear ();
                    chain.push_back (parent.get());
                }
            }
            if (frame == NULL)
                RW_THROW ("Could not create ParallelDevice! A frame could not be created.");
            chain.push_back (frame.get());
            // tree->addFrame(frame, parent);
            addFramesMap[frame->getName ()] = frame.get();
            parent                          = frame;
        }
        nameToChainIndex[chainName] = chains.size ();
        chains.push_back (chain);
        // Add frames to tree
        addToStateStructure (chains[0][0]->getName (), setup);
        for (DummyFrame& dframe : dev._frames) {
            // Add properties defined in device context
            addFrameProps (dframe, setup);
            addDevicePropsToFrame (dev, dframe.getName (), setup);
        }
        // Create the parallel legs
        std::vector< ParallelLeg* > legs;
        std::vector< Joint* > joints;
        // tree->addFrame(*(chains[0])[0]);
        for (size_t i = 0; i < chains.size (); i++) {
            if (dev._junctions.size () == 0)
                legs.push_back (new ParallelLeg (chains[i]));
            for (std::size_t j = 0; j < chains[i].size (); j++) {
                if (Joint* const joint = dynamic_cast< Joint* > (chains[i][j]))
                    joints.push_back (joint);
            }
        }
        // And last create ParallelDevice
        // State state( tree );
        State state = setup.tree->getDefaultState ();
        std::vector< ParallelDevice::Legs > junctions;
        for (std::size_t i = 0; i < dev._junctions.size (); i++) {
            ParallelDevice::Legs junctionChains;
            for (std::size_t j = 0; j < dev._junctions[i].chains.size (); j++) {
                const std::vector< std::string > split =
                    StringUtil::words (dev._junctions[i].chains[j]);
                std::vector< Frame* > totalChain;
                for (std::size_t k = 0; k < split.size (); k++) {
                    const std::map< std::string, std::size_t >::const_iterator legIndex =
                        nameToChainIndex.find (split[k]);
                    if (legIndex == nameToChainIndex.end ())
                        RW_THROW ("Could not find SerialChain with the name "
                                  << split[k] << " when creating junction.");
                    std::vector< Frame* >::iterator firstFrameIt =
                        chains[legIndex->second].begin ();
                    // Avoid duplications when stitching together chain segments
                    if (totalChain.size () > 0) {
                        if (totalChain.back () == *firstFrameIt) {
                            firstFrameIt++;
                        }
                    }
                    totalChain.insert (
                        totalChain.end (), firstFrameIt, chains[legIndex->second].end ());
                }
                junctionChains.push_back (new ParallelLeg (totalChain));
            }
            junctions.push_back (junctionChains);
        }
        if (junctions.size () == 0) {
            model = ownedPtr (new ParallelDevice (legs, dev.getName (), state));
        }
        else {
            model = ownedPtr (new ParallelDevice (dev.getName (),
                                                  junctions[0][0]->getBase (),
                                                  junctions[0][0]->getEnd (),
                                                  joints,
                                                  state,
                                                  junctions));
        }
    }
    else if (dev._type == TreeType) {
        RW_ASSERT (dev._frames.size () != 0);
        FrameMap frameMap;
        std::vector< Frame* > endEffectors;
        rw::core::Ptr<Frame> base                = createFrame (dev._frames[0], setup);    // base
        frameMap[base->getName ()] = base.get();
        rw::core::Ptr<Frame> child               = base;
        rw::core::Ptr<Frame> parent              = base;
        std::string parentname     = dev._frames[0].getRefFrame ();

        for (size_t i = 1; i < dev._frames.size (); i++) {
            DummyFrame& frame      = dev._frames[i];
            FrameMap::iterator res = frameMap.find (frame.getRefFrame ());
            if (res == frameMap.end ()) {
                RW_THROW ("Error: the frame \""
                          << frame.getName ()
                          << "\" references to a non existing or illegal frame!!"
                          << dev._frames[i].getRefFrame ());
            }
            child                       = createFrame (frame, setup);
            frameMap[child->getName ()] = child.get();

            if (setup.toChildMap[parentname].empty ()) {
                endEffectors.push_back (parent.get());
            }

            if (dev._frames[i]._type == "EndEffector") {
                endEffectors.push_back (child.get());
            }
            parent     = child;
            parentname = frame.getRefFrame ();
        }
        if (endEffectors.size () == 0)
            endEffectors.push_back (child.get());

        addToStateStructure (base->getName (), setup);
        for (DummyFrame& dframe : dev._frames) {
            // Add properties defined in device context
            RW_DEBUGS ("Add props to : " << dframe.getName ());
            addFrameProps (dframe, setup);
            addDevicePropsToFrame (dev, dframe.getName (), setup);
        }
        // And last create TreeDevice
        State state = setup.tree->getDefaultState ();
        model       = ownedPtr (new TreeDevice (base, endEffectors, dev.getName (), state));
    }
    else if (dev._type == MobileType) {
        std::string tmpstr = createScopedName (dev._name, dev._scope) + "." + dev._basename;
        MovableFrame* base = new MovableFrame (tmpstr);
        setup.tree->addDAF (base, setup.tree->getRoot ());
        MovableFrame* mframe = base;
        // Accessor::frameType().set(*mframe, rw::kinematics::FrameType::MovableFrame);
        MovableInitState* init = new MovableInitState (mframe, Transform3D<>::identity ());
        setup.actions.push_back (init);
        setup.frameMap[tmpstr] = base;

        Transform3D<> t3d   = Transform3D<>::identity ();
        t3d.R ()            = RPY<> (0, 0, Pi / 2).toRotation3D ();
        t3d.P ()[1]         = dev._axelwidth / 2;
        tmpstr              = createScopedName (dev._name, dev._scope) + "." + dev._leftname;
        RevoluteJoint* left = new RevoluteJoint (tmpstr, t3d);
        setup.tree->addFrame (left, base);
        setup.frameMap[tmpstr] = left;

        t3d.P ()[1]          = -dev._axelwidth / 2;
        tmpstr               = createScopedName (dev._name, dev._scope) + "." + dev._rightname;
        RevoluteJoint* right = new RevoluteJoint (tmpstr, t3d);
        setup.tree->addFrame (right, base);
        setup.frameMap[tmpstr] = right;

        // add properties, col models, drawables to frames
        addDevicePropsToFrame (dev, base->getName (), setup);
        addDevicePropsToFrame (dev, left->getName (), setup);
        addDevicePropsToFrame (dev, right->getName (), setup);

        for (DummyFrame& dframe : dev._frames) {
            std::string pname = dframe.getRefFrame ();

            std::map< std::string, Frame* >::iterator res = setup.frameMap.find (pname);
            if (res == setup.frameMap.end ()) {
                RW_THROW ("Error: parent " << pname << " not found");
            }
            createFrame (dframe, setup);
        }
        addToStateStructure (base, setup);
        addToStateStructure (left, setup);
        addToStateStructure (right, setup);

        for (DummyFrame& dframe : dev._frames) {
            // Add properties defined in device context
            addFrameProps (dframe, setup);
            addDevicePropsToFrame (dev, dframe.getName (), setup);
        }

        State state = setup.tree->getDefaultState ();
        model       = ownedPtr (new MobileDevice (base, left, right, state, dev.getName ()));
    }
    else if (dev._type == CompositeType) {
        RW_THROW ("CompositeDevice not supported yet");
    }
    else {
        RW_THROW ("Error: Unknown device type!!");
    }

    setup.devMap[dev.getName ()] = model;
    // copy all collision setups from device to global collision setup container
    ColSetupList::iterator colsetup = dev._colsetups.begin ();
    for (; colsetup != dev._colsetups.end (); ++colsetup) {
        setup.colsetups.push_back (*colsetup);
    }

    // copy all collision setups from device to global collision setup container
    ProxSetupList::iterator proxsetup = dev._proxsetups.begin ();
    for (; proxsetup != dev._proxsetups.end (); ++proxsetup) {
        setup.proxsetups.push_back (*proxsetup);
    }

    // add all device properties to device propertymap
    // std::map<std::string, std::vector<DummyProperty> > proplist = dev._propertyMap;
    typedef std::pair< std::string, std::vector< DummyProperty > > DPropValType;
    for (DPropValType val : dev._propertyMap) {
        // std::string name = val.first;
        for (DummyProperty dprop : val.second) {
            addPropertyToMap (dprop, model->getPropertyMap ());
            // model->getPropertyMap().add(dprop._name, dprop._desc, dprop._val);
        }
    }

    // add all configurations, add home configs to default state
    for (QConfig& config : dev._qconfig) {
        if (config.name == "Home") {
            rw::math::Q q (config.q.size ());
            for (size_t i = 0; i < q.size (); i++)
                q[i] = config.q[i];
            DeviceInitState* initDevState = new DeviceInitState (q, model);
            setup.actions.push_back (initDevState);
        }
        model->getBase ()->getPropertyMap ().add< Q > (
            config.name, "", Q (config.q.size (), &config.q[0]));
        model->getPropertyMap ().add< Q > (config.name, "", Q (config.q.size (), &config.q[0]));
    }

    return model;
}

CollisionSetup defaultCollisionSetup (const WorkCell& workcell)
{
    // We build a list of frames
    std::list< Frame::Ptr > frameList;
    std::stack< Frame* > frameStack;
    frameStack.push (workcell.getWorldFrame ());
    while (0 != frameStack.size ()) {
        rw::core::Ptr<Frame> frame = frameStack.top ();
        frameStack.pop ();

        for (Frame::iterator it = frame->getChildren ().first; it != frame->getChildren ().second;
             ++it) {
            frameStack.push (&*it);
            frameList.push_back (&*it);
        }
    }

    // Add frames to exclude list
    std::vector< std::pair< std::string, std::string > > excludeList;
    std::list< Frame::Ptr >::reverse_iterator rit;
    std::list< Frame::Ptr >::iterator it;
    for (rit = frameList.rbegin (); rit != frameList.rend (); rit++) {
        if ((*rit)->isDAF () || rit->cast< MovableFrame > ()) {
            continue;
        }
        for (it = frameList.begin (); (*it) != (*rit); it++) {
            // Do not check a child against a parent geometry
            rw::core::Ptr<Frame> parent1 = (*it)->getParent ();     // Link N
            rw::core::Ptr<Frame> parent2 = (*rit)->getParent ();    // Link N+1

            if ((*it)->isDAF () || it->cast< MovableFrame > ()) {
                continue;
            }

            if (parent1 && parent2 && parent2->getParent () != NULL) {
                if (parent2->getParent () == parent1) {
                    excludeList.push_back (std::make_pair ((*rit)->getName (), (*it)->getName ()));
                }
            }

            // Do not check a child agains its parent
            if ((*it)->getParent () == (*rit) || (*rit)->getParent () == (*it)) {
                excludeList.push_back (std::make_pair ((*rit)->getName (), (*it)->getName ()));
            }
        }
    }
    return CollisionSetup (excludeList);
}
}    // namespace

rw::models::WorkCell::Ptr XMLRWLoader::loadWorkCell (const std::string& fname)
{
    try {
        std::string filename = IOUtil::getAbsoluteFileName (fname);

        RW_DEBUGS (" ******* Loading workcell from \"" << filename << "\" ");

        // container for actions to execute when all frames and devices has been loaded
        DummySetup setup;
        setup.scene = ownedPtr (new SceneDescriptor ());

        // To be used for potential serialization
        setup.wcFilename = filename;

        // Start parsing workcell
        // std::shared_ptr<DummyWorkcell> workcell = XMLRWParser::parseWorkcell(filename);
        setup.dwc = XMLRWParser::parseWorkcell (filename);

        // do sanity check on the workcell,
        // 1. check that all parent frames are valid frames
        std::map< std::string, DummyFrame* > strToFrame;
        for (DummyFrame& df : setup.dwc->_framelist) {
            strToFrame[df.getName ()] = &df;
        }
        for (DummyDevice& dd : setup.dwc->_devlist) {
            for (DummyFrame& df : dd._frames) {
                strToFrame[df.getName ()] = &df;
            }
        }

        for (DummyFrame& df : setup.dwc->_framelist) {
            if (strToFrame.find (df.getRefFrame ()) == strToFrame.end ()) {
                if (df.getRefFrame () != "WORLD")
                    RW_THROW ("Frame " << df.getName () << " refers to a frame \""
                                       << df.getRefFrame () << "\" which has not been declared!");
            }
        }

        // Now build a workcell from the parsed results
        setup.tree                              = new StateStructure ();
        setup.world                             = setup.tree->getRoot ();
        setup.frameMap[setup.world->getName ()] = setup.world.get();

        auto tmp = setup.tree->getRoot ()
                       ->getPropertyMap ()
                       .get< ProximitySetup > ("ProximitySetup", ProximitySetup ())
                       .getProximitySetupRules ();

        // Create WorkCell
        WorkCell::Ptr wc = ownedPtr (new WorkCell (ownedPtr (setup.tree), setup.dwc->_name, fname));
        wc->setSceneDescriptor (setup.scene);

        // if(setup.scene)
        //    setup.scene->setWorkCell(wc);

        // first create all frames defined in the workcell
        for (size_t i = 0; i < setup.dwc->_framelist.size (); i++) {
            createFrame (setup.dwc->_framelist[i], setup);
        }

        // next add the frames to the StateStructure, starting with world
        addToStateStructure (setup.world, setup);

        // and lastly all properties can be added
        for (size_t i = 0; i < setup.dwc->_framelist.size (); i++) {
            addFrameProps (setup.dwc->_framelist[i], setup);
        }

        // Now create all devices
        for (size_t i = 0; i < setup.dwc->_devlist.size (); i++) {
            createDevice (setup.dwc->_devlist[i], setup);
        }

        // remember to add all models defined in the workcell to the frames
        for (size_t i = 0; i < setup.dwc->_models.size (); i++) {
            std::map< std::string, Frame* >::iterator parent =
                setup.frameMap.find (setup.dwc->_models[i]._refframe);
            if (parent == setup.frameMap.end ()) {
                RW_THROW ("Model \""
                          << setup.dwc->_models[i]._name
                          << "\" "
                             "will not be loaded since it refers to an non existing frame!!");
            }
            addModelToFrame (setup.dwc->_models[i], (*parent).second, setup.tree, setup);
        }
        State defaultState = setup.tree->getDefaultState ();

        // now add any daf frames to their respectfull parent frames
        for (size_t i = 0; i < setup.dwc->_framelist.size (); i++) {
            DummyFrame& dframe = setup.dwc->_framelist[i];
            if (!dframe._isDaf)
                continue;
            std::map< std::string, Frame* >::iterator parent =
                setup.frameMap.find (dframe.getRefFrame ());
            if (parent == setup.frameMap.end ()) {
                RW_THROW ("Frame \""
                          << dframe.getName () << "\" "
                          << "will not be loaded since it refers to an non existing frame!!"
                          << " refframe: \"" << dframe.getRefFrame () << "\"");
            }
            rw::core::Ptr<Frame> frame = setup.frameMap[dframe.getName ()];
            frame->attachTo ((*parent).second, defaultState);
        }

        // add collision models from workcell
        for (size_t i = 0; i < setup.dwc->_colmodels.size (); i++) {
            setup.colsetups.push_back (setup.dwc->_colmodels[i]);
        }

        // add collision models from workcell
        for (size_t i = 0; i < setup.dwc->_proxmodels.size (); i++) {
            setup.proxsetups.push_back (setup.dwc->_proxmodels[i]);
        }

        if (setup.dwc->_calibration.size () > 0) {
            wc->setCalibrationFilename (setup.dwc->_calibration.front ()._filename);
        }

        // now initialize state with initial actions
        std::vector< InitialAction* >::iterator action = setup.actions.begin ();
        for (; action != setup.actions.end (); ++action) {
            (*action)->setInitialState (defaultState);
            delete (*action);
        }

        setup.tree->setDefaultState (defaultState);

        // add devices to workcell
        {
            std::map< std::string, Device::Ptr >::iterator first = setup.devMap.begin ();
            for (; first != setup.devMap.end (); ++first) {
                wc->addDevice ((*first).second);
            }
        }

        // add all objects to scene
        {
            std::map< Frame*, RigidObject::Ptr >::iterator first = setup.objectMap.begin ();
            for (; first != setup.objectMap.end (); ++first) {
                wc->add ((*first).second);
            }
        }

        // add collision setup from files
        CollisionSetup collisionSetup;

        ColSetupList::iterator colsetupIter = setup.colsetups.begin ();
        for (; colsetupIter != setup.colsetups.end (); ++colsetupIter) {
            std::string prefix   = createScopedName ("", (*colsetupIter)._scope);
            std::string filename = StringUtil::getDirectoryName ((*colsetupIter)._pos.file);
            filename += "/" + (*colsetupIter)._filename;
            CollisionSetup s = CollisionSetupLoader::load (prefix, filename);
            collisionSetup.merge (s);
        }

        // in case no collisionsetup info or proximitysetup info is supplied
        if (setup.colsetups.size () == 0 && setup.proxsetups.size () == 0) {
            collisionSetup = defaultCollisionSetup (*wc);
        }

        CollisionSetup::set (collisionSetup, wc);

        // add proximity setup from files
        ProximitySetup proximitySetup;

        // Merge in old collision setups
        proximitySetup.merge (ProximitySetup (collisionSetup), "");

        std::string proxSetupFilepath;
        std::string proxSetupFilename;
        std::string proxSetupFilepathWithName;

        ProxSetupList::iterator proxsetupIter = setup.proxsetups.begin ();
        for (; proxsetupIter != setup.proxsetups.end (); ++proxsetupIter) {
            std::string prefix   = createScopedName ("", (*proxsetupIter)._scope);
            std::string filename = StringUtil::getDirectoryName ((*proxsetupIter)._pos.file);
            filename += "/" + (*proxsetupIter)._filename;

            ProximitySetup s = DOMProximitySetupLoader::load (filename);
            proximitySetup.merge (s, prefix);
        }

        if (!setup.proxsetups.empty ()) {
            auto it           = std::prev (setup.proxsetups.end ());
            proxSetupFilepath = (*it)._pos.file;
            proxSetupFilename = (*it)._filename;
            proxSetupFilepathWithName =
                StringUtil::getDirectoryName ((*it)._pos.file) + proxSetupFilename;
        }

        std::ostringstream relProxSetupFilePath;
        relProxSetupFilePath << StringUtil::getRelativeDirectoryName (
            proxSetupFilepath, StringUtil::getDirectoryName (setup.wcFilename));

        relProxSetupFilePath << proxSetupFilename;

        ProximitySetup::set (proximitySetup, wc);

        for (DummyProperty dprop : setup.dwc->_properties) {
            // wc->getPropertyMap().add(dprop._name, dprop._desc, dprop._val);
            addPropertyToMap (dprop, wc->getPropertyMap ());
        }

        // make sure to add the name of the workcell file to the workcell propertymap
        wc->getPropertyMap ().set< std::string > ("WorkCellFileName", filename);

        // save
        wc->getPropertyMap ().addForce (
            "ProximitySetupFilePath", "Main proximity filepath", proxSetupFilepathWithName);
        wc->getPropertyMap ().addForce ("ProximitySetupRelFilePath",
                                        "Main proximity rel. filepath",
                                        relProxSetupFilePath.str ());

        return wc;
    }
    catch (const std::exception& e) {
        RW_THROW ("Could not load WorkCell: " << fname << ". An error occoured:\n "
                                              << std::string (e.what ()));
    }
    return NULL;
}

rw::models::WorkCell::Ptr XMLRWLoader::load (const std::string& filename)
{
    XMLRWLoader loader;
    return loader.loadWorkCell (filename);
}

std::string XMLRWLoader::getWorkCellFileNameId ()
{
    static const std::string id_wc = "WorkCellFileName";
    return id_wc;
}
