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


#include "BeamJoint.hpp"

#include <rw/math/Vector3D.hpp>
#include <rw/math/Rotation3D.hpp>
#include <rw/kinematics/State.hpp>

#include "nr3.h"
#include "ludcmp.h"
#include "qrdcmp.h"
#include "roots_multidim.h"
#include "quadrature.h"

// User function for evaluating the objective function
struct usrfun {
    // Constructor
    usrfun(double y, double a, double L, double E, double I) : _y(y), _a(a), _L(L), _E(E), _I(I), _f(3) {}

    // Function evaluation
    const VecDoub& operator()(const VecDoub& x) {
      const double &F = x[0], &M = x[1], &z = x[2];
      _f[0] = _y - ( ((z - 3.0*_L)*F + 3*M)*z*z ) / (6.0*_E*_I);
      _f[1] = _a - std::atan( ( ((z - 2.0*_L)*F + 2.0*M)*z ) / (2.0*_E*_I) );
      integrand integ(F, M, _L, _E, _I);
      _f[2] = _L - qsimp<integrand>(integ, 0.0, z);

      return _f;
    }

  private:
    // Data members
    double _y, _a;
    double _L, _E, _I;
    VecDoub _f;
    
    // Integrand for numerical integration
    struct integrand {
      double _F, _M, _Lint, _Eint, _Iint;
      integrand(double F, double M, double L, double E, double I) : _F(F), _M(M), _Lint(L), _Eint(E), _Iint(I) {}
      double operator()(double z) {
        const double dydz = ( ((z - 2.0*_Lint)*_F+ 2.0*_M)*z ) / (2.0*_Eint*_Iint);
        
        return std::sqrt(1.0 + dydz*dydz);
      }
    };
};

namespace rw { namespace models {

BeamJoint::BeamJoint(const std::string& name, const rw::math::Transform3D<>& transform) :
                     Joint(name, 2), _transform(transform), _L(1), _E(1), _I(1)
{
  
}

BeamJoint::~BeamJoint()
{
}

rw::math::Transform3D<> BeamJoint::getJointTransform(const rw::math::Q& q) const
{
  const std::vector<double> parms = solveParameters(q);
  
  const double &F = parms[0], &M = parms[1], &z = parms[2];

  return getJointTransform(F, M, z);
}

std::vector<double> BeamJoint::solveParameters(const rw::math::Q& q) const
{
  // Control input (translation and angle)
  const double &y = q[0], &a = q[1];
  
  // Starting guess
  const double F = 0.0, M = 0.0, z = _L;
  // Solution vector
  VecDoub x(3);
  x[0] = F; x[1] = M; x[2] = z;
  // Objective function evaluator
  usrfun f(y, a, _L, _E, _I);
  
  bool check;
  try {
    newt<usrfun>(x, check, f);
  } catch(...) {
    check = true;
  }
  if(check)
    RW_WARN("Exception caught from Newton solver - inaccurate solution for beam joint expected!");
  
  // Solution (force F, moment M and projected length z)
  std::vector<double> sol(3);
  sol[0] = x[0]; sol[1] = x[1]; sol[2] = x[2];
  
  return sol;
}

rw::math::Transform3D<> BeamJoint::getJointTransform(double F, double M, double z) const
{
  // Deflection at z with F and M acting at the tip
  const double y = deflection(F, M, z); 
  
  // Position
  const rw::math::Vector3D<> P(0.0, y, z);
  
  // Rotation
  const double a = angle(F, M, z);
  const double ca = std::cos(a), sa = std::sin(a);
  const rw::math::Rotation3D<> R(1.0, 0.0, 0.0, 0.0, ca, -sa, 0.0, sa, ca);
  
  return _transform * rw::math::Transform3D<>(P, R);
}

/* 
 * joint: the transform of this joint (beam tip) seen from external frame
 * tcp: the transform of TCP seen from external frame
 */ 
void BeamJoint::getJacobian(size_t row,
                            size_t col,
                            const rw::math::Transform3D<>& joint,
                            const rw::math::Transform3D<>& tcp,
                            const rw::kinematics::State& state,
                            rw::math::Jacobian& jacobian) const
{
}

void BeamJoint::setBounds(const std::pair<const math::Q, const math::Q>& bounds)
{
  if(bounds.first[0] < -45.0*rw::math::Deg2Rad || bounds.second[0] > 45.0*rw::math::Deg2Rad ||
     bounds.first[1] < -45.0*rw::math::Deg2Rad || bounds.second[1] > 45.0*rw::math::Deg2Rad)
    RW_THROW("Beam joint bound out of range - must be between -45 and 45 degrees.");
  
  Joint::setBounds(bounds);
}

}}
