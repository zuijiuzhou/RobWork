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

#ifndef RW_MODELS_SERIALDEVICE_HPP
#define RW_MODELS_SERIALDEVICE_HPP

/**
 * @file SerialDevice.hpp
 */
#if !defined(SWIG)
#include <rw/models/JointDevice.hpp>

#include <vector>
#endif

namespace rw { namespace models {
/** @addtogroup models */
#if !defined(SWIG)
    //! @{
#endif

    /**
     * @brief The device for a serial chain.
     *
     * SerialChain is like JointDevice except that SerialChain has the
     * additional guarantee that the joints lie on a single parent to child
     * path of the kinematic tree.
     */
    class SerialDevice : public JointDevice
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< SerialDevice > Ptr;
        //! @brief smart pointer type to this const class
        typedef rw::core::Ptr< const SerialDevice > CPtr;

        /**
         * @brief Constructor
         *
         * @param first [in] the base frame of the robot
         * @param last [in] the end-effector of the robot
         * @param name [in] name of device
         * @param state [in] the connectedness of the frames
         */
        SerialDevice (rw::core::Ptr<rw::kinematics::Frame> first, rw::core::Ptr<rw::kinematics::Frame> last,
                      const std::string& name, const rw::kinematics::State& state);

        //! @brief destructor
        virtual ~SerialDevice () {}

        /**
         * @brief Frames of the device.
         *
         * This method is being used when displaying the kinematic
         * structure of devices in RobWorkStudio. The method really
         * isn't of much use for everyday programming.
         *
         * @return list of raw Frame pointers.
         */
        const std::vector< rw::kinematics::Frame* >& frames () const;

        /**
         * @brief Creates object
         *
         * @param serialChain [in] a vector of connected frames. The
         * first frame in \b serialChain is the base of the device and
         * the last frame of \b serialChain is the end of the device.
         * The joints of the device are the active joints of
         * \b serialChain.
         *
         * @param name [in] name of device
         *
         * @param state [in] the initial state of everything
         */
        SerialDevice (const std::vector< rw::kinematics::Frame* >& serialChain,
                      const std::string& name, const rw::kinematics::State& state);

      private:
        std::vector< rw::kinematics::Frame* > _kinematicChain;
    };
#if !defined(SWIG)
//! @}
#endif
}}    // namespace rw::models

#endif    // end include guard
