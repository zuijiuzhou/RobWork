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

#ifndef RWLIBS_PROXIMITYSTRATEGIES_PROXIMITYSTRATEGYRW_HPP
#define RWLIBS_PROXIMITYSTRATEGIES_PROXIMITYSTRATEGYRW_HPP

/**
 * @file ProximityStrategyRW.hpp
 */
#if !defined(SWIG)
#include <rw/proximity/rwstrategy/BVTreeCollider.hpp>
#include <rw/proximity/rwstrategy/BinaryBVTree.hpp>

#include <rw/common/Cache.hpp>
#include <rw/core/Ptr.hpp>
#include <rw/geometry/OBBToleranceCollider.hpp>
#include <rw/proximity/CollisionStrategy.hpp>
#include <rw/proximity/ProximityCache.hpp>

#include <vector>
#endif 
namespace rw { namespace proximity {
    /** @addtogroup proximity */
    /*@{*/

    /**
     * @brief This is a strategy wrapper for the distance library
     * PQP (Proximity Query Package).
     *
     * PQP use Oriented Bounding Boxes (OBB) and hierarchical bounding trees for
     * fast distance calculation.
     *
     * For further information check out http://www.cs.unc.edu/~geom/SSV/
     */
    class ProximityStrategyRW : public rw::proximity::CollisionStrategy

    {
      public:
        typedef rw::core::Ptr< ProximityStrategyRW > Ptr;

        //! @brief cache key
        typedef std::pair< std::string, double > CacheKey;

        //! @brief cache for any of the queries possible on this strategy
        struct PCache : public rw::proximity::ProximityCache
        {
            PCache (void* owner) : ProximityCache (owner), tolCollider (nullptr) {}
            virtual size_t size () const { return 0; }
            virtual void clear () {}

            // TODO: reuse stuff from the collision test
            rw::core::Ptr< rw::proximity::BVTreeCollider< rw::proximity::BinaryOBBPtrTreeD > >
                tcollider;
            rw::core::Ptr< rw::proximity::BVTreeCollider< rw::proximity::BinaryOBBPtrTreeD > >
                tolcollider;
            rw::geometry::OBBToleranceCollider<>* tolCollider;
        };



        //! @brief

        struct Model
        {
            typedef rw::core::Ptr< Model > Ptr;

            Model (rw::core::Ptr< rw::geometry::Geometry > geo, rw::math::Transform3D<> trans,
                   rw::proximity::BinaryOBBPtrTreeD::Ptr obbtree) :
                geo (geo),
                scale (1), t3d (trans), tree (obbtree)
            {}

            rw::core::Ptr< rw::geometry::Geometry > geo;
            double scale;
            rw::math::Transform3D<> t3d;
            rw::proximity::BinaryOBBPtrTreeD::Ptr tree;
            CacheKey ckey;
        };

        // typedef std::vector<RWPQPModel> RWPQPModelList;
        // typedef std::pair<RWPQPModel, RWPQPModel> RWPQPModelPair;
        struct RWProximityModel : public rw::proximity::ProximityModel
        {
            RWProximityModel (ProximityStrategy* owner) : ProximityModel (owner) {}
            std::vector< Model::Ptr > models;
        };

      private:
        rw::common::Cache< CacheKey, Model > _modelCache;

      public:
        /**
         * @brief Constructor
         */
        ProximityStrategyRW ();

        //// interface of ProximityStrategy

        /**
         * @copydoc rw::proximity::ProximityStrategy::createModel
         */
        virtual rw::proximity::ProximityModel::Ptr createModel ();

        /**
         * @copydoc rw::proximity::ProximityStrategy::destroyModel
         */
        void destroyModel (rw::proximity::ProximityModel* model);

        /**
         * @copydoc rw::proximity::ProximityStrategy::addGeometry
         */
        bool addGeometry (rw::proximity::ProximityModel* model, const rw::geometry::Geometry& geom);

        bool addGeometry (rw::proximity::ProximityModel* model,
                          rw::core::Ptr< rw::geometry::Geometry > geom, bool);

        /**
         * @copydoc rw::proximity::ProximityStrategy::removeGeometry
         */
        bool removeGeometry (rw::proximity::ProximityModel* model, const std::string& geomId);

        /**
         * @copydoc rw::proximity::ProximityStrategy::getGeometryIDs
         */
        std::vector< std::string > getGeometryIDs (rw::proximity::ProximityModel* model);

        /**
         * @copydoc rw::proximity::ProximityStrategy::getGeometrys
         */
        std::vector< rw::core::Ptr< rw::geometry::Geometry > >
        getGeometrys (rw::proximity::ProximityModel* model);

        /**
         * @copydoc rw::proximity::CollisionStrategy::doInCollision
         */
        bool doInCollision (rw::proximity::ProximityModel::Ptr a,
                            const rw::math::Transform3D<>& wTa,
                            rw::proximity::ProximityModel::Ptr b,
                            const rw::math::Transform3D<>& wTb,
                            rw::proximity::ProximityStrategyData& data);

        /**
         *  @copydoc rw::proximity::ProximityStrategy::clear
         */
        void clear ();

        /**
         * @brief returns the number of bounding volume tests performed
         * since the last call to clearStats
         */
        int getNrOfBVTests () { return _numBVTests; }

        /**
         * @brief returns the number of ptriangle tests performed
         * since the last call to clearStats
         */
        int getNrOfTriTests () { return _numTriTests; }

        /**
         * @brief clears the bounding volume and triangle test counters.
         */
        void clearStats ()
        {
            _numBVTests  = 0;
            _numTriTests = 0;
        }

        void getCollisionContacts (std::vector< rw::proximity::CollisionStrategy::Contact >& contacts,
                                   ProximityStrategyData& data);

      private:
        struct QueryData
        {
            PCache* cache;
            RWProximityModel *a, *b;
        };

        QueryData initQuery (rw::proximity::ProximityModel::Ptr& aModel,
                             rw::proximity::ProximityModel::Ptr& bModel,
                             rw::proximity::ProximityStrategyData& data);

      private:
        int _numBVTests, _numTriTests;

        rw::proximity::BVTreeCollider< rw::proximity::BinaryOBBPtrTreeD >::Ptr _tcollider;
        std::vector< Model::Ptr > _allModels;
    };

}}    // namespace rw::proximity

#endif    // end include guard
