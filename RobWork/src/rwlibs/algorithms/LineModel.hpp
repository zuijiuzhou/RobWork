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

#ifndef RW_ALGORITHMS_LineModel_HPP
#define RW_ALGORITHMS_LineModel_HPP

/**
 * @file LineModel.hpp
 */

#include <rwlibs/algorithms/RANSACModel.hpp>

#include <rw/core/Ptr.hpp>
#include <rw/geometry/Line.hpp>
#include <rw/math/Vector3D.hpp>

namespace rwlibs { namespace algorithms {

    /**
     * @brief A line model.
     */
    class LineModel : public RANSACModel< LineModel, rw::math::Vector3D<> >
    {
      public:
        //! @brief Smart pointer type to this class.
        typedef rw::core::Ptr< LineModel > Ptr;

      public:    // constructors
        /**
         * @brief Constructor.
         */
        LineModel () {}

        /**
         * @brief Constructor.
         */
        LineModel (const rw::geometry::Line& line) : _model (line) {}

        //! @brief Destructor.
        virtual ~LineModel () {}

      public:    // methods
        //! @copydoc RANSACModel::fitError
        virtual double fitError (const rw::math::Vector3D<>& sample) const;

        //! @copydoc RANSACModel::invalid
        virtual bool invalid () const;

        /**
         * @copydoc RANSACModel::getMinReqData
         *
         * LineModel requires at least 2 sample.
         */
        virtual int getMinReqData () const { return 2; }

        //! @copydoc RANSACModel::refit
        virtual double refit (const std::vector< rw::math::Vector3D<> >& samples);

        /**
         * @copydoc RANSACModel::same
         *
         * LineModels are the same when the distance between them, according
         * to metric taking into account weighted sum of direction angle difference
         * and the closest separation between lines is lower than specified threshold.
         */
        virtual bool same (const LineModel& model, double threshold) const;

        //! @brief Get line.
        rw::geometry::Line line () const { return _model; }

        /**
         * @brief Streaming operator.
         */
        friend std::ostream& operator<< (std::ostream& out, const LineModel& model)
        {
            return out << model._model;
        }

      protected:    // body
        rw::geometry::Line _model;
    };

}}    // namespace rwlibs::algorithms

#endif    // include guard
