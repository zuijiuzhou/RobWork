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

#ifndef RW_MODELS_CONTROLLERMODEL_HPP
#define RW_MODELS_CONTROLLERMODEL_HPP

/**
 * @file ControllerModel.hpp
 */
#if !defined(SWIG)
#include <rw/core/PropertyMap.hpp>
#include <rw/kinematics/Stateless.hpp>

#include <string>
#endif 
namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics

namespace rw { namespace models {

    /** @addtogroup models */
    /* @{ */

    /**
     * @brief Interface to allow modelling of different types of controllers.
     * A controller is an instance that takes an input manipulates it to an output
     * that in effect controls something. As such controllers vary greatly and have
     * only little in common.
     */
    class ControllerModel : public rw::kinematics::Stateless
    {
      public:
        //! smart pointer type
        typedef rw::core::Ptr< ControllerModel > Ptr;

        /**
         * @brief constructor
         * @param name [in] the name of this controllermodel
         * @param frame [in] the frame to which this controller is attached/associated.
         */
        ControllerModel (const std::string& name, rw::core::Ptr<rw::kinematics::Frame> frame);

        /**
         * @brief constructor
         * @param name [in] the name of this controllermodel
         * @param frame [in] the frame to which this controller is attached/associated.
         * @param description [in] description of the controller
         */
        ControllerModel (const std::string& name, rw::core::Ptr<rw::kinematics::Frame> frame,
                         const std::string& description);

        //! destructor
        virtual ~ControllerModel () {}

        /**
         * @brief sets the name of this controllermodel
         * @param name [in] name of this controllermodel
         */
        void setName (const std::string& name) { _name = name; }

        /**
         * @brief sets the description of this controllermodel
         * @param description [in] description of this controllermodel
         */
        void setDescription (const std::string& description) { _description = description; }

        /**
         * @brief returns the name of this controllermodel
         * @return name of controllermodel
         */
        const std::string& getName () const { return _name; }

        /**
         * @brief returns a description of this controllermodel
         * @return reference to this controllermodels description
         */
        const std::string& getDescription () const { return _description; }

        /**
         * @brief The frame to which the controllermodel is attached.
         *
         * The frame can be NULL.
         */
        rw::kinematics::Frame* getFrame () const { return _frame.get(); }

        /**
         * @brief Sets the frame to which the controllermodel should be attached
         *
         * @param frame The frame, which can be NULL
         */
        virtual void attachTo (rw::core::Ptr<rw::kinematics::Frame> frame) { _frame = frame; }

        /**
         * @brief gets the propertymap of this controllermodel
         */
        rw::core::PropertyMap& getPropertyMap () { return _propertyMap; }

        /**
         * @brief gets the propertymap of this controllermodel
         */
        const rw::core::PropertyMap& getPropertyMap () const { return _propertyMap; }

      private:
        std::string _name;
        std::string _description;
        rw::core::Ptr<rw::kinematics::Frame> _frame;
        rw::core::PropertyMap _propertyMap;
    };

    /** @} */
}}    // namespace rw::models

#endif    // end include guard
