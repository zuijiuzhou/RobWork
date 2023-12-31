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

#ifndef RW_MODELS_VIRTUALJOINT_HPP
#define RW_MODELS_VIRTUALJOINT_HPP

/**
 * @file VirtualJoint.hpp
 */
#if !defined(SWIG)
#include <rw/models/Joint.hpp>
#endif 
namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics

namespace rw { namespace models {

    /** @addtogroup models */
    /*@{*/

    /**
       @brief Virtuals joints.

       VirtualJoint is a joint with a role similar to a rw::kinematics::FixedFrame with
           an optional number of dof allocated in the state.

       Virtual joints are useful when you want a store joint values of e.g.
           a number of passive joints.
     */
    class VirtualJoint : public Joint
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< VirtualJoint > Ptr;

        /**
         * @brief A virtual joint with a displacement transform of \b transform.
         * @param name [in] The name of the frame.
         * @param transform [in] The displacement transform of the joint.
         * @param dof [in] Number of degrees of freedom of the joint
         */
        VirtualJoint (const std::string& name, const rw::math::Transform3D<>& transform, size_t dof);

        //! @copydoc Joint::getJacobian
        void getJacobian (size_t row, size_t col, const rw::math::Transform3D<>& joint,
                          const rw::math::Transform3D<>& tcp, const rw::kinematics::State& state,
                          rw::math::Jacobian& jacobian) const
        {}

        //! @copydoc Joint::getFixedTransform()
        rw::math::Transform3D<> getFixedTransform () const { return _transform; };

        //! @copydoc Joint::getFixedTransform()
        void setFixedTransform (const rw::math::Transform3D<>& t3d);

        //! @copydoc Joint::getJointTransform()
        rw::math::Transform3D<> getJointTransform (const rw::kinematics::State& state) const;

        //! @copydoc Joint::setJointMapping()
        virtual void setJointMapping (rw::math::Function1Diff<>::Ptr function)
        {
            RW_THROW ("setJointMapping is not supported on VirtualJoint");
        }

        //! @copydoc Joint::removeJointMapping()
        virtual void removeJointMapping () {}

      protected:
        rw::math::Transform3D<> doGetTransform (const rw::kinematics::State& state) const;

        void doMultiplyTransform (const rw::math::Transform3D<>& parent, const rw::kinematics::State& state,
                                  rw::math::Transform3D<>& result) const;

      private:
        rw::math::Transform3D<> _transform;
    };

    /*@}*/
}}    // namespace rw::models

#endif    // end include guard
