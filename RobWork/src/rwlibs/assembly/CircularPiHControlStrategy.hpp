/********************************************************************************
 * Copyright 2013 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#ifndef RWLIBS_ASSEMBLY_CIRCULARPIHCONTROLSTRATEGY_HPP_
#define RWLIBS_ASSEMBLY_CIRCULARPIHCONTROLSTRATEGY_HPP_

/**
 * @file CircularPiHControlStrategy.hpp
 *
 * \copydoc rwlibs::assembly::CircularPiHControlStrategy
 */

#include <rwlibs/assembly/AssemblyControlStrategy.hpp>

#include <rw/core/Ptr.hpp>

namespace rw { namespace core {
    class PropertyMap;
}}    // namespace rw::core

namespace rwlibs { namespace assembly {
    //! @addtogroup assembly
    //! @{
    /**
     * @brief A AssemblyControlStrategy that can be used specifically for cylindric peg in hole
     * operations.
     *
     * This strategy mainly serves as a demonstration of how a strategy can be implemented.
     */
    class CircularPiHControlStrategy : public AssemblyControlStrategy
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< CircularPiHControlStrategy > Ptr;

        //! @brief Create new control strategy.
        CircularPiHControlStrategy ();

        //! @brief Destructor.
        virtual ~CircularPiHControlStrategy ();

        //! @copydoc rwlibs::assembly::AssemblyControlStrategy::createState
        ControlState::Ptr createState () const;

        //! @copydoc rwlibs::assembly::AssemblyControlStrategy::update
        rw::core::Ptr< AssemblyControlResponse >
        update (rw::core::Ptr< AssemblyParameterization > parameters,
                rw::core::Ptr< AssemblyState > real, rw::core::Ptr< AssemblyState > assumed,
                ControlState::Ptr controlState, rw::kinematics::State& state,
                rw::sensor::FTSensor* ftSensor, double time) const;

        //! @copydoc rwlibs::assembly::AssemblyControlStrategy::getApproach
        rw::math::Transform3D<> getApproach (rw::core::Ptr< AssemblyParameterization > parameters);

        //! @copydoc rwlibs::assembly::AssemblyControlStrategy::getID
        std::string getID ();

        //! @copydoc rwlibs::assembly::AssemblyControlStrategy::getDescription
        std::string getDescription ();

        //! @copydoc rwlibs::assembly::AssemblyControlStrategy::createParameterization
        rw::core::Ptr< AssemblyParameterization >
        createParameterization (const rw::core::Ptr< rw::core::PropertyMap > map);

      private:
        class CircularControlState;
    };
    //! @}
}}     // namespace rwlibs::assembly
#endif /* RWLIBS_ASSEMBLY_CIRCULARPIHCONTROLSTRATEGY_HPP_ */
