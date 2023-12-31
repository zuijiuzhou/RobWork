/********************************************************************************
 * Copyright 2015 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#ifndef RW_INVKIN_CLOSEDFORMIK_SOLVER_UR_HPP_
#define RW_INVKIN_CLOSEDFORMIK_SOLVER_UR_HPP_
#if !defined(SWIG)
#include <rw/invkin/ClosedFormIK.hpp>
#include <rw/kinematics/FKRange.hpp>
#include <rw/math/Vector2D.hpp>
#endif
namespace rw { namespace models {
    class SerialDevice;
}}    // namespace rw::models

namespace rw { namespace invkin {

    /**
     * @brief Analytical inverse kinematics solver to the kinematics of a Universal Robots.
     */
    class ClosedFormIKSolverUR : public rw::invkin::ClosedFormIK
    {
      public:
        //! @brief Smart pointer type to ClosedFormURSolver
        typedef rw::core::Ptr< ClosedFormIKSolverUR > Ptr;
        //! @brief Smart pointer type to const ClosedFormURSolver
        typedef rw::core::Ptr< const ClosedFormIKSolverUR > CPtr;

        /**
         * @brief Construct new closed form solver for a Universal Robot.
         * @note The dimensions will be automatically extracted from the device, using an arbitrary
         * state.
         * @param device [in] the device.
         * @param state [in] the state to use to extract dimensions.
         */
        ClosedFormIKSolverUR (const rw::core::Ptr< const rw::models::SerialDevice > device,
                              const rw::kinematics::State& state);

        //! @brief Destructor.
        virtual ~ClosedFormIKSolverUR ();

        //! @copydoc InvKinSolver::solve
        std::vector< rw::math::Q > solve (const rw::math::Transform3D<double>& baseTend,
                                          const rw::kinematics::State& state) const;

        //! @copydoc InvKinSolver::setCheckJointLimits
        void setCheckJointLimits (bool check);

        /**
         * @copydoc InvKinSolver::getTCP
         */
        virtual rw::core::Ptr< const rw::kinematics::Frame > getTCP () const;

      private:
        rw::math::Q adjustJoints (const rw::math::Q& q) const;

        void addBaseAngleSolutions (const rw::math::Transform3D<double>& baseTend,
                                    const rw::math::Vector3D<double>& baseTdh5,
                                    rw::kinematics::State& state, double angle,
                                    std::vector< rw::math::Q >& res) const;

        void addElbowSolutions (const rw::math::Transform3D<double>& baseTend,
                                const rw::math::Vector3D<double>& baseTdh5, rw::kinematics::State& state,
                                double baseAngle, std::pair< double, double > elbow,
                                std::vector< rw::math::Q >& res) const;

        // UR specific geometric functions
        std::pair< rw::math::Vector3D<double>, rw::math::Vector3D<double> >
        getJoint4Positions (const rw::math::Vector3D<>& baseTdh5, const rw::math::Vector3D<>& tcpZ,
                            const rw::kinematics::State& state) const;
        std::pair< std::pair< double, double >, std::pair< double, double > >
        getElbowJoints (const rw::math::Vector3D<>& intersection,
                        rw::kinematics::State& state) const;
        rw::math::Q getOrientationJoints (const rw::math::Transform3D<>& baseTend,
                                          const rw::math::Vector3D<>& baseTdh5,
                                          rw::kinematics::State& state) const;
        std::vector< double > findBaseAngle (const rw::math::Vector2D<>& pos,
                                             const rw::kinematics::State& state) const;

        // Generic geometric functions
        static std::pair< rw::math::Vector3D<>, rw::math::Vector3D<> >
        findCirclePlaneIntersection (const rw::math::Vector3D<>& circleCenter, double radius,
                                     const rw::math::Vector3D<>& circleDir1,
                                     const rw::math::Vector3D<>& circleDir2,
                                     const rw::math::Vector3D<>& planeNormal);
        static std::pair< std::pair< double, double >, std::pair< double, double > >
        findTwoBarAngles (const rw::math::Vector2D<>& pos, double L1, double L2);
        static rw::math::Vector3D<> getPerpendicularVector (const rw::math::Vector3D<>& vec);

      private:
        const rw::core::Ptr< const rw::models::SerialDevice > _device;
        bool _checkJointLimits;
        std::vector< const rw::kinematics::Frame* > _frames;
        double _lTcp, _baseRadius, _baseRadiusSqr, _endCircleRadius, _l1, _l2, _lJ0J1;

        rw::kinematics::FKRange _fkRange0_2;
        rw::kinematics::FKRange _fkRange2_0;

        rw::kinematics::FKRange _fkRange3_0;
        rw::kinematics::FKRange _fkRange4_3;
        rw::kinematics::FKRange _fkRange5_4;
        rw::kinematics::FKRange _fkRange6_5;

        rw::math::Rotation3D<> _rotAlignElbowJoint;
        rw::math::Rotation3D<> _rotAlignDH4;
        rw::math::Vector3D<> _zaxisJoint6In5;

        rw::math::Q _qMin, _qMax;
    };

}}    // namespace rw::invkin

#endif /* RW_INVKIN_CLOSEDFORMIK_SOLVER_UR_HPP_ */
