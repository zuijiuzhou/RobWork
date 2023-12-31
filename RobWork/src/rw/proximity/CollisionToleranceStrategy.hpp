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

#ifndef RW_PROXIMITY_COLLISIONTOLERANCESTRATEGY_HPP
#define RW_PROXIMITY_COLLISIONTOLERANCESTRATEGY_HPP

/**
 * @file CollisionToleranceStrategy.hpp
 */

#if !defined(SWIG)
#include <rw/proximity/ProximityStrategy.hpp>

#include <rw/core/ExtensionPoint.hpp>
#include <rw/core/Ptr.hpp>
#include <rw/math/Transform3D.hpp>

#include <string>
#endif

namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics

namespace rw { namespace proximity {

    /** @addtogroup proximity */
    /*@{*/

    /**
     * @brief This is a collision strategy that detects collisions between objects
     * that are closer than a specified tolerance.
     */
    class CollisionToleranceStrategy : public virtual ProximityStrategy
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< CollisionToleranceStrategy > Ptr;

        /**
         * @brief Destroys object
         */
        virtual ~CollisionToleranceStrategy ();

        /**
         * @brief Checks to see if the geometry attached to two given frames @f$ \mathcal{F}_a @f$
         * and
         * @f$ \mathcal{F}_b @f$ are closer than the specified tolerance.
         * @param a [in] @f$ \mathcal{F}_a @f$
         * @param wTa [in] @f$ \robabx{w}{a}{\mathbf{T}} @f$
         * @param b [in] @f$ \mathcal{F}_b @f$
         * @param wTb [in] @f$ \robabx{w}{b}{\mathbf{T}} @f$
         * @param tolerance [in] frames with a distance in between them
         * that is less than tolerance are in collision
         *
         * @return true if @f$ \mathcal{F}_a @f$ and @f$ \mathcal{F}_b @f$ are
         * colliding, false otherwise.
         */
        bool isWithinDistance (const rw::core::Ptr<kinematics::Frame> a, const math::Transform3D<double>& wTa,
                               const rw::core::Ptr<kinematics::Frame> b, const math::Transform3D<double>& wTb,
                               double tolerance);

        /**
         * @brief Checks to see if the geometry attached to two given frames @f$ \mathcal{F}_a @f$
         * and
         * @f$ \mathcal{F}_b @f$ are closer than the specified tolerance. Result is cached in data
         * @param a [in] @f$ \mathcal{F}_a @f$
         * @param wTa [in] @f$ \robabx{w}{a}{\mathbf{T}} @f$
         * @param b [in] @f$ \mathcal{F}_b @f$
         * @param wTb [in] @f$ \robabx{w}{b}{\mathbf{T}} @f$
         * @param data
         * @param distance
         *
         * @return true if @f$ \mathcal{F}_a @f$ and @f$ \mathcal{F}_b @f$ are
         * colliding, false otherwise.
         */
        bool isWithinDistance (const rw::core::Ptr<kinematics::Frame> a, const math::Transform3D<double>& wTa,
                               const rw::core::Ptr<kinematics::Frame> b, const math::Transform3D<double>& wTb,
                               double distance, class ProximityStrategyData& data);

        /**
         * @brief Checks to see if two proximity models @f$ \mathcal{F}_a @f$ and
         * @f$ \mathcal{F}_b @f$ are closer than the specified tolerance. Result is cached in data
         * @param a [in] @f$ \mathcal{F}_a @f$
         * @param wTa [in] @f$ \robabx{w}{a}{\mathbf{T}} @f$
         * @param b [in] @f$ \mathcal{F}_b @f$
         * @param wTb [in] @f$ \robabx{w}{b}{\mathbf{T}} @f$
         * @param tolerance [in] frames with a distance in between them
         * that is less than tolerance are in collision
         * @param data
         *
         * @return true if @f$ \mathcal{F}_a @f$ and @f$ \mathcal{F}_b @f$ are
         * colliding, false otherwise.
         */
        bool isWithinDistance (ProximityModel::Ptr a, const math::Transform3D<double>& wTa,
                               ProximityModel::Ptr b, const math::Transform3D<double>& wTb,
                               double tolerance, class ProximityStrategyData& data)
        {
            return doIsWithinDistance (a, wTa, b, wTb, tolerance, data);
        }

        /**
         * @addtogroup extensionpoints
         * @extensionpoint{rw::proximity::CollisionToleranceStrategy::Factory,rw::proximity::CollisionToleranceStrategy,rw.proximity.CollisionToleranceStrategy}
         */

        /**
         * @brief A factory for a CollisionToleranceStrategy. This factory also defines an
         * ExtensionPoint.
         *
         * Extensions providing a CollisionToleranceStrategy implementation can extend this factory
         * by registering the extension using the id "rw.proximity.CollisionToleranceStrategy".
         *
         * Typically one or more of the following CollisionToleranceStrategy types will be
         * available:
         *  - Bullet - rwlibs::proximitystrategies::ProximityStrategyBullet - Bullet Physics
         *  - PQP - rwlibs::proximitystrategies::ProximityStrategyPQP - Proximity Query Package
         */
        class Factory : public rw::core::ExtensionPoint< CollisionToleranceStrategy >
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
             * @return a pointer to a new CollisionToleranceStrategy.
             */
            static CollisionToleranceStrategy::Ptr makeStrategy (const std::string& strategy);

          private:
            Factory ();
        };

      protected:
        /**
         * @brief Checks to see if two proximity models @f$ \mathcal{F}_a @f$ and
         * @f$ \mathcal{F}_b @f$ are closer than the specified tolerance. Result is cached in data
         * @param a [in] @f$ \mathcal{F}_a @f$
         * @param wTa [in] @f$ \robabx{w}{a}{\mathbf{T}} @f$
         * @param b [in] @f$ \mathcal{F}_b @f$
         * @param wTb [in] @f$ \robabx{w}{b}{\mathbf{T}} @f$
         * @param tolerance [in] frames with a distance in between them
         * that is less than tolerance are in collision
         * @param data
         *
         * @return true if @f$ \mathcal{F}_a @f$ and @f$ \mathcal{F}_b @f$ are
         * colliding, false otherwise.
         */
        virtual bool doIsWithinDistance (ProximityModel::Ptr a, const math::Transform3D<double>& wTa,
                                         ProximityModel::Ptr b, const math::Transform3D<double>& wTb,
                                         double tolerance, class ProximityStrategyData& data) = 0;

      private:
        CollisionToleranceStrategy (const CollisionToleranceStrategy&);
        CollisionToleranceStrategy& operator= (const CollisionToleranceStrategy&);

      protected:
        /**
         * @brief Creates object
         */
        CollisionToleranceStrategy ();
    };

    /*@}*/
}}    // namespace rw::proximity

#endif    // end include guard
