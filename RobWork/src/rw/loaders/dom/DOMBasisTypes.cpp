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

#include "DOMBasisTypes.hpp"

#include <rw/core/DOMElem.hpp>
#include <rw/core/StringUtil.hpp>
#include <rw/core/macros.hpp>
#include <rw/kinematics/Frame.hpp>
#include <rw/kinematics/StateStructure.hpp>
#include <rw/math/LinearAlgebra.hpp>
#include <rw/models/WorkCell.hpp>

#include <boost/lexical_cast.hpp>
#include <map>
#include <sstream>
#include <vector>

using namespace rw::math;
using namespace rw::core;
using namespace rw::kinematics;
using namespace rw::models;
using namespace rw::loaders;
using namespace rw;

DOMBasisTypes::Initializer::Initializer ()
{
    static bool done = false;
    if (!done) {
        idQ ();
        idVector3D ();
        idVector2D ();
        idRotation3D ();
        idRPY ();
        idEAA ();
        idQuaternion ();
        idRotation2D ();
        idRotation2DAngle ();
        idTransform2D ();
        idTransform3D ();
        idMatrix ();
        idVelocityScrew6D ();
        idPos ();
        idLinear ();
        idAngular ();
        idState ();
        idQState ();
        idTreeState ();
        idBoolean ();
        idDouble ();
        idFloat ();
        idInteger ();
        idString ();
        idStringList ();
        idIntList ();
        idDoubleList ();
        idStringPair ();
        idUnitAttribute ();
        done = true;
    }
}

const DOMBasisTypes::Initializer DOMBasisTypes::initializer;

// Definition of Identifiers used in the XML format
const std::string& DOMBasisTypes::idQ ()
{
    static const std::string id ("Q");
    return id;
}

const std::string& DOMBasisTypes::idVector3D ()
{
    static const std::string id ("Vector3D");
    return id;
}

const std::string& DOMBasisTypes::idVector2D ()
{
    static const std::string id ("Vector2D");
    return id;
}

const std::string& DOMBasisTypes::idRotation3D ()
{
    static const std::string id ("Rotation3D");
    return id;
}

const std::string& DOMBasisTypes::idRPY ()
{
    static const std::string id ("RPY");
    return id;
}

const std::string& DOMBasisTypes::idEAA ()
{
    static const std::string id ("EAA");
    return id;
}

const std::string& DOMBasisTypes::idQuaternion ()
{
    static const std::string id ("Quaternion");
    return id;
}

const std::string& DOMBasisTypes::idRotation2D ()
{
    static const std::string id ("Rotation2D");
    return id;
}

const std::string& DOMBasisTypes::idRotation2DAngle ()
{
    static const std::string id ("Rotation2DAngle");
    return id;
}

const std::string& DOMBasisTypes::idTransform2D ()
{
    static const std::string id ("Transform2D");
    return id;
}

const std::string& DOMBasisTypes::idTransform3D ()
{
    static const std::string id ("Transform3D");
    return id;
}

const std::string& DOMBasisTypes::idMatrix ()
{
    static const std::string id ("Matrix");
    return id;
}

const std::string& DOMBasisTypes::idVelocityScrew6D ()
{
    static const std::string id ("VelocityScrew6D");
    return id;
}

const std::string& DOMBasisTypes::idPos ()
{
    static const std::string id ("Pos");
    return id;
}

const std::string& DOMBasisTypes::idLinear ()
{
    static const std::string id ("Linear");
    return id;
}

const std::string& DOMBasisTypes::idAngular ()
{
    static const std::string id ("Angular");
    return id;
}

const std::string& DOMBasisTypes::idState ()
{
    static const std::string id ("State");
    return id;
}

const std::string& DOMBasisTypes::idQState ()
{
    static const std::string id ("QState");
    return id;
}

const std::string& DOMBasisTypes::idTreeState ()
{
    static const std::string id ("TreeState");
    return id;
}

const std::string& DOMBasisTypes::idBoolean ()
{
    static const std::string id ("Boolean");
    return id;
}

const std::string& DOMBasisTypes::idDouble ()
{
    static const std::string id ("Double");
    return id;
}

const std::string& DOMBasisTypes::idFloat ()
{
    static const std::string id ("Float");
    return id;
}

const std::string& DOMBasisTypes::idInteger ()
{
    static const std::string id ("Integer");
    return id;
}

const std::string& DOMBasisTypes::idString ()
{
    static const std::string id ("String");
    return id;
}

const std::string& DOMBasisTypes::idStringList ()
{
    static const std::string id ("StringList");
    return id;
}

const std::string& DOMBasisTypes::idIntList ()
{
    static const std::string id ("IntList");
    return id;
}

const std::string& DOMBasisTypes::idDoubleList ()
{
    static const std::string id ("DoubleList");
    return id;
}

const std::string& DOMBasisTypes::idStringPair ()
{
    static const std::string id ("StringPair");
    return id;
}

const std::string& DOMBasisTypes::idUnitAttribute ()
{
    static const std::string id ("unit");
    return id;
}

namespace {
struct UnitMap
{
  public:
    std::map< std::string, double > _map;

    UnitMap ()
    {
        _map["mm"]   = 1.0 / 1000.0;
        _map["cm"]   = 1.0 / 100.0;
        _map["m"]    = 1;
        _map["inch"] = 0.0254;

        _map["deg"] = Deg2Rad;
        _map["rad"] = 1;

        _map["m/s"]  = 1;
        _map["cm/s"] = 1.0 / 100.0;
        _map["mm/s"] = 1.0 / 1000.0;

        _map["m/s^2"]  = 1;
        _map["cm/s^2"] = 1.0 / 100.0;
        _map["mm/s^2"] = 1.0 / 1000.0;

        _map["deg/s"] = Deg2Rad;
        _map["rad/s"] = 1;

        _map["deg/s^2"] = Deg2Rad;
        _map["rad/s^2"] = 1;
    };
    UnitMap (const std::map< std::string, double >& map) : _map (map) {}

    ~UnitMap () {}
};
}    // namespace

// const DOMBasisTypes::UnitMap DOMBasisTypes::_Units;
const UnitMap _Units;

double DOMBasisTypes::getUnit (const std::string key)
{
    std::map< std::string, double >::const_iterator it = _Units._map.find (key);
    if (it == _Units._map.end ())
        RW_THROW ("Invalid Unit Attribute " << key);
    return (*it).second;
}

namespace {
double readUnit (DOMElem::Ptr element)
{
    std::string attrval = element->getAttributeValue (DOMBasisTypes::idUnitAttribute (), "");
    if (!attrval.empty ())
        return DOMBasisTypes::getUnit (attrval);
    return 1;
}

void checkHeader (DOMElem::Ptr element, const std::string id)
{
    if (!element->isName (id))
        RW_THROW ("Expected \"" << id << "\" got " << element->getName ());
}

template< class T >
inline T readVectorStructure (DOMElem::Ptr element, bool doCheckHeader, const std::string id)
{
    if (doCheckHeader)
        checkHeader (element, id);

    double scale                   = readUnit (element);
    std::vector< double > elements = element->getValueAsDoubleList ();

    T result;
    if (elements.size () != result.size ())
        RW_THROW ("Parse error: in element \"" << element->getName ()
                                               << "\" nr of elements must be \"" << result.size ()
                                               << "\" got \"" << elements.size () << "\"");

    for (size_t i = 0; i < elements.size (); i++) {
        result (i) = scale * elements[i];
    }
    return result;
}

}    // namespace

Q DOMBasisTypes::readQ (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idQ ());

    std::vector< double > values = element->getValueAsDoubleList ();
    Q q (values.size ());
    for (size_t i = 0; i < values.size (); ++i)
        q (i) = values[i];

    return q;
}

Vector3D<> DOMBasisTypes::readVector3D (DOMElem::Ptr element, bool doCheckHeader)
{
    return readVectorStructure< Vector3D<> > (element, doCheckHeader, idVector3D ());
}

Vector2D<> DOMBasisTypes::readVector2D (DOMElem::Ptr element, bool doCheckHeader)
{
    return readVectorStructure< Vector2D<> > (element, doCheckHeader, idVector2D ());
}

RPY<> DOMBasisTypes::readRPY (DOMElem::Ptr element, bool doCheckHeader)
{
    return readVectorStructure< RPY<> > (element, doCheckHeader, idRPY ());
}

EAA<> DOMBasisTypes::readEAA (DOMElem::Ptr element, bool doCheckHeader)
{
    return readVectorStructure< EAA<> > (element, doCheckHeader, idEAA ());
}

Quaternion<> DOMBasisTypes::readQuaternion (DOMElem::Ptr element, bool doCheckHeader)
{
    Quaternion<> qua =
        readVectorStructure< Quaternion<> > (element, doCheckHeader, idQuaternion ());
    qua.normalize ();
    return qua;
}

Rotation3D<> DOMBasisTypes::readRotation3D (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idRotation3D ());

    std::vector< double > values = element->getValueAsDoubleList ();
    // we are switching to quaternions instead... rotation3d are simply to error prone to precision
    // errors

    if (values.size () == 4) {
        Quaternion<> quat (values[0], values[1], values[2], values[3]);
        quat.normalize ();
        Rotation3D<> rot = quat.toRotation3D ();

        while (fabs (rot.e ().determinant () - 1.0) > 0.00001) {
            Eigen::MatrixXd u, v;
            Eigen::VectorXd w;

            std::cout.precision (17);
            std::cout << rot << std::endl;
            RW_WARN ("Parse of Rotation3D failed. A rotation 3d must be an "
                     "orthogonal matrix with determinant of 1! det="
                     << rot.e ().determinant ());
            LinearAlgebra::svd (rot.e (), u, w, v);
            Eigen::MatrixXd res = u * v.transpose ();
            rot                 = Rotation3D<> (res);
            return rot;
        }

        return rot;
    }
    else if (values.size () == 9) {
        Rotation3D<> rot (values[0],
                          values[1],
                          values[2],
                          values[3],
                          values[4],
                          values[5],
                          values[6],
                          values[7],
                          values[8]);

        while (fabs (rot.e ().determinant () - 1.0) > 0.00001) {
            Eigen::MatrixXd u, v;
            Eigen::VectorXd w;

            std::cout.precision (17);
            std::cout << rot << std::endl;
            RW_WARN ("Parse of Rotation3D failed. A rotation 3d must be an "
                     "orthogonal matrix with determinant of 1! det="
                     << rot.e ().determinant ());
            LinearAlgebra::svd (rot.e (), u, w, v);
            Eigen::MatrixXd res = u * v.transpose ();
            rot                 = Rotation3D<> (res);
        }
        return rot;
    }

    RW_THROW ("Expected 9 or 4 (quaternion) floating points for Rotation3D. Only "
              << values.size () << " values found");
}

Rotation2D<> DOMBasisTypes::readRotation2D (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idRotation2D ());

    std::vector< double > values = element->getValueAsDoubleList ();
    if (values.size () != 4)
        RW_THROW ("Expected 4 floating points for Rotation3D. Only " << values.size ()
                                                                     << " values found");

    return Rotation2D<> (values[0], values[1], values[2], values[3]);
}

Rotation3D<> DOMBasisTypes::readRotation3DStructure (DOMElem::Ptr element)
{
    if (element->isName (idRotation3D ()))
        return readRotation3D (element, false);
    if (element->isName (idRPY ()))
        return readRPY (element, false).toRotation3D ();
    if (element->isName (idEAA ()))
        return readEAA (element, false).toRotation3D ();
    if (element->isName (idQuaternion ()))
        return readQuaternion (element, false).toRotation3D ();

    RW_THROW ("Unable to find match \"" << element->getName ()
                                        << "\" with (Rotation3D|RPY|EAA|Quaternion)");
    return Rotation3D<> ();
}

Rotation2D<> DOMBasisTypes::readRotation2DStructure (DOMElem::Ptr element)
{
    if (element->isName (idRotation2D ()))
        return readRotation2D (element, false);
    if (element->isName (idRotation2DAngle ())) {
        double angle = readDouble (element, false);
        return Rotation2D<> (angle);
    }

    RW_THROW ("Unable to find match \"" << element->getName () << "\" with (Rotation2D|Angle)");
    return Rotation2D<> ();
}

Transform3D<> DOMBasisTypes::readTransform3D (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idTransform3D ());

    Vector3D<> position (0, 0, 0);
    Rotation3D<> rotation (Rotation3D<>::identity ());

    std::vector< double > values = element->getValueAsDoubleList ();
    if (values.size () == 12) {
        rotation (0, 0) = values[0];
        rotation (0, 1) = values[1];
        rotation (0, 2) = values[2];
        rotation (1, 0) = values[4];
        rotation (1, 1) = values[5];
        rotation (1, 2) = values[6];
        rotation (2, 0) = values[8];
        rotation (2, 1) = values[9];
        rotation (2, 2) = values[10];

        position (0) = values[3];
        position (1) = values[7];
        position (2) = values[11];
    }
    else {
        bool isRotationSet = false;
        bool isPositionSet = false;

        for (DOMElem::Ptr child : element->getChildren ()) {
            if (child->isName (idMatrix ())) {
                if (isRotationSet) {
                    RW_THROW ("Rotation already specified for transform when reading "
                              << idMatrix ());
                }
                if (isPositionSet) {
                    RW_THROW ("Position already specified for transform when reading "
                              << idMatrix ());
                }

                std::vector< double > values = child->getValueAsDoubleList ();
                if (values.size () != 12)
                    RW_THROW ("Expected   <Matrix> with 12 doubles when parsing Transform3D. Found "
                              << values.size () << " values");
                rotation (0, 0) = values[0];
                rotation (0, 1) = values[1];
                rotation (0, 2) = values[2];
                rotation (1, 0) = values[4];
                rotation (1, 1) = values[5];
                rotation (1, 2) = values[6];
                rotation (2, 0) = values[8];
                rotation (2, 1) = values[9];
                rotation (2, 2) = values[10];

                position (0)  = values[3];
                position (1)  = values[7];
                position (2)  = values[11];
                isRotationSet = true;
                isPositionSet = true;
            }
            else if (child->isName (idPos ())) {
                if (isPositionSet) {
                    RW_THROW ("Positions already specified for transform when reading element "
                              << idPos ());
                }
                position      = readVector3D (child, false);
                isPositionSet = true;
            }
            else {
                if (isRotationSet) {
                    RW_THROW ("Rotations already specified for transform when reading element "
                              << child->getName ());
                }
                rotation      = readRotation3DStructure (child);
                isRotationSet = true;
            }
        }
    }
    // rotation.normalize();
    return Transform3D<> (position, rotation);
}

Transform2D<> DOMBasisTypes::readTransform2D (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idTransform3D ());

    Vector2D<> position (0, 0);
    Rotation2D<> rotation (Rotation2D<>::identity ());

    {
        std::vector< double > values = element->getValueAsDoubleList ();
        if (values.size () == 6) {
            rotation (0, 0) = values[0];
            rotation (0, 1) = values[1];
            rotation (1, 0) = values[2];
            rotation (1, 1) = values[3];

            position (0) = values[4];
            position (1) = values[5];
        }
    }

    for (DOMElem::Ptr child : element->getChildren ()) {
        if (child->isName (idMatrix ())) {
            std::vector< double > values = child->getValueAsDoubleList ();
            if (values.size () != 6)
                RW_THROW ("Expected <Matrix> with 6 doubles when parsing Transform2D. Found "
                          << values.size () << " values");
            rotation (0, 0) = values[0];
            rotation (0, 1) = values[1];
            rotation (1, 0) = values[2];
            rotation (1, 1) = values[3];

            position (0) = values[4];
            position (1) = values[5];
        }
        else if (child->isName (idPos ())) {
            position = readVector2D (child, false);
        }
        else {
            rotation = readRotation2D (child);
        }
    }
    // rotation.normalize();
    return Transform2D<> (position, rotation);
}

VelocityScrew6D<> DOMBasisTypes::readVelocityScrew6D (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idVelocityScrew6D ());

    Vector3D<> linear (0, 0, 0);
    EAA<> angular (0, 0, 0);
    for (DOMElem::Ptr child : element->getChildren ()) {
        if (child->isName (idPos ())) {
            linear = readVector3D (child, false);
        }
        else if (child->isName (idEAA ())) {
            angular = readEAA (child, false);
        }
        else {
            RW_THROW ("Unknown element \"" << child->getName ()
                                           << "\" specified in VelocityScrew6D");
        }
    }

    return VelocityScrew6D<> (linear, angular);
}

std::vector< double > DOMBasisTypes::readDoubleList (DOMElem::Ptr element, bool doCheckHeader)
{
    return element->getValueAsDoubleList ();
}

std::vector< int > DOMBasisTypes::readIntList (DOMElem::Ptr element, bool doCheckHeader)
{
    std::vector< double > res2 = element->getValueAsDoubleList ();
    std::vector< int > res (res2.size ());
    for (size_t i = 0; i < res2.size (); i++)
        res[i] = (int) res2[i];
    return res;
}

/**
 * @brief Returns rw::kinematics::State<> element read from \b element
 *
 * Read in \b element and returns a rw::kinematics::State corresponding to the content.
 * If \b doCheckHeader = true it checks that the elements tag name matches State.
 * If the name does not an exception is thrown.
 *
 * @param element [in] Element to read
 * @param doCheckHeader [in] True if the header name should be checked
 * @return The element read
 */
rw::kinematics::State DOMBasisTypes::readState (DOMElem::Ptr element, WorkCell::Ptr workcell,
                                                bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idState ());

    State result = workcell->getDefaultState ();

    for (DOMElem::Ptr child : element->getChildren ()) {
        if (child->isName (idQState ())) {
            Q q = readQ (child, false);
            if (result.size () != q.size ())
                RW_THROW ("Length of State loaded does not match workcell");
            for (size_t i = 0; i < q.size (); i++)
                result (i) = q (i);
        }
        else if (child->isName (idTreeState ())) {
            std::vector< StringPair > dafs = readStringPairs (element);
            for (std::vector< StringPair >::iterator it = dafs.begin (); it != dafs.end (); ++it) {
                rw::core::Ptr<Frame> daf    = workcell->findFrame ((*it).first);
                rw::core::Ptr<Frame> parent = workcell->findFrame ((*it).second);
                if (daf == NULL)
                    RW_THROW ("Unable to locate frame named \"" << (*it).first);
                if (parent == NULL)
                    RW_THROW ("Unable to locate frame named \"" << (*it).second);

                daf->attachTo (parent, result);
            }
        }
        else {
            RW_THROW ("Unknown element \"" << child->getName () << "\" specified in State");
        }
    }
    return result;
}

std::string DOMBasisTypes::readString (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idString ());

    return element->getValue ();
}

std::vector< std::string > DOMBasisTypes::readStringList (DOMElem::Ptr element)
{
    return element->getValueAsStringList ();
}

// typedef std::pair<std::string,std::string> StringPair;

StringPair DOMBasisTypes::readStringPair (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idStringPair ());

    const std::vector< std::string > result = readStringList (element);

    if (result.size () != 2)
        RW_THROW ("Expected 2 string in StringPair but found" << result.size ());

    return std::make_pair (result[0], result[1]);
}

std::vector< StringPair > DOMBasisTypes::readStringPairs (DOMElem::Ptr element)
{
    std::vector< StringPair > result;
    for (DOMElem::Ptr child : element->getChildren ()) {
        if (child->isName (idStringPair ())) {
            std::string str                    = readString (child);
            std::vector< std::string > strings = StringUtil::words (str);
            if (strings.size () != 2)
                RW_THROW ("Expected two string elements found " << strings.size () << " in \""
                                                                << str << "\"");
            result.push_back (std::make_pair (strings[0], strings[1]));
        }
    }
    return result;
}

double DOMBasisTypes::readDouble (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idDouble ());
    return element->getValueAsDouble ();
}

float DOMBasisTypes::readFloat (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idFloat ());
    return (float) element->getValueAsDouble ();
}

int DOMBasisTypes::readInt (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idInteger ());

    return element->getValueAsInt ();
}

bool DOMBasisTypes::readBool (DOMElem::Ptr element, bool doCheckHeader)
{
    if (doCheckHeader)
        checkHeader (element, idBoolean ());

    std::string str = element->getValue ();
    bool res        = false;
    try {
        res = boost::lexical_cast< bool > (str);
    }
    catch (...) {
        if (str == "true") {
            return true;
        }
        else if (str == "false") {
            return false;
        }
        else {
            RW_THROW ("Parse error: Could not parse bool, expected true,false,1 or 0 got \""
                      << str << "\"");
        }
    }
    return res;
}

namespace {
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

template< class T > std::string createStringFromVector (const T& v, size_t n)
{
    std::ostringstream str;
    str.unsetf (std::ios::floatfield);    // floatfield not set
    str.precision (16);
    for (size_t i = 0; i < n; ++i) {
        str << v[i];
        if (i != n - 1)
            str << " ";
    }
    return std::string (str.str ());
}

template< class T > std::string createStringFromVector (const T& v)
{
    return createStringFromVector< T > (v, v.size ());
}
}    // namespace

DOMElem::Ptr DOMBasisTypes::write (int val, DOMElem::Ptr elem, bool addHeader)
{
    if (addHeader)
        elem->setName (idInteger ());

    std::stringstream sstr;
    sstr << val;

    elem->setValue (sstr.str ());
    return elem;
}

DOMElem::Ptr DOMBasisTypes::write (double val, DOMElem::Ptr elem, bool addHeader)
{
    if (addHeader)
        elem->setName (idDouble ());

    std::stringstream sstr;
    sstr << val;

    elem->setValue (sstr.str ());
    return elem;
}

DOMElem::Ptr DOMBasisTypes::write (const std::string& str, DOMElem::Ptr elem, bool addHeader)
{
    if (addHeader)
        elem->setName (idString ());

    elem->setValue (str);
    return elem;
}

DOMElem::Ptr DOMBasisTypes::write (const Q& val, DOMElem::Ptr elem, bool addHeader)
{
    if (addHeader)
        elem->setName (idQ ());

    elem->setValue (createStringFromArray (val));
    return elem;
}

DOMElem::Ptr DOMBasisTypes::write (const Vector3D<>& val, DOMElem::Ptr elem, bool addHeader)
{
    if (addHeader)
        elem->setName (idVector3D ());

    elem->setValue (createStringFromArray (val));
    return elem;
}

DOMElem::Ptr DOMBasisTypes::write (const Vector2D<>& val, DOMElem::Ptr elem, bool addHeader)
{
    if (addHeader)
        elem->setName (idVector2D ());

    elem->setValue (createStringFromArray (val));
    return elem;
}

DOMElem::Ptr DOMBasisTypes::write (const Transform2D<>& val, DOMElem::Ptr elem, bool addHeader)
{
    if (addHeader)
        elem->setName (idTransform2D ());

    const Rotation2D<> r = val.R ();
    const Vector2D<> p   = val.P ();
    std::ostringstream str;
    str.unsetf (std::ios::floatfield);    // floatfield not set
    str.precision (17);
    str << r (0, 0) << " " << r (0, 1) << " " << p (0) << " ";
    str << r (1, 0) << " " << r (1, 1) << " " << p (1);

    elem->setValue (str.str ());
    return elem;
}

DOMElem::Ptr DOMBasisTypes::write (const Transform3D<>& val, DOMElem::Ptr elem, bool addHeader)
{
    if (addHeader)
        elem->setName (idTransform3D ());

    const Rotation3D<> r = val.R ();
    const Vector3D<> p   = val.P ();
    std::ostringstream str;
    str.unsetf (std::ios::floatfield);    // floatfield not set
    str.precision (17);

    str << r (0, 0) << " " << r (0, 1) << " " << r (0, 2) << " " << p (0) << " ";
    str << r (1, 0) << " " << r (1, 1) << " " << r (1, 2) << " " << p (1) << " ";
    str << r (2, 0) << " " << r (2, 1) << " " << r (2, 2) << " " << p (2) << " ";
    elem->setValue (str.str ());
    return elem;
}

DOMElem::Ptr DOMBasisTypes::write (const Eigen::MatrixXd& val, DOMElem::Ptr elem, bool addHeader)
{
    if (addHeader)
        elem->setName (idMatrix ());

    // we save dimension in the first 2 values
    std::ostringstream str;
    str.unsetf (std::ios::floatfield);    // floatfield not set
    str.precision (17);
    str << val.cols () << " " << val.rows ();
    for (int y = 0; y < val.rows (); y++) {
        for (int x = 0; x < val.cols (); x++) {
            str << " " << val (x, y);
        }
    }
    elem->setValue (str.str ());
    return elem;
}

Eigen::MatrixXd DOMBasisTypes::readMatrix (DOMElem::Ptr elem)
{
    std::vector< double > res = elem->getValueAsDoubleList ();
    int cols                  = (int) res[0];
    int rows                  = (int) res[1];
    Eigen::MatrixXd m (cols, rows);
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++)
            m (x, y) = res[2 + x + y * cols];
    return m;
}

DOMElem::Ptr DOMBasisTypes::createElement (const std::string& id, const std::string& value,
                                           DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (id);
    element->setValue (value);
    return element;
}

DOMElem::Ptr DOMBasisTypes::createQ (const Q& q, DOMElem::Ptr doc)
{
    return createElement (idQ (), createStringFromArray (q), doc);
}

DOMElem::Ptr DOMBasisTypes::createPos (const Vector3D<>& v, DOMElem::Ptr doc)
{
    return createElement (idPos (), createStringFromArray (v), doc);
}

DOMElem::Ptr DOMBasisTypes::createVector3D (const Vector3D<>& v, DOMElem::Ptr doc)
{
    return createElement (idVector3D (), createStringFromArray (v), doc);
}

DOMElem::Ptr DOMBasisTypes::createVector2D (const Vector2D<>& v, DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idVector2D ());
    element->setValue (createStringFromArray (v));
    return element;
}

DOMElem::Ptr DOMBasisTypes::createRPY (const RPY<>& v, DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idRPY ());
    element->setValue (createStringFromArray (v, 3));

    return element;
}

DOMElem::Ptr DOMBasisTypes::createEAA (const EAA<>& v, DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idEAA ());
    element->setValue (createStringFromArray (v, 3));

    return element;
}

DOMElem::Ptr DOMBasisTypes::createQuaternion (const Quaternion<>& q, DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idQuaternion ());
    element->setValue (createStringFromArray (q, 4));

    return element;
}

DOMElem::Ptr DOMBasisTypes::createRotation3D (const Rotation3D<>& r, DOMElem::Ptr doc)
{
    // DOMElem::Ptr element = doc->addChild(Rotation3DId);

    // check if rotation is proper orthogonal before saving it
    // RW_ASSERT( fabs(LinearAlgebra::det( target.R().m() ))-1.0 < 0.00000001 );
    double detVal = r.e ().determinant ();
    if (fabs (detVal - 1.0) > 0.000001) {
        RW_WARN ("A rotation matrix that is being streamed does not have a determinant of 1, det="
                 << detVal << ", difference: " << fabs (detVal - 1.0));
    }

    std::ostringstream str;
    str.unsetf (std::ios::floatfield);    // floatfield not set
    str.precision (17);
    str << r (0, 0) << " " << r (0, 1) << " " << r (0, 2) << " ";
    str << r (1, 0) << " " << r (1, 1) << " " << r (1, 2) << " ";
    str << r (2, 0) << " " << r (2, 1) << " " << r (2, 2);
    return createElement (idRotation3D (), std::string (str.str ()), doc);
}

DOMElem::Ptr DOMBasisTypes::createRotation2D (const Rotation2D<>& r, DOMElem::Ptr doc)
{
    std::ostringstream str;
    str.unsetf (std::ios::floatfield);    // floatfield not set
    str.precision (17);
    str << r (0, 0) << " " << r (0, 1) << " " << r (1, 0) << " " << r (1, 1);
    return createElement (idRotation2D (), std::string (str.str ()), doc);
}

DOMElem::Ptr DOMBasisTypes::createTransform3D (const Transform3D<>& t, DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idTransform3D ());
    createElement (idPos (), createStringFromArray (t.P ()), element);
    createRotation3D (t.R (), element);
    return element;
}

DOMElem::Ptr DOMBasisTypes::createTransform2D (const rw::math::Transform2D<>& t,
                                               rw::core::DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idTransform2D ());
    createElement (idPos (), createStringFromArray (t.P ()), element);
    createRotation2D (t.R (), element);
    return element;
}

DOMElem::Ptr DOMBasisTypes::createVelocityScrew6D (const VelocityScrew6D<>& v, DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idVelocityScrew6D ());
    createElement (idPos (), createStringFromArray (v.linear ()), element);
    createEAA (v.angular (), element);
    return element;
    // return createElement(VelocityScrew6DId, createStringFromArray(v, 6), doc);
}

DOMElem::Ptr DOMBasisTypes::createIntList (const std::vector< int >& ints, DOMElem::Ptr doc)
{
    return createElement (idIntList (), createStringFromVector (ints), doc);
}

DOMElem::Ptr DOMBasisTypes::createDoubleList (const std::vector< double >& doubles,
                                              DOMElem::Ptr doc)
{
    return createElement (idDoubleList (), createStringFromVector (doubles), doc);
}

DOMElem::Ptr DOMBasisTypes::createQState (const rw::kinematics::State& state, DOMElem::Ptr doc)
{
    return createElement (idQState (), createStringFromArray< State > (state, state.size ()), doc);
}

DOMElem::Ptr DOMBasisTypes::createBoolean (bool value, DOMElem::Ptr doc)
{
    return createElement (idBoolean (), boost::lexical_cast< std::string > (value), doc);
}

DOMElem::Ptr DOMBasisTypes::createDouble (double value, DOMElem::Ptr doc)
{
    return createElement (idDouble (), boost::lexical_cast< std::string > (value), doc);
}

DOMElem::Ptr DOMBasisTypes::createFloat (float value, DOMElem::Ptr doc)
{
    return createElement (idFloat (), boost::lexical_cast< std::string > (value), doc);
}

DOMElem::Ptr DOMBasisTypes::createInteger (int value, DOMElem::Ptr doc)
{
    return createElement (idInteger (), boost::lexical_cast< std::string > (value), doc);
}

DOMElem::Ptr DOMBasisTypes::createString (const std::string& str, DOMElem::Ptr doc)
{
    return createElement (idString (), str, doc);
}

DOMElem::Ptr DOMBasisTypes::createStringList (const std::vector< std::string >& strings,
                                              DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idStringList ());
    std::stringstream sstr;
    if (strings.size () > 0) {
        sstr << strings[0];
        for (size_t i = 1; i < strings.size (); i++) {
            sstr << ";" << strings[i];
        }
    }
    element->setValue (sstr.str ());
    return element;
}

DOMElem::Ptr DOMBasisTypes::createStringPair (const std::string& first, const std::string& second,
                                              DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idStringPair ());
    std::stringstream sstr;
    sstr << first << ";" << second;
    element->setValue (sstr.str ());
    return element;
}

DOMElem::Ptr DOMBasisTypes::createTreeState (const rw::kinematics::State& state, DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idTreeState ());

    const std::vector< Frame* >& dafs = state.getStateStructure ()->getDAFs ();

    // Find out what frames are DAFs.
    typedef std::vector< Frame* >::const_iterator I;
    for (I p = dafs.begin (); p != dafs.end (); ++p) {
        rw::core::Ptr<Frame> frame = *p;
        createStringPair (frame->getName (), frame->getDafParent (state)->getName (), element);
    }
    return element;
}

DOMElem::Ptr DOMBasisTypes::createState (const rw::kinematics::State& state, DOMElem::Ptr doc)
{
    DOMElem::Ptr element = doc->addChild (idState ());
    createQState (state, element);
    createTreeState (state, element);
    return element;
}
