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

#ifndef RW_INVKIN_AMGIGUITYRESOLVER_HPP
#define RW_INVKIN_AMGIGUITYRESOLVER_HPP

#if !defined(SWIG)
#include <rw/invkin/InvKinSolver.hpp>

#include <rw/math/Transform3D.hpp>
#endif
namespace rw { namespace models {
    class Device;
}}    // namespace rw::models
namespace rw { namespace models {
    class JointDevice;
}}    // namespace rw::models

namespace rw { namespace invkin {

    /** @addtogroup invkin */
    /*@{*/

    /**
     * @brief Wraps a InvKinSolver and searches for ambiguities due to joint able to rotate
     * \f$ 2 \ pi \f$ or more.
     *
     * For each solution \f$\mathbf{q}\f$ the method tries to see if a \f$ j \f$ exists s.t.
     * \f$ \mathbf{q}(i)=\mathbf{q}(i)+j * 2 \ pi \f$ is a valid solution.
     *
     * The AmbiguityResolver always tests for joint limits.
     */
    class AmbiguityResolver : public rw::invkin::InvKinSolver
    {
      public:
        /**
         * @brief Constructs an AmbiguityResolver
         * @param invkin [in] The inverse kinematics solver to obtain solutions from
         * @param device [in] the device for which to calculate inverse kinematics
         */
        AmbiguityResolver (const rw::core::Ptr<rw::invkin::InvKinSolver>& invkin,
                           rw::core::Ptr< rw::models::JointDevice > device);

        /**
         * @brief Destructor
         */
        ~AmbiguityResolver (void);

        /**
         * @brief Calls the InvKinSolver provided and resolves ambiguities.
         * @copydoc InvKinSolver::solve
         */
        virtual std::vector< rw::math::Q > solve (const rw::math::Transform3D<double>& baseTend,
                                              const class rw::kinematics::State& state) const;

        /**
         * @brief No effect. The AmbiguityResolver always tests for joint limits.
         */
        virtual void setCheckJointLimits (bool check);

        /**
         * @copydoc InvKinSolver::getTCP
         */
        virtual rw::core::Ptr< const rw::kinematics::Frame > getTCP () const;

      private:
        InvKinSolver::Ptr _invkin;
        rw::core::Ptr< rw::models::Device > _device;
        std::vector< size_t > _indices;
        rw::math::Q _lower;
        rw::math::Q _upper;
    };

    /** @} */

}}    // namespace rw::invkin

#endif    //#ifndef RW_INVKIN_AMGIGUITYRESOLVER_HPP
