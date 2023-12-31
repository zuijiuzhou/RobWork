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

#ifndef RWLIBS_SIMULATION_CONTROLLER_HPP_
#define RWLIBS_SIMULATION_CONTROLLER_HPP_

//! @file SimulatedController.hpp
#if !defined(SWIG)
#include <rwlibs/simulation/Simulator.hpp>

#include <rw/core/Ptr.hpp>
#include <rw/kinematics/Stateless.hpp>
#include <rw/models/ControllerModel.hpp>
#include <rwlibs/control/Controller.hpp>
#endif 
namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics

namespace rwlibs { namespace simulation {
    //! @addtogroup simulation
    // @{
    /**
     * @brief interface of a simulated controller
     */
    class SimulatedController : public rw::kinematics::Stateless
    {
      protected:
        /**
         * @brief Constructor.
         * @param model [in] the controller model.
         */
        SimulatedController (rw::models::ControllerModel::Ptr model);

      public:
        //! @brief smart pointer type of this class
        typedef rw::core::Ptr< SimulatedController > Ptr;

        /**
         *  @brief get the name of this controller
         *  @return name of this controller
         */
        virtual std::string getControllerName () = 0;

        /**
         * @brief updates/steps the controller with time step \b dt. It will update
         * the state \b state accordingly
         * @param info [in] update information related to the time step.
         * @param state [in/out] the current state
         */
        virtual void update (const rwlibs::simulation::Simulator::UpdateInfo& info, rw::kinematics::State& state) = 0;

        /**
         * @brief reset the controller to the applied state
         * @param state [in] the state to reset to
         */
        virtual void reset (const rw::kinematics::State& state) = 0;

        /**
         * @brief get the controller handle eg. statefull handle, associated with this simulated
         * controller
         * @return
         */
        virtual rw::core::Ptr<rwlibs::control::Controller>
        getControllerHandle (rw::core::Ptr<rwlibs::simulation::Simulator> sim) = 0;

        /**
         * @brief get the controllermodel of this simulated controller
         * @return
         */
        rw::core::Ptr<rw::models::ControllerModel> getControllerModel () { return _model; }

        /**
         * @brief disable or enable this controller
         * @param enabled
         */
        virtual void setEnabled (bool enabled) = 0;

        /**
         * @brief true if this controller is enabled
         * @return
         */
        virtual bool isEnabled () const = 0;

      private:
        rw::models::ControllerModel::Ptr _model;
    };
    //! @}
}}    // namespace rwlibs::simulation

#endif /*CONTROLLER_HPP_*/
