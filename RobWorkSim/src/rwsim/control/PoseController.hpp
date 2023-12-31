#ifndef RWSIM_CONTROL_POSECONTROLLER_HPP_
#define RWSIM_CONTROL_POSECONTROLLER_HPP_

//! @file PoseController.hpp

#include <rw/core/Ptr.hpp>
#include <rw/models/Device.hpp>
#include <rwlibs/control/Controller.hpp>
#include <rwlibs/simulation/SimulatedController.hpp>

namespace rwlibs { namespace algorithms {
    class XQPController;
}}    // namespace rwlibs::algorithms
namespace rwsim { namespace dynamics {
    class DynamicDevice;
}}    // namespace rwsim::dynamics

namespace rwsim { namespace control {
    //! @addtogroup rwsim_control
    //! @{

    /**
     * @brief a JointController that use a PD loop on each joint
     * to control the velocity such that the position target is
     * reached.
     */
    class PoseController : public rwlibs::control::Controller,
                           public rwlibs::simulation::SimulatedController
    {
      public:
        //! @brief smart pointer type
        typedef rw::core::Ptr< PoseController > Ptr;

        /**
         * @brief constructor
         * @param name
         * @param rdev [in] device that is to be controlled
         * @param state [in] target state
         * @cond
         * @param cmode [in] the control mode used
         * @param pdparams [in] list of pd parameters. must be same length as number of joints.
         * @endcond
         * @param dt [in] the sampletime (time between samples in seconds) used in the control
         * loop, this should be larger than the expected update sample time.
         */
        PoseController (const std::string& name,
                        rw::core::Ptr< rwsim::dynamics::DynamicDevice > rdev,
                        const rw::kinematics::State& state, double dt);

        /**
         * @brief constructor
         * @param name
         * @param rdev [in] device that is to be controlled
         * @param state [in] target state
         * @cond
         * @param cmode [in] the control mode used
         * @param pdparam [in] pd parameter - used for all joints
         * @endcond
         * @param dt [in] the sampletime (time between samples in seconds) used in the control
         * loop, this should be larger than the expected update sample time.
         * @param endframe
         */
        PoseController (const std::string& name,
                        rw::core::Ptr< rwsim::dynamics::DynamicDevice > rdev,
                        const rw::kinematics::State& state, double dt,
                        rw::core::Ptr<rw::kinematics::Frame> endframe);

        /**
         * @brief destructor
         */
        virtual ~PoseController (){};

        /**
         * @brief the time between samples
         * @return the sample time in seconds
         */
        double getSampleTime ();

        /**
         * @brief set the time between samples in seconds
         * @param stime [in] sample time
         */
        void setSampleTime (double stime);

        //! @copydoc rwlibs::simulation::SimulatedController::update
        void update (const rwlibs::simulation::Simulator::UpdateInfo& info,
                     rw::kinematics::State& state);

        //! @copydoc rwlibs::simulation::SimulatedController::reset
        void reset (const rw::kinematics::State& state);

        //! @copydoc rwlibs::simulation::SimulatedController::getControllerName
        Controller* getController () { return this; };

        std::string getControllerName () { return getName (); };

        /**
         * @brief get the device that is controlled by this controller
         * @return
         */
        rw::core::Ptr< rw::models::Device > getControlledDevice () { return _device; }

        void setEnabled (bool enabled) { _enabled = enabled; };

        bool isEnabled () const { return _enabled; };

        ////// inherited from JointController

        //! @copydoc rwlibs::control::JointController::setTargetPos
        void setTarget (const rw::math::Transform3D<>& target);

        /**
         * @brief sets target which is a cartesean position and the target velocity in that position
         * @param target
         * @param vals
         */
        void setTarget (const rw::math::Transform3D<>& target,
                        const rw::math::VelocityScrew6D<>& vals);

        rwlibs::control::Controller::Ptr
        getControllerHandle (rwlibs::simulation::Simulator::Ptr sim)
        {
            return this;
        }

      private:
        PoseController ();

      private:
        rw::core::Ptr< rwsim::dynamics::DynamicDevice > _ddev;
        rw::core::Ptr< rw::models::Device > _device;
        rw::core::Ptr<rw::kinematics::Frame> _endframe;

        rw::math::Transform3D<> _target;
        rw::math::VelocityScrew6D<> _targetVel;

        double _stime, _accTime;    // sample time

        rw::core::Ptr< rwlibs::algorithms::XQPController > _xqp;
        bool _enabled;
    };

    //! @}
}}    // namespace rwsim::control

#endif /*PoseController_HPP_*/
