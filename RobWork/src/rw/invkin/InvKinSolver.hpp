#ifndef RW_INVKIN_INVKINSOLVER_HPP
#define RW_INVKIN_INVKINSOLVER_HPP
#if !defined(SWIG)
#include <rw/core/Ptr.hpp>
#include <rw/math/Q.hpp>
#include <rw/math/Transform3D.hpp>
#endif
namespace rw { namespace kinematics {
    class State;
    class Frame;
}}    // namespace rw::kinematics

namespace rw { namespace invkin {

    /** @addtogroup invkin */
    /*@{*/

    /**
     * @brief Interface for inverse kinematics algorithms
     *
     * The InvKinSolver interface provides an interface for calculating
     * the inverse kinematics of a device. That is to calculate
     * \f$\mathbf{q}\f$ such that \f$\robabx{base}{end}{\mathbf{T}}(\mathbf{q})=
     * \robabx{}{desired}{\mathbf{T}}\f$.
     *
     * By default it solves the problem beginning at the robot base and
     * ending with the frame defined as the end of the devices, and which is
     * accessible through the Device::getEnd() method.
     */
    class InvKinSolver
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< InvKinSolver > Ptr;
        //! @brief smart pointer type to this const class
        typedef rw::core::Ptr< const InvKinSolver > CPtr;

        //! destructor
        virtual ~InvKinSolver (){};

        /**
         * @brief Calculates the inverse kinematics
         *
         * Given a desired \f$\robabx{}{desired}{\mathbf{T}}\f$
         * and the current state, the method solves the inverse kinematics
         * problem.
         *
         * If the algorithm is able to identify multiple solutions (e.g. elbow
         * up and down) it will return all of these. Before returning a solution,
         * they may be checked to be within the bounds of the configuration space.
         * (See setCheckJointLimits(bool) )
         *
         * @param baseTend [in] Desired base to end transformation \f$
         * \robabx{}{desired}{\mathbf{T}}\f$
         *
         * @param state [in] State of the device from which to start the
         * iterations
         *
         * @return List of solutions. Notice that the list may be empty.
         *
         * @note The targets \b baseTend must be defined relative to the base of the
         * robot/device.
         */
        virtual std::vector< math::Q > solve (const rw::math::Transform3D<double>& baseTend,
                                              const rw::kinematics::State& state) const = 0;

        /**
         * @brief Specifies whether to check joint limits before returning a solution.
         *
         * @param check [in] If true the method should perform a check that joints are within
         * bounds.
         */
        virtual void setCheckJointLimits (bool check) = 0;

        /**
         * @brief Returns the Tool Center Point (TCP) used when solving the IK problem.
         *
         * @return The TCP Frame used when solving the IK.
         */
        virtual rw::core::Ptr< const rw::kinematics::Frame > getTCP () const = 0;
    };

    /** @} */

}}    // namespace rw::invkin

#endif    //#ifndef RW_INVKIN_INVKINSOLVER_HPP
