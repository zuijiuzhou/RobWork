/********************************************************************************
 * Copyright 2017 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#include "DOMWorkCellSaver.hpp"

#include <rw/core/DOMElem.hpp>
#include <rw/core/DOMParser.hpp>
#include <rw/core/PropertyBase.hpp>
#include <rw/geometry/Box.hpp>
#include <rw/geometry/Cone.hpp>
#include <rw/geometry/Cylinder.hpp>
#include <rw/geometry/Plane.hpp>
#include <rw/geometry/Sphere.hpp>
#include <rw/geometry/TriMesh.hpp>
#include <rw/geometry/Tube.hpp>
#include <rw/graphics/DrawableNode.hpp>
#include <rw/graphics/SceneDescriptor.hpp>
#include <rw/graphics/WorkCellScene.hpp>
#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/Frame.hpp>
#include <rw/kinematics/Kinematics.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/loaders/dom/DOMBasisTypes.hpp>
#include <rw/loaders/dom/DOMPropertyMapFormat.hpp>
#include <rw/loaders/dom/DOMPropertyMapSaver.hpp>
#include <rw/loaders/dom/DOMProximitySetupSaver.hpp>
#include <rw/loaders/model3d/STLFile.hpp>
#include <rw/loaders/rwxml/XMLParserUtil.hpp>
#include <rw/models/DHParameterSet.hpp>
#include <rw/models/DependentPrismaticJoint.hpp>
#include <rw/models/DependentRevoluteJoint.hpp>
#include <rw/models/MobileDevice.hpp>
#include <rw/models/Object.hpp>
#include <rw/models/ParallelDevice.hpp>
#include <rw/models/PrismaticJoint.hpp>
#include <rw/models/SerialDevice.hpp>
#include <rw/models/TreeDevice.hpp>
#include <rw/models/WorkCell.hpp>
#include <rw/proximity/ProximitySetup.hpp>

using namespace rw::core;
using namespace rw::loaders;
using namespace rw::math;
using namespace rw::kinematics;
using namespace rw::graphics;
using namespace rw::models;
using namespace rw::geometry;
using namespace rw;

namespace {

class TriMeshList : public TriMesh
{
  public:
    TriMeshList (std::vector< TriMesh::Ptr > list) : _list (list) {}

    virtual rw::geometry::Triangle< double > getTriangle (size_t idx) const
    {
        size_t i = 0;
        while (idx >= _list[i]->size ()) {
            idx -= _list[i++]->size ();
        }
        return _list[i]->getTriangle (idx);
    }
    virtual void getTriangle (size_t idx, rw::geometry::Triangle< double >& dst) const
    {
        size_t i = 0;
        while (idx >= _list[i]->size ()) {
            idx -= _list[i++]->size ();
        }
        return _list[i]->getTriangle (idx, dst);
    }
    virtual void getTriangle (size_t idx, rw::geometry::Triangle< float >& dst) const
    {
        size_t i = 0;
        while (idx >= _list[i]->size ()) {
            idx -= _list[i++]->size ();
        }
        return _list[i]->getTriangle (idx, dst);
    }

    virtual size_t getSize () const
    {
        size_t res = 0;
        for (auto & t : _list) {
            res += t->size ();
        }
        return res;
    }

    virtual size_t size () const { return getSize (); }

    virtual rw::core::Ptr< TriMesh > clone () const
    {
        return rw::core::ownedPtr (new TriMeshList (_list));
    }

    virtual void scale (double scale) {}

    virtual GeometryType getType () const{
        return GeometryType::UserType;
    }

  private:
    std::vector< TriMesh::Ptr > _list;
};

class ElementCreator
{
  public:
    ElementCreator (DOMElem::Ptr root) : _root (root) {}

    template< class T >
    DOMElem::Ptr createElement (T object, rw::core::Ptr< const rw::models::WorkCell > workcell,
                                DOMElem::Ptr parent);

    template< class T >
    DOMElem::Ptr createElement (T object, rw::core::Ptr< const rw::models::WorkCell > workcell,
                                Device::Ptr dev, DOMElem::Ptr parent);

    template< class T >
    DOMElem::Ptr createElement (T object, rw::core::Ptr< const rw::models::WorkCell > workcell,
                                const State state, DOMElem::Ptr parent);

    template< class T >
    DOMElem::Ptr createElement (T object, rw::core::Ptr< const rw::models::WorkCell > workcell,
                                const State state, Device::Ptr dev, DOMElem::Ptr parent);

  private:
    DOMElem::Ptr _root;
};

std::map< rw::kinematics::Frame*, rw::models::Device* > frameToDevice;
std::map< rw::kinematics::Frame*, rw::models::Device* > parentToDevice;
std::map< rw::kinematics::Frame*, bool > isEndEffector;

bool isFrameInDevice (rw::core::Ptr< Frame > frame)
{
    return frameToDevice.find (frame.get ()) != frameToDevice.end ();
}

bool isFrameInDevice (rw::core::Ptr< Frame > frame, Device* dev)
{
    if (isFrameInDevice (frame))
        return frameToDevice[frame.get ()] == dev;
    return false;
}

bool isFrameParentToDevice (rw::core::Ptr< Frame > frame)
{
    return parentToDevice.find (frame.get ()) != parentToDevice.end ();
}

template< class T > std::string createStringFromArray (const T& v, size_t n)
{
    std::ostringstream str;
    str.unsetf (std::ios::floatfield);    // floatfield not set
    str.precision (16);
    for (size_t i = 0; i < n; ++i) {
        str << v (i);
        if (i != n - 1)
            str << " ";
    }
    return str.str ();
}

template< class T > std::string createStringFromArray (const T& v)
{
    return createStringFromArray< T > (v, v.size ());
}

std::string scopedName (Device* dev, rw::core::Ptr< Frame > frame)
{
    std::string fname = frame->getName ();
    std::string dname = dev->getName ();
    if (fname.length () > dname.length ()) {
        if (dname == fname.substr (0, dname.length ())) {
            return fname.substr (dname.length () + 1, fname.length () - (dname.length () + 1));
        }
    }
    return fname;
}

std::string scopedName (rw::core::Ptr< Frame > sframe, rw::core::Ptr< Frame > frame)
{
    if (isFrameInDevice (frame)) {
        if (isFrameInDevice (sframe, frameToDevice[frame.get ()]))
            return scopedName (frameToDevice[frame.get ()], frame);
    }
    return frame->getName ();
}

std::string getDeviceType (Device& dev)
{
    if (dynamic_cast< SerialDevice* > (&dev)) {
        return std::string ("SerialDevice");
    }
    else if (dynamic_cast< TreeDevice* > (&dev)) {
        return std::string ("TreeDevice");
    }
    else if (dynamic_cast< ParallelDevice* > (&dev)) {
        return std::string ("ParallelDevice");
    }
    else if (dynamic_cast< MobileDevice* > (&dev)) {
        return std::string ("MobileDevice");
    }
    else {
        RW_THROW ("unknown device type!");
    }
    return "";
}

void writeDrawablesAndColModels (rw::models::Object::Ptr object, DOMElem::Ptr parent)
{
    if (object != nullptr) {
        // std::cout << "Object name: " << object->getName() << std::endl;

        if (object->getGeometry ().empty () && !object->getModels ().empty ()) {
            // std::cout << "object is only a drawable" << std::endl;
            for (const auto& mod : object->getModels ()) {
                DOMElem::Ptr draw_element = parent->addChild ("Drawable");
                // std::cout << "Object model name: " << mod->getName() << std::endl;
                draw_element->addAttribute ("name")->setValue (mod->getName ());
                draw_element->addAttribute ("refframe")->setValue (object->getName ());

                // Add the rotation in RPY
                RPY<> rpy (mod->getTransform ().R ());
                rpy (0) = rpy (0) * Rad2Deg;
                rpy (1) = rpy (1) * Rad2Deg;
                rpy (2) = rpy (2) * Rad2Deg;
                DOMBasisTypes::createRPY (rpy, draw_element);

                // Add the position
                DOMBasisTypes::createPos (mod->getTransform ().P (), draw_element);
        
                std::string t ("#");
                if (!mod->getFilePath ().empty () &&
                    mod->getFilePath ().compare (0, t.length (), t) != 0) {

                    DOMElem::Ptr polytope_element = draw_element->addChild ("Polytope");
                   
                    polytope_element->addAttribute ("file")->setValue (mod->getFilePath ());
                }
                else {
                    // Resolve type of geometry

                    // First decode the filePath string, which in this case contains info for
                    // creating geometry
                    std::string type, param1, param2, param3, param4, param5;
                    std::istringstream ss (mod->getFilePath ());
                    ss >> type >> param1 >> param2 >> param3 >> param4 >> param5;

                    if (type == "#Plane") {
                        DOMElem::Ptr plane_element = draw_element->addChild ("Plane");
                    }
                    else if (type == "#Box") {
                        DOMElem::Ptr box_element = draw_element->addChild ("Box");
                        box_element->addAttribute ("x")->setValue (param1);
                        box_element->addAttribute ("y")->setValue (param2);
                        box_element->addAttribute ("z")->setValue (param3);
                    }
                    else if (type == "#Sphere") {
                        DOMElem::Ptr sphere_element = draw_element->addChild ("Sphere");
                        sphere_element->addAttribute ("radius")->setValue (param1);
                    }
                    else if (type == "#Cone") {
                        DOMElem::Ptr cone_element = draw_element->addChild ("Cone");
                        cone_element->addAttribute ("radius")->setValue (param1);
                        cone_element->addAttribute ("z")->setValue (param2);
                    }
                    else if (type == "#Cylinder") {
                        DOMElem::Ptr cyl_element = draw_element->addChild ("Cylinder");
                        cyl_element->addAttribute ("radius")->setValue (param1);
                        cyl_element->addAttribute ("z")->setValue (param2);
                    }
                    else if (type == "#Tube") {
                        DOMElem::Ptr tube_element = draw_element->addChild ("Tube");
                        tube_element->addAttribute ("radius")->setValue (param1);
                        tube_element->addAttribute ("x")->setValue (param2);
                        tube_element->addAttribute ("z")->setValue (param3);
                    }
                    else {
                        // Save object as STL
                        std::vector<TriMesh::Ptr> list;
                        for(auto &o: mod->getObjects ()){
                            list.push_back(o.cast<TriMesh>());
                        }
                        rw::loaders::STLFile::save (TriMeshList (list),
                                              "./model_" + mod->getName () + ".stl");

                        DOMElem::Ptr polytope_element = draw_element->addChild ("Polytope");
                        polytope_element->addAttribute ("file")->setValue (
                            "./model_" + mod->getName () + ".stl");
                    }
                }
            }
        }
        else if (!object->getGeometry ().empty () && object->getModels ().empty ()) {
            // std::cout << "object is only a collision geometry" << std::endl;
            for (auto geom : object->getGeometry ()) {
                DOMElem::Ptr mod_element = parent->addChild ("CollisionModel");
                // std::cout << "Object geometry name: " << geom->getName() << std::endl;
                mod_element->addAttribute ("name")->setValue (geom->getName ());
                mod_element->addAttribute ("refframe")->setValue (object->getName ());

                // Add the rotation in RPY
                RPY<> rpy (geom->getTransform ().R ());
                rpy (0) = rpy (0) * Rad2Deg;
                rpy (1) = rpy (1) * Rad2Deg;
                rpy (2) = rpy (2) * Rad2Deg;
                DOMBasisTypes::createRPY (rpy, mod_element);

                // Add the position
                DOMBasisTypes::createPos (geom->getTransform ().P (), mod_element);

                if (!geom->getFilePath ().empty ()) {
                    DOMElem::Ptr polytope_element = mod_element->addChild ("Polytope");
                    // std::cout << "Object geometry filepath: " << geom->getFilePath() <<
                    // std::endl;
                    polytope_element->addAttribute ("file")->setValue (geom->getFilePath ());
                }
                else {
                    // Resolve type of geometry
                    if (dynamic_cast< rw::geometry::Plane* > (geom->getGeometryData ().get ())) {
                        DOMElem::Ptr plane_element = mod_element->addChild ("Plane");
                    }
                    else if (rw::geometry::Box* const box = dynamic_cast< rw::geometry::Box* > (
                                 geom->getGeometryData ().get ())) {
                        DOMElem::Ptr box_element = mod_element->addChild ("Box");
                        box_element->addAttribute ("x")->setValue (box->getParameters ()[0]);
                        box_element->addAttribute ("y")->setValue (box->getParameters ()[1]);
                        box_element->addAttribute ("z")->setValue (box->getParameters ()[2]);
                    }
                    else if (rw::geometry::Sphere* sphere = dynamic_cast< rw::geometry::Sphere* > (
                                 geom->getGeometryData ().get ())) {
                        DOMElem::Ptr sphere_element = mod_element->addChild ("Sphere");
                        sphere_element->addAttribute ("radius")->setValue (sphere->getRadius ());
                    }
                    else if (rw::geometry::Cone* cone = dynamic_cast< rw::geometry::Cone* > (
                                 geom->getGeometryData ().get ())) {
                        DOMElem::Ptr cone_element = mod_element->addChild ("Cone");
                        cone_element->addAttribute ("radius")->setValue (cone->getBottomRadius ());
                        cone_element->addAttribute ("z")->setValue (cone->getHeight ());
                    }
                    else if (rw::geometry::Cylinder* cyl = dynamic_cast< rw::geometry::Cylinder* > (
                                 geom->getGeometryData ().get ())) {
                        DOMElem::Ptr cyl_element = mod_element->addChild ("Cylinder");
                        cyl_element->addAttribute ("radius")->setValue (cyl->getRadius ());
                        cyl_element->addAttribute ("z")->setValue (cyl->getHeight ());
                    }
                    else if (rw::geometry::Tube* tube = dynamic_cast< rw::geometry::Tube* > (
                                 geom->getGeometryData ().get ())) {
                        DOMElem::Ptr tube_element = mod_element->addChild ("Tube");
                        tube_element->addAttribute ("radius")->setValue (tube->getInnerRadius ());
                        tube_element->addAttribute ("x")->setValue (tube->getThickness ());
                        tube_element->addAttribute ("z")->setValue (tube->getHeight ());
                    }
                    else {
                        // std::cout << "Unknown geometry type" << std::endl;
                    }
                }
            }
        }
        else if (!object->getGeometry ().empty () && !object->getModels ().empty ()) {
            // std::cout << "object both collision geometry and drawable vis model" << std::endl;
            for (const auto& mod : object->getModels ()) {
                DOMElem::Ptr draw_element = parent->addChild ("Drawable");
                // std::cout << "Object model name: " << mod->getName() << std::endl;
                draw_element->addAttribute ("name")->setValue (mod->getName ());
                draw_element->addAttribute ("refframe")->setValue (object->getName ());

                // Add the rotation in RPY
                RPY<> rpy (mod->getTransform ().R ());
                rpy (0) = rpy (0) * Rad2Deg;
                rpy (1) = rpy (1) * Rad2Deg;
                rpy (2) = rpy (2) * Rad2Deg;
                DOMBasisTypes::createRPY (rpy, draw_element);

                // Add the position
                DOMBasisTypes::createPos (mod->getTransform ().P (), draw_element);

                std::string t ("#");
                if (!mod->getFilePath ().empty () &&
                    mod->getFilePath ().compare (0, t.length (), t) != 0) {
                    DOMElem::Ptr polytope_element = draw_element->addChild ("Polytope");
                    // std::cout << "Object model filepath: " << mod->getFilePath() << std::endl;
                    polytope_element->addAttribute ("file")->setValue (mod->getFilePath ());
                }
                else {
                    // Resolve type of geometry

                    // First decode the filePath string, which in this case contains info for
                    // creating geometry
                    std::string type, param1, param2, param3, param4, param5;
                    std::istringstream ss (mod->getFilePath ());
                    ss >> type >> param1 >> param2 >> param3 >> param4 >> param5;
                    /*std::cout << type << std::endl << param1 << std::endl << param2 << std::endl
                       << param3
                              << std::endl <<
                              param4 << std::endl << param5 << std::endl;*/

                    if (type == "#Plane") {
                        DOMElem::Ptr plane_element = draw_element->addChild ("Plane");
                    }
                    else if (type == "#Box") {
                        DOMElem::Ptr box_element = draw_element->addChild ("Box");
                        box_element->addAttribute ("x")->setValue (param1);
                        box_element->addAttribute ("y")->setValue (param2);
                        box_element->addAttribute ("z")->setValue (param3);
                    }
                    else if (type == "#Sphere") {
                        DOMElem::Ptr sphere_element = draw_element->addChild ("Sphere");
                        sphere_element->addAttribute ("radius")->setValue (param1);
                    }
                    else if (type == "#Cone") {
                        DOMElem::Ptr cone_element = draw_element->addChild ("Cone");
                        cone_element->addAttribute ("radius")->setValue (param1);
                        cone_element->addAttribute ("z")->setValue (param2);
                    }
                    else if (type == "#Cylinder") {
                        DOMElem::Ptr cyl_element = draw_element->addChild ("Cylinder");
                        cyl_element->addAttribute ("radius")->setValue (param1);
                        cyl_element->addAttribute ("z")->setValue (param2);
                    }
                    else if (type == "#Tube") {
                        DOMElem::Ptr tube_element = draw_element->addChild ("Tube");
                        tube_element->addAttribute ("radius")->setValue (param1);
                        tube_element->addAttribute ("x")->setValue (param2);
                        tube_element->addAttribute ("z")->setValue (param3);
                    }
                    else {
                        // std::cout << "Unknown geometry type" << std::endl;
                    }
                }

                for (auto geom : object->getGeometry ()) {
                    DOMElem::Ptr var_element;

                    // This is quite the hack, if drawable and collision model has the same name
                    // only specify drawable.
                    if (geom->getName () != mod->getName ()) {
                        DOMElem::Ptr mod_element = parent->addChild ("CollisionModel");
                        // std::cout << "Object geometry name: " << geom->getName() << std::endl;
                        mod_element->addAttribute ("name")->setValue (geom->getName ());
                        mod_element->addAttribute ("refframe")->setValue (object->getName ());

                        // Add the rotation in RPY
                        RPY<> rpy (geom->getTransform ().R ());
                        rpy (0) = rpy (0) * Rad2Deg;
                        rpy (1) = rpy (1) * Rad2Deg;
                        rpy (2) = rpy (2) * Rad2Deg;
                        DOMBasisTypes::createRPY (rpy, mod_element);

                        // Add the position
                        DOMBasisTypes::createPos (geom->getTransform ().P (), mod_element);

                        var_element = mod_element;
                    }
                    else {
                        var_element = draw_element;
                    }

                    if (!geom->getFilePath ().empty () && mod->getFilePath ().empty ()) {
                        DOMElem::Ptr polytope_element = var_element->addChild ("Polytope");
                        // std::cout << "Object geometry filepath: " << geom->getFilePath() <<
                        // std::endl;
                        polytope_element->addAttribute ("file")->setValue (geom->getFilePath ());
                    }
                    else {
                        // Resolve type of geometry
                        if (dynamic_cast< rw::geometry::Plane* > (
                                geom->getGeometryData ().get ())) {
                            DOMElem::Ptr plane_element = var_element->addChild ("Plane");
                        }
                        else if (rw::geometry::Box* const box = dynamic_cast< rw::geometry::Box* > (
                                     geom->getGeometryData ().get ())) {
                            DOMElem::Ptr box_element = var_element->addChild ("Box");
                            box_element->addAttribute ("x")->setValue (box->getParameters ()[0]);
                            box_element->addAttribute ("y")->setValue (box->getParameters ()[1]);
                            box_element->addAttribute ("z")->setValue (box->getParameters ()[2]);
                        }
                        else if (rw::geometry::Sphere* sphere =
                                     dynamic_cast< rw::geometry::Sphere* > (
                                         geom->getGeometryData ().get ())) {
                            DOMElem::Ptr sphere_element = var_element->addChild ("Sphere");
                            sphere_element->addAttribute ("radius")->setValue (
                                sphere->getRadius ());
                        }
                        else if (rw::geometry::Cone* cone = dynamic_cast< rw::geometry::Cone* > (
                                     geom->getGeometryData ().get ())) {
                            DOMElem::Ptr cone_element = var_element->addChild ("Cone");
                            cone_element->addAttribute ("radius")->setValue (
                                cone->getBottomRadius ());
                            cone_element->addAttribute ("z")->setValue (cone->getHeight ());
                        }
                        else if (rw::geometry::Cylinder* cyl =
                                     dynamic_cast< rw::geometry::Cylinder* > (
                                         geom->getGeometryData ().get ())) {
                            DOMElem::Ptr cyl_element = var_element->addChild ("Cylinder");
                            cyl_element->addAttribute ("radius")->setValue (cyl->getRadius ());
                            cyl_element->addAttribute ("z")->setValue (cyl->getHeight ());
                        }
                        else if (rw::geometry::Tube* tube = dynamic_cast< rw::geometry::Tube* > (
                                     geom->getGeometryData ().get ())) {
                            DOMElem::Ptr tube_element = var_element->addChild ("Tube");
                            tube_element->addAttribute ("radius")->setValue (
                                tube->getInnerRadius ());
                            tube_element->addAttribute ("x")->setValue (tube->getThickness ());
                            tube_element->addAttribute ("z")->setValue (tube->getHeight ());
                        }
                        else {
                            // std::cout << "Unknown geometry type" << std::endl;
                        }
                    }
                }
            }

            /*for (auto geom : object->getGeometry()) {
                DOMElem::Ptr draw_element = parent->addChild("Drawable");
                std::cout << "Object model name: " << geom->getName() << std::endl;
                draw_element->addAttribute("name")->setValue(geom->getName());
                draw_element->addAttribute("refframe")->setValue(object->getName());
                draw_element->addAttribute("colmodel")->setValue("Disabled");

                // Add the rotation in RPY
                RPY<> rpy(geom->getTransform().R());
                rpy(0) = rpy(0) * Rad2Deg;
                rpy(1) = rpy(1) * Rad2Deg;
                rpy(2) = rpy(2) * Rad2Deg;
                DOMBasisTypes::createRPY(rpy, draw_element);

                // Add the position
                DOMBasisTypes::createPos(geom->getTransform().P(), draw_element);

                if (!geom->getFilePath().empty()) {
                    DOMElem::Ptr polytope_element = draw_element->addChild("Polytope");
                    std::cout << "Object geometry filepath: " << geom->getFilePath() << std::endl;
                    polytope_element->addAttribute("file")->setValue(geom->getFilePath());
                } else {
                    //Resolve type of geometry
                    if (rw::geometry::Plane *const plane = dynamic_cast<rw::geometry::Plane
            *>(geom->getGeometryData().get())) { DOMElem::Ptr plane_element =
            draw_element->addChild("Plane"); } else if (rw::geometry::Box *const box =
            dynamic_cast<rw::geometry::Box *>(geom->getGeometryData().get())) { DOMElem::Ptr
            box_element = draw_element->addChild("Box");
                        box_element->addAttribute("x")->setValue(box->getParameters()[0]);
                        box_element->addAttribute("y")->setValue(box->getParameters()[1]);
                        box_element->addAttribute("z")->setValue(box->getParameters()[2]);
                    } else if (rw::geometry::Sphere *sphere = dynamic_cast<rw::geometry::Sphere
            *>(geom->getGeometryData().get())) { DOMElem::Ptr sphere_element =
            draw_element->addChild("Sphere");
                        sphere_element->addAttribute("radius")->setValue(sphere->getRadius());
                    } else if (rw::geometry::Cone *cone = dynamic_cast<rw::geometry::Cone
            *>(geom->getGeometryData().get())) { DOMElem::Ptr cone_element =
            draw_element->addChild("Cone");
                        cone_element->addAttribute("radius")->setValue(cone->getBottomRadius());
                        cone_element->addAttribute("z")->setValue(cone->getHeight());
                    } else if (rw::geometry::Cylinder *cyl = dynamic_cast<rw::geometry::Cylinder
            *>(geom->getGeometryData().get())) { DOMElem::Ptr cyl_element =
            draw_element->addChild("Cylinder");
                        cyl_element->addAttribute("radius")->setValue(cyl->getRadius());
                        cyl_element->addAttribute("z")->setValue(cyl->getHeight());
                    } else if (rw::geometry::Tube *tube = dynamic_cast<rw::geometry::Tube
            *>(geom->getGeometryData().get())) { DOMElem::Ptr tube_element =
            draw_element->addChild("Tube");
                        tube_element->addAttribute("radius")->setValue(tube->getInnerRadius());
                        tube_element->addAttribute("x")->setValue(tube->getThickness());
                        tube_element->addAttribute("z")->setValue(tube->getHeight());
                    } else {
                        std::cout << "Unknown geometry type" << std::endl;
                    }
                }
            }*/
        }
    }
}

template<>
DOMElem::Ptr ElementCreator::createElement< RevoluteJoint* > (
    RevoluteJoint* frame, rw::core::Ptr< const rw::models::WorkCell > workcell, const State state,
    DOMElem::Ptr parent)
{
    return parent;
}

template<>
DOMElem::Ptr ElementCreator::createElement< RevoluteJoint* > (
    RevoluteJoint* frame, rw::core::Ptr< const rw::models::WorkCell > workcell, const State state,
    Device::Ptr dev, DOMElem::Ptr parent)
{
    // Check if this is a DHJoint
    const DHParameterSet* dh_param = rw::models::DHParameterSet::get (frame);
    if (dh_param != nullptr) {
        // std::cout << "The Joint is a DHJoint" << std::endl;
        DOMElem::Ptr element = parent->addChild ("DHJoint");
        element->addAttribute ("name")->setValue (scopedName (frameToDevice[frame], frame));
        if (dh_param->getType () == "HGP" && dh_param->isParallel ()) {
            element->addAttribute ("alpha")->setValue (dh_param->alpha () * Rad2Deg);
            element->addAttribute ("a")->setValue (dh_param->a ());
            element->addAttribute ("offset")->setValue (dh_param->theta () * Rad2Deg);
            element->addAttribute ("b")->setValue (dh_param->b () * Rad2Deg);
            const std::string type = "HGP";
            element->addAttribute ("type")->setValue (type);
        }
        else {
            element->addAttribute ("alpha")->setValue (dh_param->alpha () * Rad2Deg);
            element->addAttribute ("a")->setValue (dh_param->a ());
            element->addAttribute ("d")->setValue (dh_param->d ());
            element->addAttribute ("offset")->setValue (dh_param->theta () * Rad2Deg);
            const std::string type = "schilling";
            element->addAttribute ("type")->setValue (type);
        }

        Q pmin = frame->getBounds ().first;
        Q pmax = frame->getBounds ().second;
        Q vel  = frame->getMaxVelocity ();
        Q acc  = frame->getMaxAcceleration ();

        DOMElem::Ptr pos_limit_element = element->addChild ("PosLimit");
        pos_limit_element->addAttribute ("min")->setValue (pmin (0) * Rad2Deg);
        pos_limit_element->addAttribute ("max")->setValue (pmax (0) * Rad2Deg);
        DOMElem::Ptr vel_limit_element = element->addChild ("VelLimit");
        vel_limit_element->addAttribute ("max")->setValue (vel (0) * Rad2Deg);
        DOMElem::Ptr acc_limit_element = element->addChild ("AccLimit");
        acc_limit_element->addAttribute ("max")->setValue (acc (0) * Rad2Deg);

        // Get any drawables or collision models associated with this frame
        rw::models::Object::Ptr object = workcell->findObject (frame->getName ());

        if (object != nullptr) {
            // std::cout << "Object name: " << object->getName() << std::endl;

            if (object->getGeometry ().empty () && !object->getModels ().empty ()) {
                // std::cout << "object is only a drawable" << std::endl;
                for (const auto& mod : object->getModels ()) {
                    DOMElem::Ptr draw_element = element->addChild ("Drawable");
                    // std::cout << "Object model name: " << mod->getName() << std::endl;
                    draw_element->addAttribute ("name")->setValue (mod->getName ());

                    std::string fname = mod->getName ();
                    std::string dname = dev->getName ();
                    // std::cout << dname << "-->" << fname << std::endl;
                    if (fname.length () > dname.length ()) {
                        if (dname == fname.substr (0, dname.length ())) {
                            draw_element->addAttribute ("refframe")
                                ->setValue (fname.substr (dname.length () + 1,
                                                          fname.length () - (dname.length () + 1)));
                        }
                        else {
                            draw_element->addAttribute ("refframe")->setValue (fname);
                        }
                    }
                    else {
                        draw_element->addAttribute ("refframe")->setValue (fname);
                    }

                    // Add the rotation in RPY
                    RPY<> rpy (mod->getTransform ().R ());
                    rpy (0) = rpy (0) * Rad2Deg;
                    rpy (1) = rpy (1) * Rad2Deg;
                    rpy (2) = rpy (2) * Rad2Deg;
                    DOMBasisTypes::createRPY (rpy, draw_element);

                    // Add the position
                    DOMBasisTypes::createPos (mod->getTransform ().P (), draw_element);

                    std::string t ("#");
                    if (!mod->getFilePath ().empty () &&
                        mod->getFilePath ().compare (0, t.length (), t) != 0) {
                        DOMElem::Ptr polytope_element = draw_element->addChild ("Polytope");
                        // std::cout << "Object model filepath: " << mod->getFilePath() <<
                        // std::endl;
                        polytope_element->addAttribute ("file")->setValue (mod->getFilePath ());
                    }
                    else {
                        // Resolve type of geometry

                        // First decode the filePath string, which in this case contains info for
                        // creating geometry
                        std::string type, param1, param2, param3, param4, param5;
                        std::istringstream ss (mod->getFilePath ());
                        ss >> type >> param1 >> param2 >> param3 >> param4 >> param5;
                        /*std::cout << type << std::endl << param1 << std::endl << param2 <<
                           std::endl << param3
                                  << std::endl <<
                                  param4 << std::endl << param5 << std::endl;*/

                        if (type == "#Plane") {
                            DOMElem::Ptr plane_element = draw_element->addChild ("Plane");
                        }
                        else if (type == "#Box") {
                            DOMElem::Ptr box_element = draw_element->addChild ("Box");
                            box_element->addAttribute ("x")->setValue (param1);
                            box_element->addAttribute ("y")->setValue (param2);
                            box_element->addAttribute ("z")->setValue (param3);
                        }
                        else if (type == "#Sphere") {
                            DOMElem::Ptr sphere_element = draw_element->addChild ("Sphere");
                            sphere_element->addAttribute ("radius")->setValue (param1);
                        }
                        else if (type == "#Cone") {
                            DOMElem::Ptr cone_element = draw_element->addChild ("Cone");
                            cone_element->addAttribute ("radius")->setValue (param1);
                            cone_element->addAttribute ("z")->setValue (param2);
                        }
                        else if (type == "#Cylinder") {
                            DOMElem::Ptr cyl_element = draw_element->addChild ("Cylinder");
                            cyl_element->addAttribute ("radius")->setValue (param1);
                            cyl_element->addAttribute ("z")->setValue (param2);
                        }
                        else if (type == "#Tube") {
                            DOMElem::Ptr tube_element = draw_element->addChild ("Tube");
                            tube_element->addAttribute ("radius")->setValue (param1);
                            tube_element->addAttribute ("x")->setValue (param2);
                            tube_element->addAttribute ("z")->setValue (param3);
                        }
                        else {
                            // std::cout << "Unknown geometry type" << std::endl;
                        }
                    }
                }
            }
            else if (!object->getGeometry ().empty () && object->getModels ().empty ()) {
                // std::cout << "object is only a collision geometry" << std::endl;
                for (auto geom : object->getGeometry ()) {
                    DOMElem::Ptr mod_element = element->addChild ("CollisionModel");
                    // std::cout << "Object geometry name: " << geom->getName() << std::endl;
                    mod_element->addAttribute ("name")->setValue (geom->getName ());

                    std::string fname = geom->getName ();
                    std::string dname = dev->getName ();
                    // std::cout << dname << "-->" << fname << std::endl;
                    if (fname.length () > dname.length ()) {
                        if (dname == fname.substr (0, dname.length ())) {
                            mod_element->addAttribute ("refframe")
                                ->setValue (fname.substr (dname.length () + 1,
                                                          fname.length () - (dname.length () + 1)));
                        }
                        else {
                            mod_element->addAttribute ("refframe")->setValue (fname);
                        }
                    }
                    else {
                        mod_element->addAttribute ("refframe")->setValue (fname);
                    }

                    // Add the rotation in RPY
                    RPY<> rpy (geom->getTransform ().R ());
                    rpy (0) = rpy (0) * Rad2Deg;
                    rpy (1) = rpy (1) * Rad2Deg;
                    rpy (2) = rpy (2) * Rad2Deg;
                    DOMBasisTypes::createRPY (rpy, mod_element);

                    // Add the position
                    DOMBasisTypes::createPos (geom->getTransform ().P (), mod_element);

                    if (!geom->getFilePath ().empty ()) {
                        DOMElem::Ptr polytope_element = mod_element->addChild ("Polytope");
                        // std::cout << "Object geometry filepath: " << geom->getFilePath() <<
                        // std::endl;
                        polytope_element->addAttribute ("file")->setValue (geom->getFilePath ());
                    }
                    else {
                        // Resolve type of geometry
                        if (dynamic_cast< rw::geometry::Plane* > (
                                geom->getGeometryData ().get ())) {
                            DOMElem::Ptr plane_element = mod_element->addChild ("Plane");
                        }
                        else if (rw::geometry::Box* const box = dynamic_cast< rw::geometry::Box* > (
                                     geom->getGeometryData ().get ())) {
                            DOMElem::Ptr box_element = mod_element->addChild ("Box");
                            box_element->addAttribute ("x")->setValue (box->getParameters ()[0]);
                            box_element->addAttribute ("y")->setValue (box->getParameters ()[1]);
                            box_element->addAttribute ("z")->setValue (box->getParameters ()[2]);
                        }
                        else if (rw::geometry::Sphere* sphere =
                                     dynamic_cast< rw::geometry::Sphere* > (
                                         geom->getGeometryData ().get ())) {
                            DOMElem::Ptr sphere_element = mod_element->addChild ("Sphere");
                            sphere_element->addAttribute ("radius")->setValue (
                                sphere->getRadius ());
                        }
                        else if (rw::geometry::Cone* cone = dynamic_cast< rw::geometry::Cone* > (
                                     geom->getGeometryData ().get ())) {
                            DOMElem::Ptr cone_element = mod_element->addChild ("Cone");
                            cone_element->addAttribute ("radius")->setValue (
                                cone->getBottomRadius ());
                            cone_element->addAttribute ("z")->setValue (cone->getHeight ());
                        }
                        else if (rw::geometry::Cylinder* cyl =
                                     dynamic_cast< rw::geometry::Cylinder* > (
                                         geom->getGeometryData ().get ())) {
                            DOMElem::Ptr cyl_element = mod_element->addChild ("Cylinder");
                            cyl_element->addAttribute ("radius")->setValue (cyl->getRadius ());
                            cyl_element->addAttribute ("z")->setValue (cyl->getHeight ());
                        }
                        else if (rw::geometry::Tube* tube = dynamic_cast< rw::geometry::Tube* > (
                                     geom->getGeometryData ().get ())) {
                            DOMElem::Ptr tube_element = mod_element->addChild ("Tube");
                            tube_element->addAttribute ("radius")->setValue (
                                tube->getInnerRadius ());
                            tube_element->addAttribute ("x")->setValue (tube->getThickness ());
                            tube_element->addAttribute ("z")->setValue (tube->getHeight ());
                        }
                        else {
                            // std::cout << "Unknown geometry type" << std::endl;
                        }
                    }
                }
            }
            else if (!object->getGeometry ().empty () && !object->getModels ().empty ()) {
                // std::cout << "object both collision geometry and drawable vis model" <<
                // std::endl;
                for (const auto& mod : object->getModels ()) {
                    DOMElem::Ptr draw_element = element->addChild ("Drawable");
                    // std::cout << "Object model name: " << mod->getName() << std::endl;
                    draw_element->addAttribute ("name")->setValue (mod->getName ());

                    std::string fname = mod->getName ();
                    std::string dname = dev->getName ();
                    // std::cout << dname << "-->" << fname << std::endl;
                    if (fname.length () > dname.length ()) {
                        if (dname == fname.substr (0, dname.length ())) {
                            draw_element->addAttribute ("refframe")
                                ->setValue (fname.substr (dname.length () + 1,
                                                          fname.length () - (dname.length () + 1)));
                        }
                        else {
                            draw_element->addAttribute ("refframe")->setValue (fname);
                        }
                    }
                    else {
                        draw_element->addAttribute ("refframe")->setValue (fname);
                    }

                    // Add the rotation in RPY
                    RPY<> rpy (mod->getTransform ().R ());
                    rpy (0) = rpy (0) * Rad2Deg;
                    rpy (1) = rpy (1) * Rad2Deg;
                    rpy (2) = rpy (2) * Rad2Deg;
                    DOMBasisTypes::createRPY (rpy, draw_element);

                    // Add the position
                    DOMBasisTypes::createPos (mod->getTransform ().P (), draw_element);

                    if (!mod->getFilePath ().empty ()) {
                        DOMElem::Ptr polytope_element = draw_element->addChild ("Polytope");
                        // std::cout << "Object model filepath: " << mod->getFilePath() <<
                        // std::endl;
                        polytope_element->addAttribute ("file")->setValue (mod->getFilePath ());
                    }
                    else {
                        // Resolve type of geometry
                    }
                }
            }
        }

        return element;
    }
    else {
        // std::cout << "The Joint is a regular Joint" << std::endl;
        DOMElem::Ptr element = parent->addChild ("Joint");
        element->addAttribute ("name")->setValue (scopedName (frameToDevice[frame], frame));
        element->addAttribute ("refframe")->setValue (scopedName (frame, frame->getParent (state)));
        const std::string type = "Revolute";
        element->addAttribute ("type")->setValue (type);

        // Add the rotation in RPY
        RPY<> rpy (frame->getTransform (0.0).R ());
        rpy (0) = rpy (0) * Rad2Deg;
        rpy (1) = rpy (1) * Rad2Deg;
        rpy (2) = rpy (2) * Rad2Deg;
        DOMBasisTypes::createRPY (rpy, element);

        // Add the position
        DOMBasisTypes::createPos (frame->getTransform (state).P (), element);

        Q pmin = frame->getBounds ().first;
        Q pmax = frame->getBounds ().second;
        Q vel  = frame->getMaxVelocity ();
        Q acc  = frame->getMaxAcceleration ();

        DOMElem::Ptr pos_limit_element = element->addChild ("PosLimit");
        pos_limit_element->addAttribute ("min")->setValue (pmin (0) * Rad2Deg);
        pos_limit_element->addAttribute ("max")->setValue (pmax (0) * Rad2Deg);
        DOMElem::Ptr vel_limit_element = element->addChild ("VelLimit");
        vel_limit_element->addAttribute ("max")->setValue (vel (0) * Rad2Deg);
        DOMElem::Ptr acc_limit_element = element->addChild ("AccLimit");
        acc_limit_element->addAttribute ("max")->setValue (acc (0) * Rad2Deg);

        // Get any drawables or collision models associated with this frame
        rw::models::Object::Ptr object = workcell->findObject (frame->getName ());
        writeDrawablesAndColModels (object, element);

        return element;
    }
}

template<>
DOMElem::Ptr ElementCreator::createElement< MovableFrame* > (
    MovableFrame* frame, rw::core::Ptr< const rw::models::WorkCell > workcell, const State state,
    DOMElem::Ptr parent)
{
    DOMElem::Ptr element = parent->addChild ("Frame");

    // Set attributes
    element->addAttribute ("name")->setValue (frame->getName ());
    element->addAttribute ("refframe")->setValue (frame->getParent ()->getName ());

    const std::string type = "Movable";
    element->addAttribute ("type")->setValue (type);

    // Add the rotation in RPY
    RPY<> rpy (frame->getTransform (state).R ());
    rpy (0) = rpy (0) * Rad2Deg;
    rpy (1) = rpy (1) * Rad2Deg;
    rpy (2) = rpy (2) * Rad2Deg;
    DOMBasisTypes::createRPY (rpy, element);

    // Add the position
    DOMBasisTypes::createPos (frame->getTransform (state).P (), element);

    // Get any drawables or collision models associated with this frame
    rw::models::Object::Ptr object = workcell->findObject (frame->getName ());
    writeDrawablesAndColModels (object, element);

    for (const PropertyBase::Ptr& prop : frame->getPropertyMap ().getProperties ()) {
        const Property< std::string >* property = rw::core::toProperty< std::string > (prop);
        if (property != nullptr) {
            DOMElem::Ptr prop_element = element->addChild (DOMPropertyMapFormat::idProperty ());
            prop_element->addAttribute ("name")->setValue (property->getIdentifier ());
            prop_element->addAttribute ("desc")->setValue (property->getDescription ());
            prop_element->setValue (property->getValue ());
        }
    }

    return element;
}

template<>
DOMElem::Ptr ElementCreator::createElement< FixedFrame* > (
    FixedFrame* frame, rw::core::Ptr< const rw::models::WorkCell > workcell, DOMElem::Ptr parent)
{
    DOMElem::Ptr element = parent->addChild ("Frame");

    // Set attributes
    element->addAttribute ("name")->setValue (frame->getName ());
    element->addAttribute ("refframe")->setValue (frame->getParent ()->getName ());

    const std::string type = "Fixed";
    element->addAttribute ("type")->setValue (type);

    // Add the rotation in RPY
    RPY<> rpy (frame->getFixedTransform ().R ());
    rpy (0) = rpy (0) * Rad2Deg;
    rpy (1) = rpy (1) * Rad2Deg;
    rpy (2) = rpy (2) * Rad2Deg;
    DOMBasisTypes::createRPY (rpy, element);

    // Add the position
    DOMBasisTypes::createPos (frame->getFixedTransform ().P (), element);

    // Get any drawables or collision models associated with this frame
    rw::models::Object::Ptr object = workcell->findObject (frame->getName ());
    writeDrawablesAndColModels (object, element);

    for (const PropertyBase::Ptr& prop : frame->getPropertyMap ().getProperties ()) {
        const Property< std::string >* property = rw::core::toProperty< std::string > (prop);
        if (property != nullptr) {
            DOMElem::Ptr prop_element = element->addChild (DOMPropertyMapFormat::idProperty ());
            prop_element->addAttribute ("name")->setValue (property->getIdentifier ());
            prop_element->addAttribute ("desc")->setValue (property->getDescription ());
            prop_element->setValue (property->getValue ());
        }
    }

    return element;
}

template<>
DOMElem::Ptr
ElementCreator::createElement< FixedFrame* > (FixedFrame* frame,
                                              rw::core::Ptr< const rw::models::WorkCell > workcell,
                                              Device::Ptr dev, DOMElem::Ptr parent)
{
    DOMElem::Ptr element = parent->addChild ("Frame");
    // Set attributes
    std::string fname = frame->getName ();
    std::string dname = dev->getName ();
    // std::cout << dname << "-->" << fname << std::endl;
    if (fname.length () > dname.length ()) {
        if (dname == fname.substr (0, dname.length ())) {
            element->addAttribute ("name")->setValue (
                fname.substr (dname.length () + 1, fname.length () - (dname.length () + 1)));
        }
        else {
            element->addAttribute ("name")->setValue (fname);
        }
    }
    else {
        element->addAttribute ("name")->setValue (fname);
    }

    fname = frame->getParent ()->getName ();

    if (fname.length () > dname.length ()) {
        if (dname == fname.substr (0, dname.length ())) {
            element->addAttribute ("refframe")
                ->setValue (
                    fname.substr (dname.length () + 1, fname.length () - (dname.length () + 1)));
        }
        else {
            element->addAttribute ("refframe")->setValue (fname);
        }
    }
    else {
        element->addAttribute ("refframe")->setValue (fname);
    }

    // const std::string type = "Fixed";
    // element->addAttribute("type")->setValue(type);

    // Add the rotation in RPY
    RPY<> rpy (frame->getFixedTransform ().R ());
    rpy (0) = rpy (0) * Rad2Deg;
    rpy (1) = rpy (1) * Rad2Deg;
    rpy (2) = rpy (2) * Rad2Deg;
    DOMBasisTypes::createRPY (rpy, element);

    // Add the position
    DOMBasisTypes::createPos (frame->getFixedTransform ().P (), element);

    for (const PropertyBase::Ptr& prop : frame->getPropertyMap ().getProperties ()) {
        const Property< std::string >* property = rw::core::toProperty< std::string > (prop);
        if (property != nullptr) {
            DOMElem::Ptr prop_element = element->addChild (DOMPropertyMapFormat::idProperty ());
            prop_element->addAttribute ("name")->setValue (property->getIdentifier ());
            prop_element->addAttribute ("desc")->setValue (property->getDescription ());
            prop_element->setValue (property->getValue ());
        }
    }

    // Get any drawables or collision models associated with this frame
    rw::models::Object::Ptr object = workcell->findObject (frame->getName ());
    writeDrawablesAndColModels (object, element);

    return element;
}

void writeDeviceFrame (rw::core::Ptr< Frame > frame, ElementCreator& creator,
                       rw::core::Ptr< const rw::models::WorkCell > workcell, const State state,
                       Device::Ptr dev, DOMElem::Ptr parent)
{
    if (FixedFrame* ff = frame.cast< FixedFrame > ().get ()) {
        // std::cout << "The frame type was Fixed!" << std::endl;
        if (isFrameInDevice (frame))
            creator.createElement< FixedFrame* > (ff, workcell, dev, parent);
        else
            creator.createElement< FixedFrame* > (ff, workcell, parent);
    }
    else if (MovableFrame* mf = frame.cast< MovableFrame > ().get ()) {
        // std::cout << "The frame type was Movable!" << std::endl;
        creator.createElement< MovableFrame* > (mf, workcell, state, parent);
    }
    else if (RevoluteJoint* rj = frame.cast< RevoluteJoint > ().get ()) {
        // std::cout << "The frame type was RevoluteJoint" << std::endl;
        creator.createElement< RevoluteJoint* > (rj, workcell, state, dev, parent);
    } /*
     else if(PrismaticJoint *pj = dynamic_cast<PrismaticJoint*>(frame)) {
         //std::cout << "The frame type was PrismaticJoint" << std::endl;
     }
     else if(DependentRevoluteJoint* rdj = dynamic_cast<DependentRevoluteJoint*>(frame)) {
         //std::cout << "The frame type was DependentRevoluteJoint" << std::endl;
     }
     else if(DependentPrismaticJoint* pdj = dynamic_cast<DependentPrismaticJoint*>(frame)) {
         //std::cout << "The frame type was DependentPrismaticJoint" << std::endl;
     }
     else
     {
         //std::cout << "The frame type could not be serialized" << std::endl;
     }*/
}

void writeFrame (rw::core::Ptr< Frame > frame, ElementCreator& creator,
                 rw::core::Ptr< const rw::models::WorkCell > workcell, const State state,
                 DOMElem::Ptr parent)
{
    if (FixedFrame* ff = frame.cast< FixedFrame > ().get ()) {
        // std::cout << "The frame type was Fixed!" << std::endl;
        creator.createElement< FixedFrame* > (ff, workcell, parent);
    }
    else if (MovableFrame* mf = frame.cast< MovableFrame > ().get ()) {
        // std::cout << "The frame type was Movable!" << std::endl;
        creator.createElement< MovableFrame* > (mf, workcell, state, parent);
    }
    else if (RevoluteJoint* rj = frame.cast< RevoluteJoint > ().get ()) {
        // std::cout << "The frame type was RevoluteJoint" << std::endl;
        creator.createElement< RevoluteJoint* > (rj, workcell, state, parent);
    } /*
     else if(PrismaticJoint *pj = dynamic_cast<PrismaticJoint*>(frame)) {
         //std::cout << "The frame type was PrismaticJoint" << std::endl;
     }
     else if(DependentRevoluteJoint* rdj = dynamic_cast<DependentRevoluteJoint*>(frame)) {
         //std::cout << "The frame type was DependentRevoluteJoint" << std::endl;
     }
     else if(DependentPrismaticJoint* pdj = dynamic_cast<DependentPrismaticJoint*>(frame)) {
         //std::cout << "The frame type was DependentPrismaticJoint" << std::endl;
     }
     else
     {
         //std::cout << "The frame type could not be serialized" << std::endl;
     }*/
}

void createDOMDocument (DOMElem::Ptr rootDoc, rw::core::Ptr< const rw::models::WorkCell > workcell,
                        const State state)
{
    DOMElem::Ptr rootElement = rootDoc->addChild ("WorkCell");
    rootElement->addAttribute ("name")->setValue (workcell->getName ());
    ElementCreator creator (rootElement);

    // Get all frames in the workcell
    const std::vector< Frame* > wc_frames = workcell->getFrames ();

    // Init frame maps to be used for writing devices
    std::vector< Device::Ptr > devices = workcell->getDevices ();
    for (Device::Ptr devp : devices) {
        Device* dev                   = devp.get ();
        std::vector< Frame* > dframes = Kinematics::findAllFrames (dev->getBase ());
        std::string dname             = dev->getName ();
        for (Frame* dframe : dframes) {
            std::string fname = dframe->getName ();

            if (fname.length () > dname.length ()) {
                if (dname == fname.substr (0, dname.length ())) {
                    frameToDevice[dframe] = dev;
                }
            }
        }

        if (TreeDevice* tdev = dynamic_cast< TreeDevice* > (dev)) {
            for (Frame* endf : tdev->getEnds ())
                isEndEffector[endf] = true;
        }
        else {
            isEndEffector[dev->getEnd ()] = true;
        }

        parentToDevice[dev->getBase ()->getParent ()] = dev;
    }

    for (const auto& frame : wc_frames) {
        // The WORLD frame should not be added to the file
        if (frame->getName () == "WORLD") {
            // Skip the frame
            continue;
        }
        if (isFrameInDevice (frame)) {
            // std::cout << "The frame is in a device" << std::endl;
            // Skip the frame
            continue;
        }
        if (isFrameParentToDevice (frame)) {
            // std::cout << "The frame is a parent to a device" << std::endl;
            // Skip the frame
            continue;
        }

        writeFrame (frame, creator, workcell, state, rootElement);
    }

    for (Device::Ptr dev : devices) {
        // First write parent frame to device
        rw::core::Ptr< Frame > parent = dev->getBase ()->getParent ();
        writeFrame (parent, creator, workcell, state, rootElement);

        std::string devType      = getDeviceType (*dev);
        DOMElem::Ptr dev_element = rootElement->addChild (devType);
        dev_element->addAttribute ("name")->setValue (dev->getName ());
        // write device joint structure
        std::vector< Frame* > flist;
        std::stack< Frame* > frames;
        frames.push (dev->getBase ());
        while (!frames.empty ()) {
            rw::core::Ptr< Frame > frame = frames.top ();
            frames.pop ();
            flist.push_back (frame.get ());

            writeDeviceFrame (frame, creator, workcell, state, dev, dev_element);

            for (Frame::Ptr child : frame->getChildrenList (state)) {
                if (!isFrameInDevice (child.get (), dev.get ()))
                    continue;
                frames.push (child.get ());
            }
        }
        DOMElem::Ptr q_element = dev_element->addChild (DOMBasisTypes::idQ ());
        const std::string home = "Home";
        q_element->addAttribute ("name")->setValue (home);
        q_element->setValue (createStringFromArray (dev->getQ (workcell->getDefaultState ())));
    }

    // obtain current proximity setup from workcell
    rw::proximity::ProximitySetup prox_setup = rw::proximity::ProximitySetup::get (*workcell);

    // get where to save the proximity setup, assumes that a main proximity setup file was provided.
    const std::string id_main = "ProximitySetupFilePath";
    const std::string id_rel  = "ProximitySetupRelFilePath";
    std::string proxFilePath =
        static_cast< std::string > (workcell->getPropertyMap ().get< std::string > (id_main));
    std::string proxRelFilepath =
        static_cast< std::string > (workcell->getPropertyMap ().get< std::string > (id_rel));
    // std::cout << "Proximity filename: " << proxFilePath << std::endl;
    // save using the DOMProximitySetupSaver
    rw::loaders::DOMProximitySetupSaver::save (prox_setup, proxFilePath);

    // add relative reference to the proximity setup file
    DOMElem::Ptr element = rootElement->addChild ("ProximitySetup");
    element->addAttribute ("file")->setValue (proxRelFilepath);
}
}    // end of anonymous namespace

void DOMWorkCellSaver::save (rw::core::Ptr< const rw::models::WorkCell > workcell,
                             const State& state, std::string fileName)
{
    DOMParser::Ptr doc = DOMParser::make ();
    DOMElem::Ptr root  = doc->getRootElement ();

    createDOMDocument (root, workcell, state);

    // save to file
    doc->save (fileName);
}

void DOMWorkCellSaver::save (rw::core::Ptr< const rw::models::WorkCell > workcell,
                             const State& state, std::ostream& ostream)
{
    DOMParser::Ptr doc = DOMParser::make ();
    DOMElem::Ptr root  = doc->getRootElement ();

    createDOMDocument (root, workcell, state);

    // save to stream
    doc->save (ostream);
}
