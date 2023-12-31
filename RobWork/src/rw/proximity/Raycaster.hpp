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

#ifndef RW_PROXIMITY_RAYCASTER_HPP_
#define RW_PROXIMITY_RAYCASTER_HPP_

#if !defined(SWIG)
#include <rw/proximity/ProximityModel.hpp>
#include <rw/proximity/ProximityStrategyData.hpp>

#include <rw/core/Ptr.hpp>
#include <rw/geometry/PlainTriMesh.hpp>
#include <rw/math/Vector3D.hpp>
#endif

namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics
namespace rw { namespace models {
    class WorkCell;
}}    // namespace rw::models

namespace rw { namespace proximity {
    class CollisionStrategy;

    /**
     * @brief a raycast implementation that relies on a collision strategy for finding the
     * collision between the ray and the scene.
     */
    class Raycaster
    {
      public:
        //! result structure of ray cast
        typedef enum { CLOSEST_HIT, ALL_HITS } QueryType;

        /**
         * @brief result of a Raycast query. All contact information are described in ray coordinate
         * frame.
         */
        struct QueryResult
        {
            QueryType qtype;

            // closest contact point and normal (if normals enabled)
            rw::math::Vector3D<double> point;
            rw::math::Vector3D<double> normal;

            // all contact points and normals (if normals enabled)
            std::vector< rw::math::Vector3D<double> > points;
            std::vector< rw::math::Vector3D<double> > normals;

            // the models that where in contact and an associated int that indexes into the
            // starting of points,normals for the object.
            std::vector< std::pair< rw::proximity::ProximityModel::Ptr, int > > models;

            rw::proximity::ProximityStrategyData data;
        };

      public:
#if !defined(SWIG)
        // TODO NOT IMPLEMENTED
        /**
         * @brief constructor - only the frames in the vector are tested against each
         * other.
         */
        Raycaster (double ray_length = 100.0);
#endif
        /**
         * @brief constructor - only the frames in the vector are tested against each
         * other.
         */
        Raycaster (rw::proximity::CollisionStrategy::Ptr cdstrategy, double ray_length = 100.0);

        //! @brief destructor
        virtual ~Raycaster ();

        void setRayFrame (rw::core::Ptr< rw::kinematics::Frame > rayframe) { _rayFrame = rayframe; }

#if !defined(SWIG)
        // TODO THESE FUNCTIONS ARE NOT IMPLEMENTED
        void add (rw::core::Ptr< rw::geometry::Geometry > geom);

        void add (rw::core::Ptr< rw::models::Object > object);

        void add (rw::core::Ptr< rw::models::WorkCell > wc);
#endif

        /**
         * @brief shoots a ray in the direction of the vector \b direction starting from
         * the position vector \b pos. The frame associated with the geometry that is hit
         * first by the ray is returned along with the intersection point described in
         * world frame.
         */
        bool shoot (const rw::math::Vector3D< double >& pos,
                    const rw::math::Vector3D< double >& direction, QueryResult& result,
                    const rw::kinematics::State& state);

        /**
         * @brief set to true if normals should also be calculated
         * @param enabled
         */
        void setCalculateNormals (bool enabled) { _calculateNormals = enabled; };

      private:
        bool _calculateNormals;

        // rw::proximity::CollisionDetectorPtr _detector;
        std::vector< rw::kinematics::Frame* > _frames;
        rw::core::Ptr< rw::proximity::CollisionStrategy > _cdstrategy;
        rw::geometry::PlainTriMeshF::Ptr _ray;
        rw::core::Ptr< rw::kinematics::Frame > _rayFrame;

        rw::proximity::ProximityModel::Ptr _rayModel;

        std::vector< rw::proximity::ProximityModel::Ptr > _obstacles;

        QueryType _queryType;
    };

}}    // namespace rw::proximity

#endif /* RW_PROXIMITY_RAYCASTER_HPP_ */
