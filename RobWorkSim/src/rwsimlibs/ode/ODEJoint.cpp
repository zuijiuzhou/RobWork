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

#include "ODEJoint.hpp"
#include "ODEUtil.hpp"

#include <rw/common/macros.hpp>
#include <rw/kinematics/Frame.hpp>
#include <rw/kinematics/Kinematics.hpp>
#include <rw/math/Transform3D.hpp>

#include <ode/ode.h>

using namespace rw::kinematics;
using namespace rw::math;
using namespace rwsim;
using namespace rwsim::simulator;

ODEJoint::ODEJoint(
		JointType jtype,
         dJointID odeJoint,
         dJointID odeMotor,
         dBodyID body,
         dynamics::RigidJoint* rwjoint):
			 _jtype(jtype),
             _jointId(odeJoint),
             _bodyId(body),
             _motorId(odeMotor),
             _rwJoint(rwjoint),
             _owner(NULL),
             _type(ODEJoint::RIGID),
             _bodyFrame(rwjoint->getBodyFrame()),
             _offset( rwjoint->getInfo().masscenter )
{

}

ODEJoint::ODEJoint(
		 JointType jtype,
		 dJointID odeJoint,
         dJointID odeMotor,
         dBodyID body,
         ODEJoint* owner,
         rw::kinematics::Frame *bframe,
         double scale,
         double off,
         dynamics::RigidJoint* rwjoint):
			 _jtype(jtype),
             _jointId(odeJoint),
             _motorId(odeMotor),
             _bodyId(body),
             _owner(owner),
             _scale(scale),
             _rwJoint(rwjoint),
             _off(off),
             _type(ODEJoint::DEPEND),
             _bodyFrame(bframe),
             _offset(rwjoint->getInfo().masscenter)
{

}
/*
ODEJoint* ODEJoint::make(RevoluteJoint* joint, dBodyID parent, dWorldID worldId){
    const double qinit = rwjoint->getQ(initState)[0];

    dJointID hinge = dJointCreateHinge(worldId, 0);
    dJointAttach(hinge, odeChild, odeParent);
    dJointSetHingeAxis(hinge, haxis(0) , haxis(1), haxis(2));
    dJointSetHingeAnchor(hinge, hpos(0), hpos(1), hpos(2));

    dJointID motor = dJointCreateAMotor (_worldId, 0);
    dJointAttach(motor, odeChild, odeParent);
    dJointSetAMotorNumAxes(motor, 1);
    dJointSetAMotorAxis(motor, 0, 1, haxis(0) , haxis(1), haxis(2));
    dJointSetAMotorAngle(motor,0, qinit);
    dJointSetAMotorParam(motor,dParamFMax, maxForce(i) );
    dJointSetAMotorParam(motor,dParamVel,0);

    //dJointSetAMotorParam(Amotor,dParamLoStop,-0);
    //dJointSetAMotorParam(Amotor,dParamHiStop,0);
    //std::cout << "CREATED ODEJOINT: " << rwjoint->getName() << std::endl;
    ODEJoint *odeJoint = new ODEJoint(hinge, motor, rjoint);
    _jointToODEJoint[rwjoint] = odeJoint;
    odeJoints.push_back(odeJoint);

}
*/

void ODEJoint::reset(const rw::kinematics::State& state){
    // if the fixed transform between two bodies is changed
    // then any constraint between these need to be reset
    // so we need to reattach the constraint
    Frame *bframe = NULL;
    State rstate = state;
    double zeroq[] = {0.0,0.0,0.0,0.0};
    if(_type!=ODEJoint::DEPEND){
        _rwJoint->getJoint()->setData(rstate, zeroq);
        bframe = &_rwJoint->getFrame();
        Transform3D<> wTb = rw::kinematics::Kinematics::worldTframe( bframe, rstate);
        wTb.P() += wTb.R()*_offset;
        ODEUtil::setODEBodyT3D( _bodyId, wTb );
    } else {
        //_rwJoint->getJoint()->setData(rstate, zeroq);
        _owner->getRigidJoint()->getJoint()->setData(rstate, zeroq);
        bframe = _bodyFrame;
        Transform3D<> wTb = rw::kinematics::Kinematics::worldTframe( bframe, rstate);
        wTb.P() += wTb.R()*_offset;
        ODEUtil::setODEBodyT3D( _bodyId, wTb );
    }

    Transform3D<> wTchild = Kinematics::worldTframe(bframe, rstate);
    Vector3D<> hpos = wTchild.P();
    Vector3D<> haxis = wTchild.R() * Vector3D<>(0,0,1);

    if(_jtype==Revolute){
        //dJointGetBody()
        dJointSetHingeAxis(_jointId, haxis(0) , haxis(1), haxis(2));
        dJointSetHingeAnchor(_jointId, hpos(0), hpos(1), hpos(2));
    } else if(_jtype==Prismatic){
        //dJointAttach(slider, odeChild->getBodyID(), odeParent->getBodyID());
        //dJointSetSliderAxis(_jointId, haxis(0) , haxis(1), haxis(2));
        //dJointSetHingeAnchor(slider, hpos(0), hpos(1), hpos(2));
    }

    if(_type!=ODEJoint::DEPEND){
        bframe = &_rwJoint->getFrame();
        Transform3D<> wTb = rw::kinematics::Kinematics::worldTframe( bframe, state);
        wTb.P() += wTb.R()*_offset;
        ODEUtil::setODEBodyT3D( _bodyId, wTb );
    } else {
        bframe = _bodyFrame;
        Transform3D<> wTb = rw::kinematics::Kinematics::worldTframe( bframe, state);
        wTb.P() += wTb.R()*_offset;
        ODEUtil::setODEBodyT3D( _bodyId, wTb );
    }

    dBodyEnable( _bodyId );
    dBodySetAngularVel( _bodyId, 0, 0, 0 );
    dBodySetLinearVel( _bodyId, 0, 0, 0 );
}

