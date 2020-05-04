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

#include "ProximityStrategyFCL.hpp"

#include <rw/geometry/Geometry.hpp>
#include <rw/geometry/TriMesh.hpp>
#include <rw/math/Vector3D.hpp>
#include <rw/proximity/ProximityStrategyData.hpp>

#if FCL_VERSION_LESS_THEN_0_6_0
#include <fcl/BVH/BVH_model.h>
#include <fcl/collision.h>
#include <fcl/distance.h>
#else
#include <fcl/common/types.h>
#include <fcl/geometry/bvh/BVH_model.h>
#include <fcl/narrowphase/collision.h>
#include <fcl/narrowphase/distance.h>
#endif

using rw::core::ownedPtr;
using namespace rw::math;
using namespace rw::proximity;
using namespace rw::geometry;
using namespace rwlibs::proximitystrategies;

namespace {
#ifdef FCL_VERSION_LESS_THEN_0_6_0

fcl::Transform3f toFCL (const Transform3D<>& rwT)
{
    fcl::Matrix3f rotation (rwT (0, 0),
                            rwT (0, 1),
                            rwT (0, 2),
                            rwT (1, 0),
                            rwT (1, 1),
                            rwT (1, 2),
                            rwT (2, 0),
                            rwT (2, 1),
                            rwT (2, 2));
    fcl::Vec3f translation (rwT (0, 3), rwT (1, 3), rwT (2, 3));
    fcl::Transform3f fclT (rotation, translation);

    return fclT;
}
using rw_AABB    = fcl::AABB;
using rw_OBB     = fcl::OBB;
using rw_RSS     = fcl::RSS;
using rw_OBBRSS  = fcl::OBBRSS;
using rw_kIOS    = fcl::kIOS;
using rw_KDOP16  = fcl::KDOP< 16 >;
using rw_KDOP18  = fcl::KDOP< 18 >;
using rw_KDOP24  = fcl::KDOP< 24 >;

using fclContact = fcl::Contact;
#else
fcl::Transform3d toFCL (const Transform3D<>& rwT)
{
    return fcl::Transform3d (rwT.e ());
}
using rw_AABB    = fcl::AABB< double >;
using rw_OBB     = fcl::OBB< double >;
using rw_RSS     = fcl::RSS< double >;
using rw_OBBRSS  = fcl::OBBRSS< double >;
using rw_kIOS    = fcl::kIOS< double >;
using rw_KDOP16  = fcl::KDOP< double, 16 >;
using rw_KDOP18  = fcl::KDOP< double, 18 >;
using rw_KDOP24  = fcl::KDOP< double, 24 >;
using fclContact = fcl::Contact< double >;
#endif
}    // namespace

ProximityStrategyFCL::ProximityStrategyFCL (BV bv) :
    _bv (bv), _fclCollisionRequest (new fclCollisionRequest ()),
    _fclDistanceRequest (new fclDistanceRequest ()), _fclDistanceResult (new fclDistanceResult ())
{
    // Setup defaults
    _fclDistanceRequest->enable_nearest_points = true;
    _fclCollisionRequest->enable_contact       = true;
    _collectFCLResults                         = false;
}

ProximityStrategyFCL::~ProximityStrategyFCL ()
{
    delete _fclCollisionRequest;
    delete _fclDistanceRequest;
    delete _fclDistanceResult;
}

ProximityModel::Ptr ProximityStrategyFCL::createModel ()
{
    ProximityModel::Ptr p = ownedPtr (new FCLProximityModel (this));
    return p;
}

void ProximityStrategyFCL::destroyModel (ProximityModel* model)
{
    /* The created model was placed in a smart pointer
     * thus the memory deallocation is happening
     * automatically.
     */
}

bool ProximityStrategyFCL::addGeometry (ProximityModel* model, const Geometry& geom)
{
    switch (_bv) {
        case BV::AABB: return addGeometry< rw_AABB > (model, geom); break;
        case BV::OBB: return addGeometry< rw_OBB > (model, geom); break;
        case BV::RSS: return addGeometry< rw_RSS > (model, geom); break;
        case BV::OBBRSS: return addGeometry< rw_OBBRSS > (model, geom); break;
        case BV::kIOS: return addGeometry< rw_kIOS > (model, geom); break;
        case BV::KDOP16: return addGeometry< rw_KDOP16 > (model, geom); break;
        case BV::KDOP18: return addGeometry< rw_KDOP18 > (model, geom); break;
        case BV::KDOP24: return addGeometry< rw_KDOP24 > (model, geom); break;
        default:
            RW_THROW ("Implementation error! Support for the chosen FCL bounding volume has not "
                      "been properly implemented!");
            break;
    }

    return false;
}

bool ProximityStrategyFCL::addGeometry (rw::proximity::ProximityModel* model,
                                        rw::geometry::Geometry::Ptr geom, bool forceCopy)
{
    switch (_bv) {
        case BV::AABB: return addGeometry< rw_AABB > (model, geom, forceCopy); break;
        case BV::OBB: return addGeometry< rw_OBB > (model, geom, forceCopy); break;
        case BV::RSS: return addGeometry< rw_RSS > (model, geom, forceCopy); break;
        case BV::OBBRSS: return addGeometry< rw_OBBRSS > (model, geom, forceCopy); break;
        case BV::kIOS: return addGeometry< rw_kIOS > (model, geom, forceCopy); break;
        case BV::KDOP16: return addGeometry< rw_KDOP16 > (model, geom, forceCopy); break;
        case BV::KDOP18: return addGeometry< rw_KDOP18 > (model, geom, forceCopy); break;
        case BV::KDOP24: return addGeometry< rw_KDOP24 > (model, geom, forceCopy); break;
        default:
            RW_THROW ("Implementation error! Support for the chosen FCL bounding volume has not "
                      "been properly implemented!");
            break;
    }

    return false;
}

template< typename BV_t >
bool ProximityStrategyFCL::addGeometry (ProximityModel* model, const Geometry& geom)
{
    return addGeometry (model, ownedPtr (new Geometry (geom)), true);
}

template< typename BV_t >
bool ProximityStrategyFCL::addGeometry (rw::proximity::ProximityModel* model,
                                        rw::geometry::Geometry::Ptr geom, bool forceCopy)
{
    // The data is always copied in the called addGeometry function

    RW_ASSERT (model != 0);
    FCLProximityModel* pmodel = dynamic_cast< FCLProximityModel* > (model);

    /* Verify that the geometry has not already been added
     * - Could also implement a cache, like for PQP, and accept adding the same geometry by just
     * fetching it from the cache. */
    for (const auto& m : pmodel->models) {
        if (m.geo->getId () == geom->getId ()) {
            RW_THROW ("The specified geometry \"" + geom->getId () +
                      "\" (geometry identifiers are supposed to be unique) has "
                      "already been added to the FCL proximity strategy model!");
            return false;
        }
    }

    const double scale            = geom->getScale ();
    TriMesh::Ptr mesh             = geom->getGeometryData ()->getTriMesh (false);
    std::size_t numberOfTriangles = mesh->getSize ();
    /* There are 3 vertices in a triangle, thus a factor of 3 is used to specify the amount of data
     * to allocate */
    std::size_t numberOfVertices = numberOfTriangles * 3;

    rw::core::Ptr< fcl::BVHModel< BV_t > > fclBVHModel = ownedPtr (new fcl::BVHModel< BV_t >);

    int returnCode = 0;
    returnCode     = fclBVHModel->beginModel (numberOfTriangles, numberOfVertices);
    if (returnCode != fcl::BVH_OK) {
        /* error - do diagnosis on the error codes and throw a proper exception? */
        return false;
    }

    for (std::size_t triIndex = 0; triIndex < numberOfTriangles; ++triIndex) {
        const Triangle<> face  = mesh->getTriangle (triIndex);
        const Vector3D<> v0Tmp = face[0] * scale;
        const Vector3D<> v1Tmp = face[1] * scale;
        const Vector3D<> v2Tmp = face[2] * scale;

#ifdef FCL_VERSION_LESS_THEN_0_6_0
        fcl::Vec3f v0, v1, v2;
        for (std::size_t i = 0; i < 3; ++i) {
            v0[i] = v0Tmp[i];
            v1[i] = v1Tmp[i];
            v2[i] = v2Tmp[i];
        }
#else
        fcl::Vector3d v0 = v0Tmp.e ();
        fcl::Vector3d v1 = v1Tmp.e ();
        fcl::Vector3d v2 = v2Tmp.e ();
#endif
        /* mband todo:
         * - How to ensure that double is the type supported and that it can just be copied even
         * when SSE is enabled in the FCL library, thus causing it to be floats (or some other
         * aligned/optimised type layout)?
         */
        returnCode = fclBVHModel->addTriangle (v0, v1, v2);
        if (returnCode != fcl::BVH_OK) {
            /* error - do diagnosis on the error codes and throw a proper exception? */
            return false;
        }
    }

    returnCode = fclBVHModel->endModel ();
    if (returnCode != fcl::BVH_OK) {
        /* error - do diagnosis on the error codes and throw a proper exception? */
        return false;
    }

    /* Could use .scast to use static_cast instead of dynamic_cast and avoid the overhead caused by
     * runtime verification that the converted object is complete. */
    FCLBVHModelPtr fclBVHModelPtr = fclBVHModel.template cast< fclCollisionGeometry > ();
    if (!fclBVHModelPtr) {
        return false;
    }
    FCLModel fclModel (geom, geom->getTransform (), fclBVHModelPtr);
    pmodel->models.push_back (fclModel);
    return true;
}

bool ProximityStrategyFCL::removeGeometry (rw::proximity::ProximityModel* model,
                                           const std::string& geomId)
{
    RW_ASSERT (model != 0);
    FCLProximityModel* pmodel = dynamic_cast< FCLProximityModel* > (model);

    for (auto it = pmodel->models.begin (); it != pmodel->models.end (); ++it) {
        if ((*it).geo->getId () == geomId) {
            pmodel->models.erase (it);
            return true;
        }
    }

    /* Nothing was erased */
    return false;
}

std::vector< std::string >
ProximityStrategyFCL::getGeometryIDs (rw::proximity::ProximityModel* model)
{
    RW_ASSERT (model != 0);
    FCLProximityModel* pmodel = dynamic_cast< FCLProximityModel* > (model);

    std::vector< std::string > geometryIDs;
    geometryIDs.reserve (pmodel->models.size ());
    for (const auto& m : pmodel->models) {
        geometryIDs.push_back (m.geo->getId ());
    }

    return geometryIDs;
}
std::vector< rw::core::Ptr< rw::geometry::Geometry > >
ProximityStrategyFCL::getGeometrys (rw::proximity::ProximityModel* model)
{
    RW_ASSERT (model != 0);
    FCLProximityModel* pmodel = dynamic_cast< FCLProximityModel* > (model);

    std::vector< rw::core::Ptr< rw::geometry::Geometry > > geometrys;
    geometrys.reserve (pmodel->models.size ());
    for (const auto& m : pmodel->models) {
        geometrys.push_back (m.geo);
    }

    return geometrys;
}
void ProximityStrategyFCL::clear ()
{
    /* Assumes that all the ProximityModels are attached/connected to frames and thus being cleared
     */
    clearFrames ();
}

bool ProximityStrategyFCL::doInCollision (rw::proximity::ProximityModel::Ptr a,
                                          const rw::math::Transform3D<>& wTa,
                                          rw::proximity::ProximityModel::Ptr b,
                                          const rw::math::Transform3D<>& wTb,
                                          rw::proximity::ProximityStrategyData& data)
{
    RW_ASSERT (a != nullptr);
    RW_ASSERT (b != nullptr);

    bool inCollision = false;

    FCLProximityModel* aModel = dynamic_cast< FCLProximityModel* > (a.get ());
    FCLProximityModel* bModel = dynamic_cast< FCLProximityModel* > (b.get ());

    _fclCollisionResults.clear ();

    rw::proximity::CollisionResult& collisionResult = data.getCollisionData ();
    collisionResult.clear ();

    bool firstContact = false;
    switch (data.getCollisionQueryType ()) {
        case CollisionStrategy::FirstContact:
            firstContact                           = true;
            _fclCollisionRequest->num_max_contacts = 1;
            break;
        case CollisionStrategy::AllContacts:
            _fclCollisionRequest->num_max_contacts = std::numeric_limits< size_t >::max ();
            break;
        default:
            RW_THROW ("There is no implementation for the chosen CollisionStrategy type '"
                      << data.getCollisionQueryType () << "'!");
            break;
    }

    /* TODO: Could use the fcl::CollisionResult that is created in _fclCollisionResults, and avoid
     * having the cost of copying data into that collection */
    fclCollisionResult fclCollisionResult;

    size_t geoIdxA = 0;
    size_t geoIdxB = 0;
    for (const auto& ma : aModel->models) {
        for (const auto& mb : bModel->models) {
            fclCollisionResult.clear ();
            fcl::collide (ma.model.get (),
                          toFCL (wTa * ma.t3d),
                          mb.model.get (),
                          toFCL (wTb * mb.t3d),
                          *_fclCollisionRequest,
                          fclCollisionResult);

            if (fclCollisionResult.isCollision ()) {
                inCollision = true;

                collisionResult.a    = a;
                collisionResult.b    = b;
                collisionResult._aTb = inverse (wTa) * wTb;

                rw::proximity::CollisionResult::CollisionPair collisionPair;
                collisionPair.geoIdxA  = static_cast< int > (geoIdxA);
                collisionPair.geoIdxB  = static_cast< int > (geoIdxB);
                collisionPair.startIdx = static_cast< int > (collisionResult._geomPrimIds.size ());
                collisionPair.size     = fclCollisionResult.numContacts ();

                collisionResult._collisionPairs.push_back (collisionPair);

                for (size_t j = 0; j < fclCollisionResult.numContacts (); ++j) {
                    const fclContact& contact = fclCollisionResult.getContact (j);
                    collisionResult._geomPrimIds.push_back (
                        std::make_pair (contact.b1, contact.b2));
                }

                if (_collectFCLResults) {
                    _fclCollisionResults.push_back (fclCollisionResult);
                }

                if (firstContact) {
                    return inCollision;
                }
            }
            ++geoIdxB;
        }
        ++geoIdxA;
    }

    return inCollision;
}

void ProximityStrategyFCL::getCollisionContacts (
    std::vector< CollisionStrategy::Contact >& contacts, rw::proximity::ProximityStrategyData& data)
{
    RW_THROW ("The getCollisionContacts function is not implemented (yet)!");
}

rw::proximity::DistanceStrategy::Result& ProximityStrategyFCL::doDistance (
    rw::proximity::ProximityModel::Ptr a, const rw::math::Transform3D<>& wTa,
    rw::proximity::ProximityModel::Ptr b, const rw::math::Transform3D<>& wTb,
    class rw::proximity::ProximityStrategyData& data)
{
    RW_ASSERT (a != nullptr);
    RW_ASSERT (b != nullptr);

    FCLProximityModel* aModel = dynamic_cast< FCLProximityModel* > (a.get ());
    FCLProximityModel* bModel = dynamic_cast< FCLProximityModel* > (b.get ());

    _fclDistanceResult->clear ();

    rw::proximity::DistanceStrategy::Result& res = data.getDistanceData ();
    res.clear ();

    res.a = a;
    res.b = b;

    fclDistanceResult AfclDistanceResult;

    _fclDistanceRequest->abs_err = data.abs_err;
    _fclDistanceRequest->rel_err = data.rel_err;

    res.distance = std::numeric_limits< double >::max ();

    size_t geoIdxA = 0;
    size_t geoIdxB = 0;
    for (const auto& ma : aModel->models) {
        geoIdxB = 0;
        for (const auto& mb : bModel->models) {
            double minDistance = 0;
            /* mband TODO:
             * For some FCL Bounding Volumes (.e.g BV_OBB), when used for both collision objects,
             * there is no valid distance calculator (i.e. the following is output in cerr
             * (hardcoded from the fcl library)): "Warning: distance function between node type 2
             * and node type 2 is not supported"
             *
             *  ^---> How to properly report this issue/problem? Throw a patch upstream to make
             * their error reporting/handling more library like/tolerant (e.g. not writing to
             * std::cerr and giving some sort of error). Or make a patch that fixes it locally for
             * the FCL that is distributed together with RobWork?
             */
            minDistance = fcl::distance (ma.model.get (),
                                         toFCL (wTa * ma.t3d),
                                         mb.model.get (),
                                         toFCL (wTb * mb.t3d),
                                         *_fclDistanceRequest,
                                         AfclDistanceResult);
            RW_ASSERT (fabs (minDistance - AfclDistanceResult.min_distance) < 1.0e-16);

            // Only update data if a shorter distance has been found
            if (minDistance < res.distance) {
                res.distance = minDistance;

                res.geoIdxA = static_cast< int > (geoIdxA);
                res.geoIdxB = static_cast< int > (geoIdxB);
                res.idx1    = AfclDistanceResult.b1;
                res.idx2    = AfclDistanceResult.b2;

                if (_fclDistanceRequest->enable_nearest_points) {
                    for (size_t i = 0; i < 3; ++i) {
                        res.p1[i] = AfclDistanceResult.nearest_points[0][i];
                        res.p2[i] = AfclDistanceResult.nearest_points[1][i];
                    }
                }

                if (_collectFCLResults) {
                    *_fclDistanceResult = AfclDistanceResult;
                }
            }
            if (minDistance <= 0) {
                RW_LOG_DEBUG ("The objects are in collision! But still calculating a distance.");
            }
            ++geoIdxB;
        }
        ++geoIdxA;
    }

    return res;
}

fclCollisionRequest& ProximityStrategyFCL::getCollisionRequest ()
{
    return *_fclCollisionRequest;
}

fclDistanceRequest& ProximityStrategyFCL::getDistanceRequest ()
{
    return *_fclDistanceRequest;
}

fclCollisionResult& ProximityStrategyFCL::getCollisionResult (std::size_t index)
{
    /* Using .at(...) will throw an exception if index is out of range */
    return _fclCollisionResults.at (index);
}

fclDistanceResult& ProximityStrategyFCL::getDistanceResult ()
{
    return *_fclDistanceResult;
}

/*
 * Create ::make() functions that constructs and initialises this ProximityStrategy according to
 * some predefined / common use cases:
 * ::makeFastCollisionChecker <- no contact information just a simple query on whether a collision
 * occured
 * ::makeVerboseCollisionChecker <- Finds all the contact points etc.
 * ::makeSOMETHINGDistanceChecker
 */
