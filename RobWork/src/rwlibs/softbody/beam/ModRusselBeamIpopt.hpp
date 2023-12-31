/*
Copyright 2013 The Robotics Group, The Maersk Mc-Kinney Moller Institute,

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef RWLIBS_SOFTBODY_MODRUSSELBEAMIPOPT_HPP
#define RWLIBS_SOFTBODY_MODRUSSELBEAMIPOPT_HPP

#include "IpIpoptApplication.hpp"
#include <rwlibs/softbody/beam/ModRusselBeamBase.hpp>
#include <memory>

namespace rwlibs { namespace softbody {
    /** @addtogroup softbody */
    /*@{*/

    /**
     * @brief Implementation of the Modified Russel Beam Problem using IPOPT
     **/
    class ModRusselBeamIpopt : public ModRusselBeamBase
    {
      public:
        /**
         * @brief Constructor
         *
         * @param geomPtr pointer to the beam geometry
         * @param obstaclePtr pointer to the plane obstacle
         * @param M number of discretization points
         **/
        ModRusselBeamIpopt (std::shared_ptr< BeamGeometry > geomPtr,
                            std::shared_ptr< BeamObstaclePlane > obstaclePtr, int M);

        virtual ~ModRusselBeamIpopt ();

        /**
         * @brief solve the minimization problem
         *
         * @param xinituser starting guess, will be overwritten by solution
         * @param U vector to put the x-component of the curve in
         * @param V vector to put the x-component of the curve in
         **/
        void solve (Eigen::VectorXd& xinituser, Eigen::VectorXd& U, Eigen::VectorXd& V);

      private:
        Ipopt::SmartPtr< Ipopt::TNLP > _nlp;
        Ipopt::SmartPtr< Ipopt::IpoptApplication > _app;
    };
    /*@}*/
}}    // namespace rwlibs::softbody

#endif    // MODRUSSELBEAMIPOPT_HPP
