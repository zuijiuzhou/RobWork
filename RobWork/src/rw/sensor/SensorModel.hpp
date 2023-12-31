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

#ifndef RW_SENSOR_SENSORMODEL_HPP
#define RW_SENSOR_SENSORMODEL_HPP

/**
 * @file Sensor.hpp
 */
#if !defined(SWIG)
#include <rw/core/PropertyMap.hpp>
#include <rw/kinematics/Stateless.hpp>

#include <string>
#endif 

namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics

namespace rw { namespace sensor {

    /** @addtogroup sensor */
    /* @{ */

    /**
     * @brief a general sensormodel interface. The sensormodel describe the model of a sensor
     * and define the data that are part of the State. Much like Device, which describe
     * the kinematic model of a robot. A sensormodel should have a name id and be associated,
     * referenced to some frame in the workcell.
     */
    class SensorModel : public rw::kinematics::Stateless
    {
      public:
        //! smart pointer type
        typedef rw::core::Ptr< SensorModel > Ptr;

        /**
         * @brief constructor
         * @param name [in] the name of this sensor
         * @param frame [in] the frame that the sensor is referenced to
         */
        SensorModel (const std::string& name, rw::core::Ptr<kinematics::Frame> frame);

        /**
         * @brief constructor
         * @param name [in] the name of this sensor
         * @param frame [in] the frame that the sensor is referenced to
         * @param description [in] description of the sensor
         */
        SensorModel (const std::string& name, rw::core::Ptr<kinematics::Frame> frame,
                     const std::string& description);

        //! destructor
        virtual ~SensorModel () {}

        /**
         * @brief sets the name of this sensor
         * @param name [in] name of this sensor
         */
        void setName (const std::string& name) { _name = name; }

        /**
         * @brief sets the description of this sensor
         * @param description [in] description of this sensor
         */
        void setDescription (const std::string& description) { _description = description; }

        /**
         * @brief returns the name of this sensor
         * @return name of sensor
         */
        const std::string& getName () const { return _name; }

        /**
         * @brief returns a description of this sensor
         * @return reference to this sensors description
         */
        const std::string& getDescription () const { return _description; }

        /**
         * @brief The frame to which the sensor is attached.
         *
         * The frame can be NULL.
         */
        kinematics::Frame* getFrame () const { return _frame.get(); }

        /**
         * @brief Sets the frame to which the sensor should be attached
         *
         * @param frame The frame, which can be NULL
         */
        virtual void attachTo (rw::core::Ptr<kinematics::Frame> frame) { _frame = frame; }

        /**
         * @brief gets the propertymap of this sensor
         */
        rw::core::PropertyMap& getPropertyMap () { return _propertyMap; }

        /**
         * @brief gets the propertymap of this sensor
         */
        const rw::core::PropertyMap& getPropertyMap () const { return _propertyMap; }

      private:
        std::string _name;
        std::string _description;
        rw::core::Ptr<kinematics::Frame> _frame;
        rw::core::PropertyMap _propertyMap;
    };

    /** @} */
}}    // namespace rw::sensor

#endif    // end include guard
