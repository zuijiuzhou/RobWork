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

#ifndef RW_PROXIMITY_DISTANCEMULTISTRATEGY_HPP
#define RW_PROXIMITY_DISTANCEMULTISTRATEGY_HPP
/**
 * @file DistanceMultiStrategy.hpp
 */

#if !defined(SWIG)
#include <rw/proximity/ProximityStrategy.hpp>

#include <rw/core/ExtensionPoint.hpp>
#include <rw/math/Transform3D.hpp>
#endif
namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics

namespace rw { namespace proximity {

    /** @addtogroup proximity */
    /*@{*/

    /**
     * @brief This interface allows the definition of computing all points between two
     * geometric objects that are closer than a specified tolerance. See ProxmityStrategy on
     * how to add geometry to the strategy.
     */
    class DistanceMultiStrategy : public virtual ProximityStrategy
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< DistanceMultiStrategy > Ptr;

        /**
         * @brief DistanceResult contains basic information about the distance
         * result between two frames.
         */
        struct Result
        {
            //! @brief reference to the first proximity model
            ProximityModel::Ptr a;

            //! @brief reference to the second proximity model
            ProximityModel::Ptr b;

            //! Closest point on f1 to f2, described in world reference frame
            math::Vector3D< double > p1;

            //! Closest point on f2 to f1, described in world reference frame
            math::Vector3D< double > p2;

            //! @brief distance between frame f1 and frame f2
            double distance;

            //! Closest points on f1 to f2, described in world reference frame
            std::vector< math::Vector3D<double> > p1s;

            /**
             * @brief Closest point on f2 to f1, described in world reference frame
             */
            std::vector< math::Vector3D<double> > p2s;

            //! @brief geometry index to geometry in object A
            std::vector< int > geoIdxA;

            //! @brief geometry index to geometry in object B
            std::vector< int > geoIdxB;

            /**
             * @brief indices to the primitives which are the closest points on the first proximity
             *model
             **/
            std::vector< int > p1prims;

            /**
             * @brief indices to the primitives which are the closest points on the second proximity
             *model
             **/
            std::vector< int > p2prims;

            //! distances between contact points
            std::vector< double > distances;

            void clear ()
            {
                a  = NULL;
                b  = NULL;
                p1 = rw::math::Vector3D< double > (0, 0, 0);
                p2 = rw::math::Vector3D< double > (0, 0, 0);
                p1s.clear ();
                p2s.clear ();
                p1prims.clear ();
                p2prims.clear ();
                distances.clear ();
            }
        };

        /**
         * @brief Destroys object
         */
        virtual ~DistanceMultiStrategy ();

        /**
         * @brief Calculates all distances between geometry of two given frames @f$ \mathcal{F}_a
         * @f$ and
         * @f$ \mathcal{F}_b @f$
         * @param a [in] @f$ \mathcal{F}_a @f$
         * @param wTa [in] @f$ \robabx{w}{a}{\mathbf{T}} @f$
         * @param b [in] @f$ \mathcal{F}_b @f$
         * @param wTb [in] @f$ \robabx{w}{b}{\mathbf{T}} @f$
         * @param tolerance [in] point pairs that are closer than tolerance will
         * be included in the result.
         *
         * @return shortest distance if @f$ \mathcal{F}_a @f$ and @f$ \mathcal{F}_b @f$ are
         * separated and not in collision.
         */
        Result distances (const rw::core::Ptr<kinematics::Frame> a, const math::Transform3D<double>& wTa,
                          const rw::core::Ptr<kinematics::Frame> b, const math::Transform3D<double>& wTb,
                          double tolerance);

        /**
         * @brief Calculates all distances between geometry of two  given frames @f$ \mathcal{F}_a
         * @f$ and
         * @f$ \mathcal{F}_b @f$
         * @param a [in] @f$ \mathcal{F}_a @f$
         * @param wTa [in] @f$ \robabx{w}{a}{\mathbf{T}} @f$
         * @param b [in] @f$ \mathcal{F}_b @f$
         * @param wTb [in] @f$ \robabx{w}{b}{\mathbf{T}} @f$
         * @param tolerance [in] point pairs that are closer than tolerance will be included in the
         * result.
         * @param data
         *
         * @return shortest distance if @f$ \mathcal{F}_a @f$ and @f$ \mathcal{F}_b @f$ are
         * separated and not in collision.
         */
        Result& distances (const rw::core::Ptr<kinematics::Frame> a, const math::Transform3D<double>& wTa,
                           const rw::core::Ptr<kinematics::Frame> b, const math::Transform3D<double>& wTb,
                           double tolerance, class ProximityStrategyData& data);

        /**
         * @copydoc doDistances
         */
        Result& distances (ProximityModel::Ptr a, const math::Transform3D<double>& wTa,
                           ProximityModel::Ptr b, const math::Transform3D<double>& wTb, double tolerance,
                           class ProximityStrategyData& data)
        {
            return doDistances (a, wTa, b, wTb, tolerance, data);
        }

        /**
         * @addtogroup extensionpoints
         * @extensionpoint{rw::proximity::DistanceMultiStrategy::Factory,rw::proximity::DistanceMultiStrategy,rw.proximity.DistanceMultiStrategy}
         */

        /**
         * @brief A factory for a DistanceMultiStrategy. This factory also defines an
         * ExtensionPoint.
         *
         * Extensions providing a DistanceMultiStrategy implementation can extend this factory by
         * registering the extension using the id "rw.proximity.DistanceMultiStrategy".
         *
         * Typically one or more of the following DistanceMultiStrategy types will be available:
         *  - Bullet - rwlibs::proximitystrategies::ProximityStrategyBullet - Bullet Physics
         *  - PQP - rwlibs::proximitystrategies::ProximityStrategyPQP - Proximity Query Package
         */
        class Factory : public rw::core::ExtensionPoint< DistanceMultiStrategy >
        {
          public:
            /**
             * @brief Get the available strategies.
             * @return a vector of identifiers for strategies.
             */
            static std::vector< std::string > getStrategies ();

            /**
             * @brief Check if strategy is available.
             * @param strategy [in] the name of the strategy.
             * @return true if available, false otherwise.
             */
            static bool hasStrategy (const std::string& strategy);

            /**
             * @brief Create a new strategy.
             * @param strategy [in] the name of the strategy.
             * @return a pointer to a new DistanceMultiStrategy.
             */
            static DistanceMultiStrategy::Ptr makeStrategy (const std::string& strategy);

          private:
            Factory ();
        };

      protected:
        /**
         * @brief Calculates all distances between two given objects @f$ \mathcal{F}_a @f$ and
         * @f$ \mathcal{F}_b @f$ where the distances are below a certain threshold.
         * @param a [in] @f$ \mathcal{F}_a @f$
         * @param wTa [in] @f$ \robabx{w}{a}{\mathbf{T}} @f$
         * @param b [in] @f$ \mathcal{F}_b @f$
         * @param wTb [in] @f$ \robabx{w}{b}{\mathbf{T}} @f$
         * @param tolerance [in] point pairs that are closer than tolerance will be included in the
         * result.
         * @param data
         *
         * @return list of distances between all points that are closer than threshold.
         */
        virtual Result& doDistances (ProximityModel::Ptr a, const math::Transform3D<double>& wTa,
                                     ProximityModel::Ptr b, const math::Transform3D<double>& wTb,
                                     double tolerance, class ProximityStrategyData& data) = 0;

      private:
        DistanceMultiStrategy (const DistanceMultiStrategy&);
        DistanceMultiStrategy& operator= (const DistanceMultiStrategy&);

      protected:
        /**
         * @brief Creates object
         */
        DistanceMultiStrategy ();
    };

    /*@}*/
}}    // namespace rw::proximity

#endif /* RW_PROXIMITY_DISTANCEMULTISTRATEGY_HPP */
