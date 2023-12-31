%module sdurw_pathoptimization

%{
#include <rwlibs/swig/ScriptTypes.hpp>
#include <rw/core/Ptr.hpp>
#include <rw/models/Joint.hpp>
#include <rw/models/PrismaticSphericalJoint.hpp>
#include <rw/models/PrismaticUniversalJoint.hpp>
#include <rw/models/SphericalJoint.hpp>
#include <rw/models/UniversalJoint.hpp>
#include <rw/models/VirtualJoint.hpp>
#include <rw/models/DependentJoint.hpp>
#include <rw/models/DependentRevoluteJoint.hpp>
#include <rw/models/DependentPrismaticJoint.hpp>
#include <rw/models/RevoluteJoint.hpp>
#include <rw/models/PrismaticJoint.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/kinematics/FixedFrame.hpp>

#include <rw/geometry/IndexedTriMesh.hpp>

using namespace rwlibs::swig;
using rw::math::Metric;
using rw::trajectory::Path;
%}

%include <rwlibs/swig/swig_macros.i>
%include <exception.i>

%import <rwlibs/swig/sdurw_core.i>
%import <rwlibs/swig/sdurw_common.i>
%import <rwlibs/swig/sdurw_math.i>
%import <rwlibs/swig/sdurw_kinematics.i>
%import <rwlibs/swig/sdurw_models.i>
%import <rwlibs/swig/sdurw_proximity.i>
%import <rwlibs/swig/sdurw_pathplanning.i>

%pragma(java) jniclassimports=%{
import org.robwork.sdurw_core.*;
import org.robwork.sdurw_common.*;
import org.robwork.sdurw_math.*;
import org.robwork.sdurw_kinematics.*;
import org.robwork.sdurw_models.*;
import org.robwork.sdurw_proximity.*;
import org.robwork.sdurw_pathplanning.*;
%}
%pragma(java) moduleimports=%{
import org.robwork.sdurw_core.*;
import org.robwork.sdurw_common.*;
import org.robwork.sdurw_math.*;
import org.robwork.sdurw_kinematics.*;
import org.robwork.sdurw_models.*;
import org.robwork.sdurw_proximity.*;
import org.robwork.sdurw_pathplanning.*;
%}
%typemap(javaimports) SWIGTYPE %{
import org.robwork.sdurw_core.*;
import org.robwork.sdurw_common.*;
import org.robwork.sdurw_math.*;
import org.robwork.sdurw_kinematics.*;
import org.robwork.sdurw_models.*;
import org.robwork.sdurw_proximity.*;
import org.robwork.sdurw_pathplanning.*;
%}

%{
    #include <rw/math/MetricFactory.hpp>
%}

class PathLengthOptimizer
{
public:

    %extend {

        PathLengthOptimizer(rw::core::Ptr<rw::proximity::CollisionDetector> cd,
                            rw::core::Ptr<rw::models::Device> dev,
                            const rw::kinematics::State &state)
        {
            rw::pathplanning::PlannerConstraint constraint =
                    rw::pathplanning::PlannerConstraint::make(cd.get(), dev, state);
            return new PathLengthOptimizer(constraint, rw::math::MetricFactory::makeEuclidean< rw::math::Q >());
        }

        PathLengthOptimizer(rw::core::Ptr<rw::proximity::CollisionDetector> cd,
                            rw::core::Ptr<rw::models::Device> dev,
                            rw::core::Ptr< rw::math::Metric< rw::math::Q > > metric,
                            const rw::kinematics::State &state)
        {
            rw::pathplanning::PlannerConstraint constraint =
                    rw::pathplanning::PlannerConstraint::make(cd.get(), dev, state);
            return new PathLengthOptimizer(constraint, metric );
        }

        PathLengthOptimizer(rw::core::Ptr<rw::pathplanning::PlannerConstraint> constraint,
                            rw::core::Ptr< rw::math::Metric< rw::math::Q > > metric)
        {
            return new PathLengthOptimizer(*constraint, metric);
        }

        rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > pathPruning(rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > path){
            rw::trajectory::Path< rw::math::Q > res = $self->rwlibs::pathoptimization::PathLengthOptimizer::pathPruning(*path);
            return rw::core::ownedPtr( new rw::trajectory::Path< rw::math::Q >(res) );
        }
/*
        rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > shortCut(rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > path,
                                       size_t cnt,
                                       double time,
                                       double subDivideLength);
*/
        rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > shortCut(rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > path){
            rw::trajectory::Path< rw::math::Q > res = $self->rwlibs::pathoptimization::PathLengthOptimizer::shortCut(*path);
            return rw::core::ownedPtr( new rw::trajectory::Path< rw::math::Q >(res) );
        }

        rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > partialShortCut(rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > path){
            rw::trajectory::Path< rw::math::Q > res = $self->rwlibs::pathoptimization::PathLengthOptimizer::partialShortCut(*path);
            return rw::core::ownedPtr( new rw::trajectory::Path< rw::math::Q >(res) );
        }
/*
        rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > partialShortCut(rw::core::Ptr<rw::trajectory::Path< rw::math::Q > > path,
                                              size_t cnt,
                                              double time,
                                              double subDivideLength);
                                              */
    }
    rw::core::PropertyMap& getPropertyMap();

};