/*
 * PlaceBody.hpp
 *
 *  Created on: 19/05/2011
 *      Author: jimali
 */

#ifndef RWSIM_DYNAMICS_BODYUTIL_HPP_
#define RWSIM_DYNAMICS_BODYUTIL_HPP_

#include "Body.hpp"

#include <rw/core/Ptr.hpp>
#include <rw/math/Transform3D.hpp>
#include <rw/math/Vector3D.hpp>

namespace rw { namespace proximity {
    class CollisionDetector;
}}    // namespace rw::proximity
namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics

namespace rwsim { namespace dynamics {
    class DynamicWorkCell;

    //! @brief Utility functions related to dynamic bodies.
    class BodyUtil
    {
      public:
        /**
         * @brief a utility function for calculating the transformation of a body if
         * it is kinematically projected in the direction of \b dir. Where dir is described
         * in world coordinates.
         * @param body [in] the body to project.
         * @param coldect [in] collision detector to use for detection of contact.
         * @param state [in] the initial state.
         * @param dir [in] the direction to project.
         * @return the transform.
         */
        static rw::math::Transform3D<>
        placeBody (rwsim::dynamics::Body::Ptr body,
                   rw::core::Ptr< rw::proximity::CollisionDetector > coldect,
                   const rw::kinematics::State& state,
                   const rw::math::Vector3D<>& dir = -rw::math::Vector3D<>::z ());

        /**
         * @brief Find the parent body of a body.
         * @param child [in] the child body.
         * @param dwc [in] the dynamic workcell.
         * @param state [in] the current state.
         * @return the parent body, or NULL if not found.
         */
        static Body::Ptr getParentBody (Body::Ptr child, rw::core::Ptr< DynamicWorkCell > dwc,
                                        const rw::kinematics::State& state);

        /**
         * @brief Find the parent body of a frame.
         * @param child [in] the child frame.
         * @param dwc [in] the dynamic workcell.
         * @param state [in] the current state.
         * @return the parent body, or NULL if not found.
         */
        static Body::Ptr getParentBody (rw::core::Ptr<rw::kinematics::Frame> child,
                                        rw::core::Ptr< DynamicWorkCell > dwc,
                                        const rw::kinematics::State& state);
    };

}}     // namespace rwsim::dynamics
#endif /* RWSIM_DYNAMICS_BODYUTIL_HPP_ */
