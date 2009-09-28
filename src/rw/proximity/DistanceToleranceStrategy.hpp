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


#ifndef RW_PROXIMITY_DISTANCETOLERANCESTRATEGY_HPP
#define RW_PROXIMITY_DISTANCETOLERANCESTRATEGY_HPP
/**
 * @file DistanceStrategy.hpp
 */

#include <rw/kinematics/Frame.hpp>
#include <rw/math/Transform3D.hpp>
#include <rw/geometry/Face.hpp>

#include "ProximityStrategy.hpp"

namespace rw { namespace proximity {

    /** @addtogroup proximity */
    /*@{*/


	/**
	 * @brief DistanceResult contains basic information about the distance
	 * result between two frames.
	 */
	struct MultiDistanceResult {
		 //! @brief reference to the first frame
		const kinematics::Frame* f1;

		//! @brief reference to the second frame
		const kinematics::Frame* f2;

		// TODO: is this correct?? vector pointing along shortest distance axis from f1 towards f2

		//! Closest point on f1 to f2, described in f1 reference frame
		math::Vector3D<double> p1;

		//! Closest point on f2 to f1, described in f2 reference frame
		math::Vector3D<double> p2;

		//! @brief distance between frame f1 and frame f2
		double distance;

        //! Closest points on f1 to f2, described in f1 reference frame
		std::vector< math::Vector3D<> > p1s;

        //! IMPORTANT! NOTICE! VERY UGLY: Closest point on f2 to f1, described in >>>> \b f1 <<<<< reference frame
		std::vector< math::Vector3D<> > p2s;

		std::vector< double > distances;
	};


    /**
     * @brief The DistanceStrategy interface is used to abstract away
     * specific collision detection algorithms or strategies.
     */
    class DistanceToleranceStrategy: public virtual ProximityStrategy {

    public:
        /**
         * @brief Destroys object
         */
        virtual ~DistanceToleranceStrategy();

        /**
         * @brief Calculates the distance between two given frames @f$ \mathcal{F}_a @f$ and
         * @f$ \mathcal{F}_b @f$
         * @param result [out] MultiDistanceResult to copy result into
         * @param a [in] @f$ \mathcal{F}_a @f$
         * @param wTa [in] @f$ \robabx{w}{a}{\mathbf{T}} @f$
         * @param b [in] @f$ \mathcal{F}_b @f$
         * @param wTb [in] @f$ \robabx{w}{b}{\mathbf{T}} @f$
         * @param tolerance [in] point pairs that are closer than tolerance will
         * be included in the result.
         * @param rel_err [in] relative acceptable error
         * @param abs_err [in] absolute acceptable error
         *
         * @return shortest distance if @f$ \mathcal{F}_a @f$ and @f$ \mathcal{F}_b @f$ are
         * separated and not in collision.
         */
        virtual bool getDistances(
            MultiDistanceResult &result,
            const kinematics::Frame* a,
            const math::Transform3D<>& wTa,
            const kinematics::Frame* b,
            const math::Transform3D<>& wTb,
            double tolerance,
            double rel_err = 0.0,
            double abs_err = 0.0);

        virtual bool calcDistances(
            MultiDistanceResult &result,
            ProximityModelPtr a,
            const math::Transform3D<>& wTa,
            ProximityModelPtr b,
            const math::Transform3D<>& wTb,
            double tolerance,
            double rel_err = 0.0,
            double abs_err = 0.0) = 0;

    private:
        DistanceToleranceStrategy(const DistanceToleranceStrategy&);
        DistanceToleranceStrategy& operator=(const DistanceToleranceStrategy&);

    protected:
        /**
         * @brief Creates object
         */
        DistanceToleranceStrategy();
    };

    /*@}*/
}} // end namespaces

#endif /* RW_PROXIMITY_DISTANCETOLERANCESTRATEGY_HPP */
