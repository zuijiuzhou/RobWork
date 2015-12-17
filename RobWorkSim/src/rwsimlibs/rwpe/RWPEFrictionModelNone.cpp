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

#include "RWPEFrictionModelNone.hpp"

using namespace rw::common;
using namespace rw::math;
using namespace rw::kinematics;
using namespace rwsimlibs::rwpe;

RWPEFrictionModelNone::RWPEFrictionModelNone() {
}

RWPEFrictionModelNone::~RWPEFrictionModelNone() {
}

const RWPEFrictionModel* RWPEFrictionModelNone::withProperties(const PropertyMap &map) const {
	return this;
}

RWPEFrictionModel::DryFriction RWPEFrictionModelNone::getDryFriction(const RWPEContact& contact, const RWPEIslandState& islandState, const State& rwstate, const RWPEFrictionModelData* data) const {
	return DryFriction();
}

Wrench6D<> RWPEFrictionModelNone::getViscuousFriction(const RWPEContact& contact, const RWPEIslandState& islandState, const State& rwstate, const RWPEFrictionModelData* data) const {
	return Wrench6D<>(Vector3D<>::zero(),Vector3D<>::zero());
}