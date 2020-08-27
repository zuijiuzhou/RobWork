%module sdurw_control

%{
#include <rwlibs/swig/ScriptTypes.hpp>
#include <rw/core/Ptr.hpp>

using namespace rwlibs::swig;
%}
%include <exception.i>

%import <rwlibs/swig/sdurw.i>
%import <rwlibs/swig/sdurw_core.i>

%pragma(java) jniclassimports=%{
import org.robwork.sdurw.*;
import org.robwork.sdurw_core.*;
%}
%pragma(java) moduleimports=%{
import org.robwork.sdurw.*;
import org.robwork.sdurw_core.*;
%}
%typemap(javaimports) SWIGTYPE %{
import org.robwork.sdurw.*;
import org.robwork.sdurw_core.*;
%}

%nodefaultctor Controller;
/**
 * @brief interface that defines functionality for control of devices and actuators
 */
class Controller {
public:
    /**
     * @brief get the unique name of this controller
     *
     * @return name of the controller.
     */
	const std::string& getName() const;

    /**
     * @brief set the name of the controller
     *
     * @param name [in] the name
     */
	void setName(const std::string& name);
};

%template (ControllerPtr) rw::core::Ptr<Controller>;

%nodefaultctor JointController;
/**
 * @brief the joint controller interface describe how to input to a joint controller.
 * The output Force, Vel, Pos... must be available in the class implementing JointController interface
 */
class JointController {
public:
    //! control mode
	typedef enum {
        POSITION = 1, CNT_POSITION = 2, VELOCITY = 4, FORCE = 8, CURRENT = 16
    } ControlMode;
    
    /**
     * @brief destructor
     */
    virtual ~JointController();

    /**
     * @brief gets the control mode mask. Defines which types of control the JointController
     * supports
     */
    virtual unsigned int getControlModes() = 0;

    /**
     * @brief sets the control mode of this JointController. If the mode
     * is unsupported an exception is thrown
     */
    virtual void setControlMode(ControlMode mode) = 0;

    /**
     * @brief sets the target joint value for the current control mode.
     */
    virtual void setTargetPos(const Q& vals) = 0;

    /**
     * @brief sets the target velocity
     *
     * @param vals [in] in m/s
     */
    virtual void setTargetVel(const Q& vals) = 0;

    /**
     * @brief sets the target acceleration
     *
     * @param vals [in] in m/s^2
     */
    virtual void setTargetAcc(const Q& vals) = 0;

    /**
     * @brief get kinematic model of device that is controlled
     */
    virtual Device& getModel();

    /**
     * @brief return the current position of the controlled robot
     */
    virtual Q getQ() = 0;

    /**
     * @brief return the current velocity
     */
    virtual Q getQd() = 0;
};

%template (JointControllerPtr) rw::core::Ptr<JointController>;