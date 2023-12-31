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

#ifndef RW_MODELS_DEPENDENTPRISMATICJOINT_HPP
#define RW_MODELS_DEPENDENTPRISMATICJOINT_HPP

/**
 * @file DependentPrismaticJoint.hpp
 */
#if !defined(SWIG)
#include <rw/models/DependentJoint.hpp>
#include <rw/models/PrismaticJoint.hpp>
#endif 
namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics

namespace rw { namespace models {

    /** @addtogroup models */
    /*@{*/

    /**
     * @brief Dependent prismatic joint.
     *
     * DependentPrismaticJoint implements a prismatic joint for which the displacement
     * along the z-axis are linearly dependent on another joint
     */
    class DependentPrismaticJoint : public DependentJoint
    {
      public:
        /**
         * @brief A revolute joint with a displacement transform of \b transform.
         *
         * @param name [in] The name of the frame.
         *
         * @param transform [in] The displacement transform of the joint.
         *
         * @param owner [in] The joint controlling the dependent joint.
         *
         * @param scale [in] Scaling factor for the controlling joint value.
         *
         * @param offset [in] Offset for the controlling joint value.
         */
        DependentPrismaticJoint (const std::string& name, const rw::math::Transform3D<>& transform,
                                 rw::models::Joint* owner, double scale, double offset);

        /**
         * @brief The parent to frame transform for a revolute joint.
         *
         * The parent to frame transform is T * Tz(q) where:
         *
         * - T is the displacement transform of the joint;
         *
         * - q = q_owner * scale + offset is the joint value of the joint;
         *
         * - Tz(q) is the transform that translates a point an distance q in the
         * direction of the z-axis.
         *
         * @copydoc rw::kinematics::Frame::getTransform
         */
        rw::math::Transform3D<> getTransform (const rw::kinematics::State& state) const;

        /**
           @brief The joint controlling the passive revolute frame.
        */
        const rw::models::Joint& getOwner () const { return *_owner; }

        /**
           @brief The joint controlling the passive revolute frame.
        */
        rw::models::Joint& getOwner () { return *_owner; }

        /**
           @brief The scaling factor for the joint value of the controlling joint.
         */
        double getScale () const { return _scale; }

        /**
         * @brief get offset of this joint value in relation to controlling joint
         */
        double getOffset () const { return _offset; }

        //! @copydoc DependentJoint::isControlledBy
        bool isControlledBy (const rw::models::Joint* joint) const { return _owner == joint; }

        //! @copydoc rw::models::Joint::getJacobian
        void getJacobian (size_t row, size_t col, const rw::math::Transform3D<>& joint,
                          const rw::math::Transform3D<>& tcp, const rw::kinematics::State& state,
                          rw::math::Jacobian& jacobian) const;

        //! @copydoc rw::models::Joint::getFixedTransform()
        rw::math::Transform3D<> getFixedTransform () const { return _helper.getFixedTransform (); };

        //! @copydoc rw::models::Joint::setFixedTransform()
        void setFixedTransform (const rw::math::Transform3D<>& t3d);

        //! @copydoc rw::models::Joint::getJointTransform()
        rw::math::Transform3D<> getJointTransform (const rw::kinematics::State& state) const;
        //! @copydoc rw::models::Joint::setJointMapping()
        virtual void setJointMapping (rw::math::Function1Diff<>::Ptr function)
        {
            RW_THROW ("setJointMapping is not supported on VirtualJoint");
        }

        //! @copydoc rw::models::Joint::removeJointMapping()
        virtual void removeJointMapping () {}

      private:
        void doMultiplyTransform (const rw::math::Transform3D<>& parent,
                                  const rw::kinematics::State& state,
                                  rw::math::Transform3D<>& result) const;

        rw::math::Transform3D<> doGetTransform (const rw::kinematics::State& state) const;

        rw::math::Jacobian doGetJacobian (const rw::kinematics::State& state) const;

      private:
        PrismaticJoint _helper;
        rw::models::Joint* _owner;
        double _scale;
        double _offset;
    };

    /*@}*/
}}    // namespace rw::models

#endif    // end include guard
