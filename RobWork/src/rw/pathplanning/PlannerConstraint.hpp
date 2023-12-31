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

#ifndef RW_PATHPLANNING_PLANNERCONSTRAINT_HPP
#define RW_PATHPLANNING_PLANNERCONSTRAINT_HPP

/**
   @file PlannerConstraint.hpp
*/
#if !defined(SWIG)
#include <rw/pathplanning/QConstraint.hpp>
#include <rw/pathplanning/QEdgeConstraint.hpp>
#endif
namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics
namespace rw { namespace models {
    class Device;
}}    // namespace rw::models
namespace rw { namespace models {
    class WorkCell;
}}    // namespace rw::models
namespace rw { namespace proximity {
    class CollisionDetector;
}}    // namespace rw::proximity
namespace rw { namespace proximity {
    class CollisionSetup;
}}    // namespace rw::proximity
namespace rw { namespace proximity {
    class CollisionStrategy;
}}    // namespace rw::proximity

namespace rw { namespace pathplanning {

    /** @addtogroup pathplanning */
    /** @{*/

    /**
       @brief A tuple of (QConstraintPtr, QEdgeConstraintPtr).

       A planner constraint is a small copyable object containing pointers to a
       configuration constraint and an edge constraint. Sampling based path
       planners and path optimizers typically use a PlannerConstraint object for
       the collision checking for the paths.

       A number of make() utility constructors are provided for applications
       where defaults for configuration and edge constraints can be used.
    */
    class PlannerConstraint
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< PlannerConstraint > Ptr;
        //! @brief smart pointer type to this const class
        typedef rw::core::Ptr< const PlannerConstraint > CPtr;

        /**
         * @brief Default constructed without constraints initialized
         */
        PlannerConstraint ();

        /**
           @brief A (QConstraintPtr, QEdgeConstraintPtr) tuple.

           The constraints must be non-null.
        */
        PlannerConstraint (rw::core::Ptr<rw::pathplanning::QConstraint> constraint, QEdgeConstraint::Ptr edge);

        /**
         * @brief Forwards call to the QConstraint wrapped by the PlannerConstraint
         */
        bool inCollision (const rw::math::Q& q);

        /**
         * @brief Forwards call to the QEdgeConstraint wrapped by the PlannerConstraint
         */
        bool inCollision (const rw::math::Q& q1, const rw::math::Q& q2);

        /**
           @brief The configuration constraint.
        */
        QConstraint& getQConstraint () const { return *_constraint; }

        /**
           @brief The edge constraint.
        */
        QEdgeConstraint& getQEdgeConstraint () const { return *_edge; }

        /**
           @brief The configuration constraint pointer.
        */
        const rw::core::Ptr<rw::pathplanning::QConstraint>& getQConstraintPtr () const { return _constraint; }

        /**
           @brief The edge constraint pointer.
        */
        const QEdgeConstraint::Ptr& getQEdgeConstraintPtr () const { return _edge; }

        /**
           @brief A (QConstraintPtr, QEdgeConstraintPtr) tuple.

           This is equivalent to the standard constructor.
        */
        static PlannerConstraint make (rw::core::Ptr<rw::pathplanning::QConstraint> constraint, QEdgeConstraint::Ptr edge);

        /**
           @brief Planner constraint for a collision detector.

           Path are checked discretely for a default device dependent
           resolution.
        */
        static PlannerConstraint make (rw::core::Ptr< rw::proximity::CollisionDetector > detector,
                                       rw::core::Ptr< const rw::models::Device > device,
                                       const rw::kinematics::State& state);

        /**
   @brief Planner constraint for a collision strategy.

   Path are checked discretely for a default device dependent
   resolution.

   The default collision setup of the workcell is used.
*/
        static PlannerConstraint make (rw::core::Ptr< rw::proximity::CollisionStrategy > strategy,
                                       rw::core::Ptr< rw::models::WorkCell > workcell,
                                       rw::core::Ptr< const rw::models::Device > device,
                                       const rw::kinematics::State& state);

        /**
           @brief Planner constraint for a collision strategy and collision
           setup.

           Path are checked discretely for a default device dependent
           resolution.
        */
        static PlannerConstraint make (rw::core::Ptr< rw::proximity::CollisionStrategy > strategy,
                                       const rw::proximity::CollisionSetup& setup,
                                       rw::core::Ptr< rw::models::WorkCell > workcell,
                                       rw::core::Ptr< const rw::models::Device > device,
                                       const rw::kinematics::State& state);

      private:
        rw::core::Ptr<rw::pathplanning::QConstraint> _constraint;
        QEdgeConstraint::Ptr _edge;
    };

    /* @} */
}}    // namespace rw::pathplanning

#endif    // end include guard
