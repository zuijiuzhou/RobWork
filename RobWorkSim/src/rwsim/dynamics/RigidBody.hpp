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

#ifndef RWSIM_DYNAMICS_RIGIDBODY_HPP_
#define RWSIM_DYNAMICS_RIGIDBODY_HPP_

#include "Body.hpp"

#include <rw/core/Ptr.hpp>
#include <rw/kinematics/Frame.hpp>
#include <rw/kinematics/Kinematics.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/math/InertiaMatrix.hpp>
#include <rw/math/Vector3D.hpp>

namespace rwsim { namespace dynamics {
    //! @addtogroup rwsim_dynamics
    //! @{
    /**
     * @brief A body is an abstraction over some physical object in the scene.
     * The body class is an abstract class that allows interaction by adding gravity,
     * forces and impulses. The integration scheme used to update the velocity and position
     * of the body is defined by the class that inherit the body interface.
     *
     *
     * The rigid body hold 12 state variables in the StateStructure beside the 6 allocated by
     * MovableFrame (for position). the first 6 state variables are for velocity(linear,angular),
     * and the last six are for force/torque accumulation (force,torque)
     */
    class RigidBody : public Body
    {
      public:
        //! @brief Smart pointer type for a RigidBody.
        typedef rw::core::Ptr< RigidBody > Ptr;

        //! @copydoc dynamics::Body::Body()
        RigidBody (const BodyInfo& info, rw::models::Object::Ptr obj);

        //! @brief Destructor.
        virtual ~RigidBody (){};

      public:    // functions that need to be implemented by specialized class
        /**
         * @copydoc dynamics::Body::getPointVelW
         */
        rw::math::Vector3D<> getPointVelW (const rw::math::Vector3D<>& p,
                                           const rw::kinematics::State& state) const;

        //! @copydoc dynamics::Body::getVelocity
        rw::math::VelocityScrew6D<> getVelocity (const rw::kinematics::State& state) const;
        /**
         * @copydoc dynamics::Body::reset
         */
        void reset (rw::kinematics::State& state);

        /**
         * @copydoc dynamics::Body::calcEnergy
         * @note RigidBody energy is calculated as
         * \f$ \frac12 m \mathbf{v}^2 + \frac12 \mathbf{I} \mathbf{w} \cdot \mathbf{w} - m
         * \mathbf{g} \cdot (\mathbf{p}-\mathbf{p_0}) \f$
         */
        double
        calcEnergy (const rw::kinematics::State& state,
                    const rw::math::Vector3D<>& gravity = rw::math::Vector3D<>::zero (),
                    const rw::math::Vector3D<>& potZero = rw::math::Vector3D<>::zero ()) const;

        //! @copydoc dynamics::Body::setForce
        void setForce (const rw::math::Vector3D<>& f, rw::kinematics::State& state)
        {
            _rstate.get (state).force = f;
        }

        //! @copydoc dynamics::Body::addForce
        void addForce (const rw::math::Vector3D<>& force, rw::kinematics::State& state)
        {
            _rstate.get (state).force += force;
        }

        //! @copydoc dynamics::Body::getForce
        rw::math::Vector3D<> getForce (const rw::kinematics::State& state) const
        {
            return _rstate.get (state).force;
        }

        //! @copydoc dynamics::Body::setTorque
        void setTorque (const rw::math::Vector3D<>& t, rw::kinematics::State& state)
        {
            _rstate.get (state).torque = t;
        }

        //! @copydoc dynamics::Body::addTorque
        void addTorque (const rw::math::Vector3D<>& t, rw::kinematics::State& state)
        {
            _rstate.get (state).torque += t;
        }

        //! @copydoc dynamics::Body::getTorque
        rw::math::Vector3D<> getTorque (const rw::kinematics::State& state) const
        {
            return _rstate.get (state).torque;
        }

      public:

        /**
         * @brief return the parent frame
         */
        rw::kinematics::Frame* getParent (const rw::kinematics::State& state) const
        {
            return _mframe->getParent (state);
        }

        /**
         * @brief returns the transform from parent to body
         */
        rw::math::Transform3D<> getPTBody (const rw::kinematics::State& state) const
        {
            return _mframe->getTransform (state);
        }

        /**
         * @brief set the transform of the body. The transform is described relative to parent.
         */
        void setPTBody (const rw::math::Transform3D<>& pTb, rw::kinematics::State& state)
        {
            _mframe->setTransform (pTb, state);
        }

        /**
         * @brief returns the transform from world to body
         */
        rw::math::Transform3D<> getWTBody (const rw::kinematics::State& state) const
        {
            return rw::kinematics::Kinematics::worldTframe (_mframe, state);
        }

        /**
         * @brief Get the transform from the parent.
         * @param state [in] the state giving the current pose of the body.
         * @return the relative transform.
         */
        rw::math::Transform3D<> getWTParent (const rw::kinematics::State& state) const
        {
            return rw::kinematics::Kinematics::worldTframe (getParent (state), state);
        }

        /**
         * @brief return the linear velocity described in parent frame
         */
        rw::math::Vector3D<> getLinVel (const rw::kinematics::State& state) const
        {
            return _rstate.get (state).linvel;
            // const double *q = this->getData(state);
            // return rw::math::Vector3D<>(q[0],q[1],q[2]);
        }

        /**
         * @brief return the linear velocity described in world frame
         */
        rw::math::Vector3D<> getLinVelW (const rw::kinematics::State& state) const
        {
            return getWTParent (state).R () * getLinVel (state);
        }

        /**
         * @brief set the linear velocity.
         */
        void setLinVel (const rw::math::Vector3D<>& lvel, rw::kinematics::State& state)
        {
            _rstate.get (state).linvel = lvel;
        }

        /**
         * @brief Set the linear velocity in world frame.
         * @param lvel [in] the linear velocity.
         * @param state [out] the state with the current pose, updated with new velocity.
         */
        void setLinVelW (const rw::math::Vector3D<>& lvel, rw::kinematics::State& state)
        {
            setLinVel (inverse (getWTParent (state).R ()) * lvel, state);
        }

        /**
         * @brief returns the angular velocity described in parent frame
         */
        rw::math::Vector3D<> getAngVel (const rw::kinematics::State& state) const
        {
            return _rstate.get (state).angvel;
            // const double *q = this->getData(state);
            // return rw::math::Vector3D<>(q[3],q[4],q[5]);
        }

        /**
         * @brief returns the angular velocity described in world frame
         */
        virtual rw::math::Vector3D<> getAngVelW (const rw::kinematics::State& state) const
        {
            return getWTParent (state).R () * getAngVel (state);
        }

        /**
         * @brief set the angular velocity of this rigid body.
         */
        virtual void setAngVel (const rw::math::Vector3D<>& avel, rw::kinematics::State& state);

        /**
         * @brief Set the angular velocity in world frame.
         * @param avel [in] the angular velocity.
         * @param state [out] the state with the current pose, updated with new velocity.
         */
        void setAngVelW (const rw::math::Vector3D<>& avel, rw::kinematics::State& state)
        {
            setAngVel (inverse (getWTParent (state).R ()) * avel, state);
        }

        /**
         * @brief calculates the relative velocity in parent frame of a point p on the body
         * described in parent frame.
         */
        rw::math::Vector3D<> getPointVel (const rw::math::Vector3D<>& p,
                                          const rw::kinematics::State& state)
        {
            return getLinVel (state) + cross (getAngVel (state), p);
        }

        /**
         * @brief returns the mass of this body
         */
        inline double getMass () const { return getInfo ().mass; }

        /**
         * @brief returns the inverse of the mass of this body
         */
        inline double getMassInv () const { return _massInv; }

        /**
         * @brief returns the body inertia matrix
         */
        const rw::math::InertiaMatrix<>& getBodyInertia () const { return _Ibody; };

        /**
         * @brief return the inverse of the body inertia matrix
         */
        const rw::math::InertiaMatrix<>& getBodyInertiaInv () const { return _IbodyInv; };

        /**
         * @brief Get the principal inertia of this body. The inertia is described
         * around the center of mass for the principal axes.
         * @return the principal inertia and the principal axes as a rotation matrix.
         */
        const std::pair< rw::math::Rotation3D<>, rw::math::Vector3D<> >&
        getBodyPrincipalInertia () const
        {
            return _IbodyPrincipal;
        };

        /**
         * @brief returns the inverse of the inertia tensor described in
         * parent frame.
         */
        rw::math::InertiaMatrix<> calcInertiaTensorInv (const rw::kinematics::State& state) const;

        /**
         * @brief returns the inverse of the inertia tensor described in
         * world frame.
         */
        // rw::math::InertiaMatrix<> calcInertiaTensorInvW(const rw::kinematics::State& state)
        // const;

        /**
         * @brief returns the inverse of the inertia tensor described in
         * parent frame.
         */
        rw::math::InertiaMatrix<> calcInertiaTensor (const rw::kinematics::State& state) const;

        /**
         * @brief The the body frame as a movable frame.
         * @return a pointer to a movable frame.
         */
        rw::kinematics::MovableFrame* getMovableFrame () { return _mframe; }

        /**
         * @brief calculates the effective mass of this rigid body seen from
         * the contact point \b wPc. \b wPc is described relative to parent frame
         */
        rw::math::InertiaMatrix<> calcEffectiveMass (const rw::math::Vector3D<>& wPc,
                                                     const rw::kinematics::State& state) const;

        /**
         * @brief calculates the effective mass of this rigid body seen from
         * the contact point \b wPc. \b wPc is described relative to world
         */
        rw::math::InertiaMatrix<> calcEffectiveMassW (const rw::math::Vector3D<>& wPc,
                                                      const rw::kinematics::State& state) const;

        // rw::math::InertiaMatrix<> calcEffectiveInertia(const rw::kinematics::State& state) const;

        // rw::math::InertiaMatrix<> calcEffectiveInertiaInv(const rw::kinematics::State& state)
        // const;

      protected:
        // const variables
        //! @brief Mass of the body.
        const double _mass;
        //! @brief The inverse of the mass of the body.
        const double _massInv;

        //! @brief The state of a rigid body.
        struct RigidBodyState
        {
            //! @brief The linear velocity.
            rw::math::Vector3D<> linvel;
            //! @brief The angular velocity.
            rw::math::Vector3D<> angvel;
            //! @brief The net force.
            rw::math::Vector3D<> force;
            //! @brief The net torque.
            rw::math::Vector3D<> torque;
        };

        //! @brief The body frame.
        rw::kinematics::MovableFrame* _mframe;
        // rw::kinematics::Frame *_parent;

        // inertia tensors
        //! @brief Inertia of the body.
        const rw::math::InertiaMatrix<> _Ibody;
        //! @brief Inverse of the inertia of the body.
        const rw::math::InertiaMatrix<> _IbodyInv;
        //! @brief The principal inertia as a rotation and a vector giving the diagonal.
        const std::pair< rw::math::Rotation3D<>, rw::math::Vector3D<> > _IbodyPrincipal;

        // state variables
        //! @brief Inertia tensor in parent frame.
        rw::math::InertiaMatrix<> _ITensorInv;
        //! @brief Inverse inertia tensor in parent frame.
        rw::math::InertiaMatrix<> __ITensor;

        //! @brief The state of the rigid body.
        rw::kinematics::StatelessData< RigidBodyState > _rstate;
    };
    //! @}
}}    // namespace rwsim::dynamics

#endif /*DYNAMICS_RIGIDBODY_HPP_*/
