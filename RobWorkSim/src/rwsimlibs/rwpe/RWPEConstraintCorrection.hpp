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

#ifndef RWSIMLIBS_RWPE_RWPECONSTRAINTCORRECTION_HPP_
#define RWSIMLIBS_RWPE_RWPECONSTRAINTCORRECTION_HPP_

/**
 * @file RWPEConstraintCorrection.hpp
 *
 * \copydoc rwsimlibs::rwpe::RWPEConstraintCorrection
 */

#include <list>

#include <Eigen/Eigen>

namespace rw { namespace common { class PropertyMap; } }

namespace rwsimlibs {
namespace rwpe {
//! @addtogroup rwsimlibs_rwpe

//! @{
/**
 * @brief Correction of the position and orientation of bodies such that positional errors in constraints and
 * contacts are reduced or eliminated.
 *
 * The correction algorithm uses the properties described in #addDefaultProperties .
 */
class RWPEConstraintCorrection {
public:
	//! @brief Constructor.
	RWPEConstraintCorrection();

	//! @brief Destructor.
	virtual ~RWPEConstraintCorrection();

	/**
	 * @brief Do the correction.
	 * @param constraints [in] vector of constraints to correct.
	 * @param properties [in] a reference to the PropertyMap.
	 * @param state [in/out] the state that will be updated.
	 * @param log [in/out] the log utility to log to (optional).
	 */
	virtual void correct(
			const std::list<class RWPEConstraint*>& constraints,
			class RWPEIslandState& state,
			const rw::common::PropertyMap& properties,
			class RWPELogUtil* log = NULL) const;

	/**
	 * @brief Insert the properties used by default into property map.
	 *
	 *  Property Name                | Type   | Default value          | Description
	 *  ---------------------------- | ------ | ---------------------- | -----------
	 *  RWPECorrectionContactLayer    | double | \f$0.0005\f$           | The maximum penetration allowed in a contact after correction (in meters).
	 *
	 * @param properties [in/out] PropertyMap to add properties to.
	 */
	static void addDefaultProperties(rw::common::PropertyMap& properties);

private:
	static Eigen::VectorXd minimize(
			const Eigen::MatrixXd& Ae,
			const Eigen::VectorXd& be,
			const Eigen::MatrixXd& Ai,
			const Eigen::VectorXd& bi,
			const std::vector<bool>& hasContacts,
			double svdPrecision,
			double layer,
			class RWPELogUtil* log = NULL);
};
//! @}
} /* namespace rwpe */
} /* namespace rwsimlibs */
#endif /* RWSIMLIBS_RWPE_RWPECONSTRAINTCORRECTION_HPP_ */