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
 
 

#ifndef RW_ALGORITHMS_PlaneModel_HPP
#define RW_ALGORITHMS_PlaneModel_HPP



/**
 * @file PlaneModel.hpp
 */

#include <rw/math/Vector3D.hpp>
#include <rw/geometry/Plane.hpp>

#include "RANSACModel.hpp"



namespace rwlibs { namespace algorithms {



/**
 * @brief A plane constraint model.
 * 
 * Describes a plane constraint, i.e. a surface.
 */
class PlaneModel : public RANSACModel<rw::math::Transform3D<> > {
	public:
		//! @brief Smart pointer type to this class.
		typedef rw::common::Ptr<PlaneModel> Ptr;
		
		//! @copydoc ConstraintModel::MinSamples
		static const int MinSamples = 3;
		
	public: // constructors
		/**
		 * @brief Constructor.
		 */
		PlaneModel() :
			_model(rw::math::Vector3D<>(), rw::math::Vector3D<>::x(), rw::math::Vector3D<>::y())
		{};
		
		//! @brief Destructor.
		virtual ~PlaneModel() {};
		
		//! @brief Create a model from a set of samples.
		static PlaneModel& make(const std::vector<rw::math::Transform3D<> >& data)
		{
			PlaneModel::Ptr model = new PlaneModel();
			
			model->_data = data;
			model->refit(data);
			
			return *model;
		}

	public: // methods
		//! @copydoc RANSACModel::fitError
		virtual double fitError(rw::math::Transform3D<> sample) const;
		
		//! @copydoc RANSACModel::invalid
		virtual bool invalid() const;
		
		//! @copydoc RANSACModel::refit
		virtual double refit(const std::vector<rw::math::Transform3D<> >& samples);
		
		//! @copydoc RANSACModel::getMinReqData
		static int getMinReqData() { return MinSamples; }
		
		//! @copydoc RANSACModel::same
		virtual bool same(const PlaneModel& model, double threshold) const;
		
		/**
		 * @brief Streaming operator.
		 */
		friend std::ostream& operator<<(std::ostream& out, const PlaneModel& plane)
		{
			return out << plane._model;
		}
	
	protected: // body
		rw::geometry::Plane _model;
};



}} // /namespaces

#endif // include guard
