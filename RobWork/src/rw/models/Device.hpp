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

#ifndef RW_MODELS_DEVICE_HPP
#define RW_MODELS_DEVICE_HPP

/**
 * @file Device.hpp
 */
#if !defined(SWIG)
#include <rw/models/JacobianCalculator.hpp>

#include <rw/core/PropertyMap.hpp>
#include <rw/core/Ptr.hpp>
#include <rw/kinematics/Stateless.hpp>
#include <rw/math/Q.hpp>
#include <rw/math/Transform3D.hpp>

#include <iosfwd>
#include <string>
#endif
namespace rw { namespace math {
    class Jacobian;
}}    // namespace rw::math

namespace rw { namespace kinematics {
    class Frame;
    class State;
}}    // namespace rw::kinematics

namespace rw { namespace models {
    /** @addtogroup models */
    /*@{*/

    /**
     * @brief An abstract device class
     *
     * The Device class is the basis for all other devices. It is assumed that all devices
     * have a configuration which can be encoded by a rw::math::Q, that all have a base frame
     * representing where in the work cell they are located and a primary end frame. Notice that
     * some devices may have multiple end-frames.
     */
    class Device : public rw::kinematics::Stateless
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< Device > Ptr;
        //! @brief const smart pointer type to this class
        typedef rw::core::Ptr< const Device > CPtr;

        //! Lower and upper corner of a box shaped configuration space.
        typedef std::pair< rw::math::Q, rw::math::Q > QBox;

        /**
         * Constructs a device with a name
         *
         * @param name [in] name of the device
         */
        Device (const std::string& name) : _name (name) {}

        /**
         * @brief Virtual destructor
         */
        virtual ~Device () {}

        /**
         * @brief Sets configuration vector @f$ \mathbf{q} \in \mathbb{R}^n @f$
         *
         * @param q [in] configuration vector @f$ \mathbf{q} @f$
         * @param state [in] state into which to set @f$ \mathbf{q} @f$
         *
         * @pre q.size() == getDOF()
         */
        virtual void setQ (const rw::math::Q& q, rw::kinematics::State& state) const = 0;

        /**
         * @brief Gets configuration vector @f$ \mathbf{q}\in \mathbb{R}^n @f$
         *
         * @param state [in] state from which which to get @f$ \mathbf{q} @f$
         * @return configuration vector @f$ \mathbf{q} @f$
         */
        virtual rw::math::Q getQ (const rw::kinematics::State& state) const = 0;

        /**
         * @brief Returns the upper @f$ \mathbf{q}_{min} \in \mathbb{R}^n @f$ and
         * lower @f$ \mathbf{q}_{max} \in \mathbb{R}^n @f$ bounds of the joint space
         *
         * @return std::pair containing @f$ (\mathbf{q}_{min}, \mathbf{q}_{max}) @f$
         */
        virtual QBox getBounds () const = 0;

        /**
         * @brief Sets the upper @f$ \mathbf{q}_{min} \in \mathbb{R}^n @f$ and
         * lower @f$ \mathbf{q}_{max} \in \mathbb{R}^n @f$ bounds of the joint space
         *
         * @param bounds [in] std::pair containing
         * @f$ (\mathbf{q}_{min}, \mathbf{q}_{max}) @f$
         */
        virtual void setBounds (const QBox& bounds) = 0;

        /**
         * @brief Returns the maximal velocity of the joints
         * \f$\mathbf{\dot{q}}_{max}\in \mathbb{R}^n\f$
         *
         * It is assumed that \f$
         * \dot{\mathbf{q}}_{min}=-\dot{\mathbf{q}}_{max}\f$
         *
         * @return the maximal velocity
         */
        virtual rw::math::Q getVelocityLimits () const = 0;

        /**
         * @brief Sets the maximal velocity of the joints
         * \f$\mathbf{\dot{q}}_{max}\in \mathbb{R}^n\f$
         *
         * It is assumed that \f$
         * \dot{\mathbf{q}}_{min}=-\dot{\mathbf{q}}_{max}\f$
         *
         * @param vellimits [in] Q with the maximal velocity
         */
        virtual void setVelocityLimits (const rw::math::Q& vellimits) = 0;

        /**
         * @brief Returns the maximal acceleration of the joints
         * \f$\mathbf{\ddot{q}}_{max}\in \mathbb{R}^n\f$
         *
         * It is assumed that \f$
         * \ddot{\mathbf{q}}_{min}=-\ddot{\mathbf{q}}_{max}\f$
         *
         * @return the maximal acceleration
         */
        virtual rw::math::Q getAccelerationLimits () const = 0;

        /**
         * @brief Sets the maximal acceleration of the joints
         * \f$\mathbf{\ddot{q}}_{max}\in \mathbb{R}^n\f$
         *
         * It is assumed that \f$
         * \ddot{\mathbf{q}}_{min}=-\ddot{\mathbf{q}}_{max}\f$
         *
         * @param  acclimits [in] the maximal acceleration
         */
        virtual void setAccelerationLimits (const rw::math::Q& acclimits) = 0;

        /**
         * @brief Returns number of active joints
         * @return number of active joints @f$ n @f$
         */
        virtual size_t getDOF () const = 0;

        /**
         * @brief Returns the name of the device
         * @return name of the device
         */
        const std::string& getName () const { return _name; }

        /**
         * @brief Sets the name of the Device
         * @param name [in] the new name of the frame
         */
        void setName (const std::string& name) { _name = name; }

        /**
         * @brief a method to return the frame of the base of the device.
         * @return the base frame
         */
        virtual rw::kinematics::Frame* getBase () = 0;

        /**
         * @brief a method to return the frame of the base of the device.
         * @return the base frame
         */
        virtual const rw::kinematics::Frame* getBase () const = 0;

        /**
         * @brief a method to return the frame of the end of the device
         * @return the end frame
         */
        virtual rw::kinematics::Frame* getEnd () = 0;

        /**
         * @brief a method to return the frame of the end of the device
         *
         * @return the end frame
         */
        virtual const rw::kinematics::Frame* getEnd () const = 0;

        /**
         * @brief Calculates the homogeneous transform from base to a frame f
         * @f$ \robabx{b}{f}{\mathbf{T}} @f$
         * @return the homogeneous transform @f$ \robabx{b}{f}{\mathbf{T}} @f$
         */
        rw::math::Transform3D< double > baseTframe (rw::core::Ptr<const rw::kinematics::Frame> f,
                                                    const rw::kinematics::State& state) const;

        /**
         * @brief Calculates the homogeneous transform from base to the end frame
         * @f$ \robabx{base}{end}{\mathbf{T}} @f$
         * @return the homogeneous transform @f$ \robabx{base}{end}{\mathbf{T}} @f$
         */
        rw::math::Transform3D< double > baseTend (const rw::kinematics::State& state) const;

        /**
         * @brief Calculates the homogeneous transform from world to base @f$
         * \robabx{w}{b}{\mathbf{T}} @f$
         *
         * @return the homogeneous transform @f$ \robabx{w}{b}{\mathbf{T}} @f$
         */
        rw::math::Transform3D< double > worldTbase (const rw::kinematics::State& state) const;

# if !defined(SWIGJAVA)
        /**
         * @brief Calculates the jacobian matrix of the end-effector described
         * in the robot base frame @f$ ^{base}_{end}\mathbf{J}_{\mathbf{q}}(\mathbf{q})
         * @f$
         *
         * @param state [in] State for which to calculate the Jacobian
         *
         * @return the @f$ 6*ndof @f$ jacobian matrix: @f$
         * {^{base}_{end}}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) @f$
         *
         * This method calculates the jacobian relating joint velocities (@f$
         * \mathbf{\dot{q}} @f$) to the end-effector velocity seen from
         * base-frame (@f$ \nu^{ase}_{end} @f$)
         *
         * \f[
         * \nu^{base}_{end} =
         * {^{base}_{end}}\mathbf{J}_\mathbf{q}(\mathbf{q})\mathbf{\dot{q}}
         * \f]
         *
         *
         * The jacobian matrix \f[ {^{base}_n}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) \f]
         * is defined as:
         *
         * \f[
         * {^{base}_n}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) =
         * \frac{\partial ^{base}\mathbf{x}_n}{\partial \mathbf{q}}
         * \f]
         *
         * Where:
         * \f[
         *  {^{base}_n}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) =
         *  \left[
         *    \begin{array}{cccc}
         *      {^{base}_1}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) &
         *      {^{base}_2}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) &
         *      \cdots &
         *      {^b_n}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) \\
         *    \end{array}
         *  \right]
         * \f]
         * where \f$ {^{base}_i}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) \f$ is defined by
         * \f[
         *  {^{base}_i}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) =
         *  \begin{array}{cc}
         *    \left[
         *      \begin{array}{c}
         *        {^{base}}\mathbf{z}_i \times {^{i}\mathbf{p}_n} \\
         *        {^{base}}\mathbf{z}_i \\
         *      \end{array}
         *    \right] & \textrm{revolute joint}
         *  \end{array}
         * \f]
         * \f[
         *  {^{base}_i}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) =
         *  \begin{array}{cc}
         *    \left[
         *      \begin{array}{c}
         *        {^{base}}\mathbf{z}_i \\
         *        \mathbf{0} \\
         *    \end{array}
         *    \right] & \textrm{prismatic joint} \\
         *  \end{array}
         * \f]
         *
         * By default the method forwards to baseJframe().
         */

         #endif 
        virtual rw::math::Jacobian baseJend (const rw::kinematics::State& state) const = 0;

        /**
         * @brief Calculates the jacobian matrix of a frame f described in the
         * robot base frame @f$ ^{base}_{frame}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) @f$
         *
         * @param frame [in] Frame for which to calculate the Jacobian
         * @param state [in] State for which to calculate the Jacobian
         *
         * @return the @f$ 6*ndof @f$ jacobian matrix: @f$
         * {^{base}_{frame}}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) @f$
         *
         * This method calculates the jacobian relating joint velocities (@f$
         * \mathbf{\dot{q}} @f$) to the frame f velocity seen from base-frame
         * (@f$ \nu^{base}_{frame} @f$)
         *
         * \f[
         * \nu^{base}_{frame} =
         * {^{base}_{frame}}\mathbf{J}_\mathbf{q}(\mathbf{q})\mathbf{\dot{q}}
         * \f]
         *
         *
         * The jacobian matrix \f[ {^{base}_n}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) \f]
         * is defined as:
         *
         * \f[
         * {^{base}_n}\mathbf{J}_{\mathbf{q}}(\mathbf{q}) =
         * \frac{\partial ^{base}\mathbf{x}_n}{\partial \mathbf{q}}
         * \f]
         *
         * By default the method forwards to baseJframes().
         */
        virtual rw::math::Jacobian baseJframe (rw::core::Ptr<const rw::kinematics::Frame> frame,
                                               const rw::kinematics::State& state) const;

        /**
         * @brief The Jacobian for a sequence of frames.
         *
         * A Jacobian is computed for each of the frames and the Jacobians are
         * stacked on top of eachother.
         * @param frames [in] the frames to calculate the frames from
         * @param state [in] the state to calculate in
         * @return the jacobian
         */
        virtual rw::math::Jacobian baseJframes (const std::vector< rw::kinematics::Frame* >& frames,
                                                const rw::kinematics::State& state) const
        {
            return baseJCframes (frames, state)->get (state);
        }

        /**
           @brief DeviceJacobian for the end frame.

           By default this method forwards to baseDJframe().
        */
        virtual rw::core::Ptr< rw::models::JacobianCalculator >
        baseJCend (const rw::kinematics::State& state) const;

        /**
           @brief DeviceJacobian for a particular frame.

           By default this method forwards to baseDJframes().
        */
        virtual rw::core::Ptr< rw::models::JacobianCalculator >
        baseJCframe ( rw::core::Ptr< const rw::kinematics::Frame> frame, const rw::kinematics::State& state) const;

        /**
         * @brief DeviceJacobian for a sequence of frames.
         */
        virtual rw::core::Ptr< rw::models::JacobianCalculator >
        baseJCframes (const std::vector< rw::kinematics::Frame* >& frames,
                      const rw::kinematics::State& state) const = 0;

        /**
         * @brief Miscellaneous properties of the device.
         *
         * The property map of the device is provided to let the user store
         * various settings for the device. The settings are typically loaded
         * from setup files.
         *
         * The low-level manipulations of the property map can be cumbersome. To
         * ease these manipulations, the PropertyAccessor utility class has been
         * provided. Instances of this class are provided for a number of common
         * settings, however it is undecided if these properties are a public
         * part of RobWork.
         *
         * @return The property map of the device.
         */
        const rw::core::PropertyMap& getPropertyMap () const { return _propertyMap; }

        /**
         * @copydoc getPropertyMap
         */
        rw::core::PropertyMap& getPropertyMap () { return _propertyMap; }

      private:
        std::string _name;

        rw::core::PropertyMap _propertyMap;

      private:
        Device (const Device&);
        Device& operator= (const Device&);
    };

#if !defined(SWIG)
    /**
       @brief Streaming operator for devices.
     */
    std::ostream& operator<< (std::ostream& out, const Device& device);

/*@}*/
#endif

}}    // namespace rw::models

#if defined(SWIG)
TOSTRING_OUTOFCLASSDEF (rw::models::Device);
#endif

#endif    // end include guard
