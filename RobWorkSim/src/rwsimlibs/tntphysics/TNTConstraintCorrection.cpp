/********************************************************************************
 * Copyright 2014 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#include "TNTConstraintCorrection.hpp"
#include "TNTConstraint.hpp"
#include "TNTIslandState.hpp"

#include "TNTRigidBody.hpp"
#include "TNTContact.hpp"

#include <rw/kinematics/State.hpp>

using namespace rw::kinematics;
using namespace rw::math;
using namespace rwsimlibs::tntphysics;

TNTConstraintCorrection::TNTConstraintCorrection() {
}

TNTConstraintCorrection::~TNTConstraintCorrection() {
}

void TNTConstraintCorrection::correct(const std::list<TNTConstraint*>& constraints, TNTIslandState& tntstate, const State& rwstate) const {
	unsigned int nrOfConstraints = 0;
	std::map<const TNTBody*, unsigned int> bodies;
	std::vector<const TNTRigidBody*> idToBody;
	unsigned int id = 0;
	BOOST_FOREACH(const TNTConstraint* const constraint, constraints) {
		if (constraint->getDimVelocity() == 0)
			continue;
		const TNTRigidBody* const rParent = dynamic_cast<const TNTRigidBody*>(constraint->getParent());
		const TNTRigidBody* const rChild = dynamic_cast<const TNTRigidBody*>(constraint->getChild());
		const TNTContact* const contact = dynamic_cast<const TNTContact*>(constraint);
		if (rParent) {
			if (bodies.find(rParent) == bodies.end()) {
				bodies[rParent] = id;
				idToBody.push_back(rParent);
				id++;
			}
		}
		if (rChild) {
			if (bodies.find(rChild) == bodies.end()) {
				bodies[rChild] = id;
				idToBody.push_back(rChild);
				id++;
			}
		}
		if (contact)
			nrOfConstraints += 1;
		else {
			const std::vector<TNTConstraint::Mode> modes = constraint->getConstraintModes();
			for (std::size_t i = 0; i < 6; i++) {
				if (modes[i] == TNTConstraint::Velocity) {
					nrOfConstraints++;
				}
			}
		}
	}

	if (nrOfConstraints == 0 || id == 0)
		return;

	unsigned int curConstraint = 0;
	Eigen::MatrixXd lhs = Eigen::MatrixXd::Zero(nrOfConstraints,6*id);
	Eigen::VectorXd rhs = Eigen::VectorXd::Zero(nrOfConstraints);
	BOOST_FOREACH(const TNTConstraint* const constraint, constraints) {
		if (constraint->getDimVelocity() == 0)
			continue;
		const TNTRigidBody* const rParent = dynamic_cast<const TNTRigidBody*>(constraint->getParent());
		const TNTRigidBody* const rChild = dynamic_cast<const TNTRigidBody*>(constraint->getChild());
		const TNTContact* const contact = dynamic_cast<const TNTContact*>(constraint);
		if (!rParent && !rChild)
			continue;
		if (contact) {
			const Vector3D<> rij = contact->getPositionParentW(tntstate);
			const Vector3D<> rji = contact->getPositionChildW(tntstate);
			const Vector3D<> nij = contact->getLinearRotationParentW(tntstate).getCol(2);
			rhs[curConstraint] = -contact->getContact().getDepth();
			if (rParent) {
				const Vector3D<> Ri = rParent->getWorldTcom(tntstate).P();
				const unsigned int bodyId = bodies[rParent]*6;
				lhs(curConstraint,bodyId+0) = nij[0];
				lhs(curConstraint,bodyId+1) = nij[1];
				lhs(curConstraint,bodyId+2) = nij[2];
				const Vector3D<> ang(-nij.e().transpose()*Math::skew(rij-Ri));
				lhs(curConstraint,bodyId+3) = ang[2];
				lhs(curConstraint,bodyId+4) = ang[2];
				lhs(curConstraint,bodyId+5) = ang[2];
			}
			if (rChild) {
				const Vector3D<> Rj = rChild->getWorldTcom(tntstate).P();
				const unsigned int bodyId = bodies[rChild]*6;
				lhs(curConstraint,bodyId+0) = -nij[0];
				lhs(curConstraint,bodyId+1) = -nij[1];
				lhs(curConstraint,bodyId+2) = -nij[2];
				const Vector3D<> ang(nij.e().transpose()*Math::skew(rji-Rj));
				lhs(curConstraint,bodyId+3) = ang[2];
				lhs(curConstraint,bodyId+4) = ang[2];
				lhs(curConstraint,bodyId+5) = ang[2];
			}
			curConstraint += 1;
		} else {
			const Vector3D<> rij = constraint->getPositionParentW(tntstate);
			const Vector3D<> rji = constraint->getPositionChildW(tntstate);
			const Rotation3D<> rotI = constraint->getAngularRotationParentW(tntstate);
			const Rotation3D<> rotJ = constraint->getAngularRotationChildW(tntstate);
			const EAA<> rotDifEAA(inverse(rotJ)*rotI);
			const Vector3D<> rotDif = rotDifEAA.angle()*rotDifEAA.axis();
			const Rotation3D<> linR = constraint->getLinearRotationParentW(tntstate);
			const std::vector<TNTConstraint::Mode> modes = constraint->getConstraintModes();
			std::size_t constraintID = curConstraint;
			for (std::size_t i = 0; i < 3; i++) {
				if (modes[i] == TNTConstraint::Velocity) {
					const Vector3D<> dir = linR.getCol(i);
					rhs[constraintID] = dot(rji-rij,dir);
					constraintID++;
				}
			}
			for (std::size_t i = 3; i < 6; i++) {
				if (modes[i] == TNTConstraint::Velocity) {
					const Vector3D<> dir = rotI.getCol(i-3);
					rhs[constraintID] = dot(rotDif,dir);
					constraintID++;
				}
			}
			if (rParent) {
				const Vector3D<> Ri = rParent->getWorldTcom(tntstate).P();
				const unsigned int bodyId = bodies[rParent]*6;
				constraintID = curConstraint;
				for (std::size_t i = 0; i < 3; i++) {
					if (modes[i] == TNTConstraint::Velocity) {
						const Vector3D<> dir = linR.getCol(i);
						lhs(constraintID,bodyId+0) = dir[0];
						lhs(constraintID,bodyId+1) = dir[1];
						lhs(constraintID,bodyId+2) = dir[2];
						const Vector3D<> ang(-dir.e().transpose()*Math::skew(rij-Ri));
						lhs(constraintID,bodyId+3) = ang[0];
						lhs(constraintID,bodyId+4) = ang[1];
						lhs(constraintID,bodyId+5) = ang[2];
						constraintID++;
					}
				}
				for (std::size_t i = 3; i < 6; i++) {
					if (modes[i] == TNTConstraint::Velocity) {
						lhs(constraintID,bodyId+0) = 0;
						lhs(constraintID,bodyId+1) = 0;
						lhs(constraintID,bodyId+2) = 0;
						const Vector3D<> dir = rotI.getCol(i-3);
						lhs(constraintID,bodyId+3) = dir[0];
						lhs(constraintID,bodyId+4) = dir[1];
						lhs(constraintID,bodyId+5) = dir[2];
						constraintID++;
					}
				}
			}
			if (rChild) {
				const Vector3D<> Rj = rChild->getWorldTcom(tntstate).P();
				const unsigned int bodyId = bodies[rChild]*6;
				constraintID = curConstraint;
				for (std::size_t i = 0; i < 3; i++) {
					if (modes[i] == TNTConstraint::Velocity) {
						const Vector3D<> dir = linR.getCol(i);
						lhs(constraintID,bodyId+0) = -dir[0];
						lhs(constraintID,bodyId+1) = -dir[1];
						lhs(constraintID,bodyId+2) = -dir[2];
						const Vector3D<> ang(dir.e().transpose()*Math::skew(rji-Rj));
						lhs(constraintID,bodyId+3) = ang[0];
						lhs(constraintID,bodyId+4) = ang[1];
						lhs(constraintID,bodyId+5) = ang[2];
						constraintID++;
					}
				}
				for (std::size_t i = 3; i < 6; i++) {
					if (modes[i] == TNTConstraint::Velocity) {
						lhs(constraintID,bodyId+0) = 0;
						lhs(constraintID,bodyId+1) = 0;
						lhs(constraintID,bodyId+2) = 0;
						const Vector3D<> dir = -rotI.getCol(i-3);
						lhs(constraintID,bodyId+3) = dir[0];
						lhs(constraintID,bodyId+4) = dir[1];
						lhs(constraintID,bodyId+5) = dir[2];
						constraintID++;
					}
				}
			}
			for (std::size_t i = 0; i < 6; i++) {
				if (modes[i] == TNTConstraint::Velocity) {
					curConstraint++;
				}
			}
		}
	}

	const Eigen::MatrixXd lhsInv = LinearAlgebra::pseudoInverse(lhs,1e-12);
	const Eigen::VectorXd sol = lhsInv*rhs;
	for (unsigned int i = 0; i < id; i++) {
		const Eigen::VectorXd correction = sol.block(i*6,0,6,1);
		const TNTRigidBody* body = idToBody[i];
		Transform3D<> wTcom = body->getWorldTcom(tntstate);
		wTcom.P() += Vector3D<>(correction.block(0,0,3,1));
		wTcom.R() = EAA<>(Vector3D<>(correction.block(3,0,3,1))).toRotation3D()*wTcom.R();
		body->setWorldTcom(wTcom,tntstate);
	}
}