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

#ifndef RW_MATH_JACOBIAN_HPP
#define RW_MATH_JACOBIAN_HPP

/**
 * @file math/Jacobian.hpp
 */

#if !defined(SWIG)
#include <rw/common/Serializable.hpp>
#include <rw/math/Q.hpp>
#include <rw/math/Rotation3D.hpp>
#include <rw/math/Transform3D.hpp>
#include <rw/math/VelocityScrew6D.hpp>

#include <Eigen/Core>
#endif
namespace rw {
namespace math {

    /** @addtogroup math */
    /*@{*/

    /**
     * @brief A Jacobian class. A jacobian with m rows and n columns.
     *
     * An ordinary robot jacobian defined over the joints 0 to n with
     * configuration \b q is expressed as a @f$ 6\times n @f$ matrix:
     * \f[
     * \robabx{0}{n}{\bf{J}}(\bf{q}) = [
     * \robabx{0}{1}{\bf{J}}(\bf{q}),
     * \robabx{1}{2}{\bf{J}}(\bf{q}),...,
     * \robabx{n-1}{n}{\bf{J}}(\bf{q}) ]
     * \f]
     *
     */
    class Jacobian
    {
      public:
        //! @brief The type of the internal Eigen matrix implementation.
        typedef Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic > Base;

        /**
         * @brief Creates an empty @f$ m\times n @f$ (uninitialized) Jacobian matrix
         *
         * @param m [in] number of rows
         *
         * @param n [in] number of columns
         */
        Jacobian (size_t m, size_t n) : _jac (m, n) {}

        /**
         * @brief Default constructor
         */
        Jacobian () {}

        /**
           @brief The number of rows.
         */
        size_t size1 () const { return _jac.rows (); }

        /**
           @brief The number of columns.
         */
        size_t size2 () const { return _jac.cols (); }

        /**
         * @brief Creates an empty @f$ 6\times n @f$ (uninitialized) Jacobian matrix
         *
         * @param n [in] number of columns
         */
        explicit Jacobian (size_t n) : _jac (6, n) {}

        /**
         * @brief Creates a Jacobian from a Eigen::MatrixBase
         *
         * @param r [in] an Eigen Matrix
         */
        template< class R > explicit Jacobian (const Eigen::MatrixBase< R >& r) : _jac (r) {}

        /**
         * @brief Construct zero initialized Jacobian.
         * @param size1 [in] number of rows.
         * @param size2 [in] number of columns.
         * @return zero-initialized jacobian.
         */
        static Jacobian zero (size_t size1, size_t size2)
        {
            return Jacobian (
                Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic >::Zero (size1, size2));
        }

        /**
         * @brief Accessor for the internal Eigen matrix state.
         */
        Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic >& e () { return _jac; }

        /**
         * @brief Accessor for the internal Eigen matrix state.
         */
        const Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic >& e () const { return _jac; }

#if !defined(SWIG)
        /**
         * @brief Returns reference to matrix element
         * @param row [in] row
         * @param column [in] column
         * @return reference to the element
         */
        double& operator() (size_t row, size_t column) { return _jac (row, column); }

        /**
         * @brief Returns reference to matrix element
         * @param row [in] row
         * @param column [in] column
         * @return reference to the element
         */
        const double& operator() (size_t row, size_t column) const { return _jac (row, column); }
#else
        MATRIXOPERATOR (double);
#endif
        /**
         * @brief Get an element of the jacobian.
         * @param row [in] the row.
         * @param col [in] the column.
         * @return reference to the element.
         */
        double& elem (size_t row, size_t col) { return _jac (row, col); }

#if !defined(SWIGJAVA)

        /**
         * @brief Creates the velocity transform jacobian
         * @f$ \robabcdx{a}{b}{a}{b}{\bf{J_v}} @f$
         * for transforming both the reference frame and the velocity
         * reference point from one frame \b b to another frame \b a
         *
         * @param aTb [in] @f$ \robabx{a}{b}{\bf{T}} @f$
         *
         * @return @f$ \robabcdx{a}{b}{a}{b}{\bf{J_v}} @f$
         *
         * \f[
         * \robabcdx{a}{b}{a}{b}{\bf{J_v}} =
         * \left[
         *  \begin{array}{cc}
         *    \robabx{a}{b}{\mathbf{R}} & S(\robabx{a}{b}{\mathbf{d}})\robabx{a}{b}{\mathbf{R}} \\
         *    \mathbf{0}^{3x3} & \robabx{a}{b}{\mathbf{R}}
         *  \end{array}
         * \right]
         * \f]
         *
         * Change the frame of reference from \b b to frame \b a and reference point
         * from frame \b a to frame \b b:
         * @f$ \robabx{a}{b}{\bf{J}} =  \robabcdx{a}{b}{a}{b}{\bf{J}_v} \cdot \robabx{b}{a}{\bf{J}}
         * @f$
         */

#endif 
        explicit Jacobian (const rw::math::Transform3D<double>& aTb);

#if !defined(SWIGJAVA)
        /**
         * @brief Creates the velocity transform jacobian
         * @f$ \robabcdx{a}{b}{i}{i}{\bf{J_v}} @f$
         * for transforming a velocity screw from one frame of reference \b b to
         * another frame \b a
         *
         * @param aRb [in] @f$ \robabx{a}{b}{\bf{R}} @f$
         *
         * @return @f$ \robabcdx{a}{b}{i}{i}{\bf{J}_v} @f$
         *
         * \f[
         * \robabcdx{a}{b}{i}{i}{\bf{J_v}} =
         * \left[
         *  \begin{array}{cc}
         *    \robabx{a}{b}{\mathbf{R}} & \mathbf{0}^{3x3} \\
         *    \mathbf{0}^{3x3} & \robabx{a}{b}{\mathbf{R}}
         *  \end{array}
         * \right]
         * \f]
         *
         * Change the frame of reference from \b b to frame \b a :
         * @f$ \robabx{a}{c}{\bf{J}} =  \robabcdx{a}{b}{c}{c}{\bf{J}_v} \cdot \robabx{b}{c}{\bf{J}}
         * @f$
         *
         */

#endif 
        explicit Jacobian (const rw::math::Rotation3D<>& aRb);

#if !defined(SWIGJAVA)

        /**
         * @brief Creates the velocity transform jacobian
         * @f$ \robabcdx{i}{i}{b}{a}{\bf{J}_v} @f$
         * for transforming the reference point of a velocity screw from one
         * frame \b b to another frame \b a
         *
         * @param aPb [in] @f$ \robabx{a}{b}{\bf{P}} @f$
         *
         * @return @f$ \robabcdx{i}{i}{b}{a}{\bf{J}_v} @f$
         *
         * \f[
         * \robabcdx{i}{i}{b}{a}{\bf{J}_v} =
         * \left[
         *  \begin{array}{cc}
         *    \bf{I}^{3x3} & S(\robabx{a}{b}{\bf{P}}) \\
         *    \bf{0}^{3x3} & \bf{I}^{3x3}
         *  \end{array}
         * \right]
         * \f]
         *
         *  transforming the reference point of a Jacobian from
         * frame \b c to frame \b d :
         * @f$ \robabx{a}{d}{\mathbf{J}} =  \robabcdx{a}{a}{c}{d}{\mathbf{J_v}} \cdot
         * \robabx{a}{c}{\mathbf{J}} @f$
         */

#endif
        explicit Jacobian (const rw::math::Vector3D<>& aPb);

        /**
         * @brief add rotation jacobian to a specific row and column in this jacobian
         * @param part
         * @param row
         * @param col
         */
        void addRotation (const rw::math::Vector3D<>& part, size_t row, size_t col);

        /**
         * @brief add position jacobian to a specific row and column in this jacobian
         * @param part
         * @param row
         * @param col
         */
        void addPosition (const rw::math::Vector3D<>& part, size_t row, size_t col);
        
#if !defined(SWIG)
        /**
       * @brief Streaming operator.
    */
    friend inline std::ostream& operator<< (std::ostream& out, const Jacobian& v) { return out << v.e (); }
#else
        TOSTRING(rw::math::Jacobian);
#endif
      private:
        Base _jac;
    };

    /**
     * @brief Calculates velocity vector
     * @param Jq [in] the jacobian @f$ \mathbf{J}_{\mathbf{q}} @f$
     * @param dq [in] the joint velocity vector @f$ \dot{\mathbf{q}} @f$
     * @return the velocity vector @f$ \mathbf{\nu} @f$
     * @relates Jacobian
     */
    inline const rw::math::VelocityScrew6D<> operator* (const Jacobian& Jq, const rw::math::Q& dq)
    {
        return rw::math::VelocityScrew6D<> (Jq.e () * dq.e ());
    }

    /**
     * @brief Calculates joint velocities
     *
     * @param JqInv [in] the inverse jacobian @f$ \mathbf{J}_{\mathbf{q}}^{-1} @f$
     *
     * @param v [in] the velocity vector @f$ \mathbf{\nu} @f$
     *
     * @return the joint velocity vector @f$ \dot{\mathbf{q}} @f$
     *
     * @relates Jacobian
     */
    inline const rw::math::Q operator* (const Jacobian& JqInv, const rw::math::VelocityScrew6D<>& v)
    {
        return rw::math::Q (JqInv.e () * v.e ());
        // prod(JqInv.m(), v.m()));
    }

    /**
     * @brief Multiplies jacobians @f$ \mathbf{J} = \mathbf{J}_1 *
     * \mathbf{J}_2 @f$
     *
     * @param j1 [in] @f$ \mathbf{J}_1 @f$
     *
     * @param j2 [in] @f$ \mathbf{J}_2 @f$
     *
     * @return @f$ \mathbf{J} @f$
     *
     * @relates Jacobian
     */
    inline const Jacobian operator* (const Jacobian& j1, const Jacobian& j2)
    {
        return Jacobian (j1.e () * j2.e ());
        // return Jacobian(prod(j1.m(), j2.m()));
    }

    /**
       @brief Rotates each column of \b v by \b r.

       The Jacobian must be of height 6.

       @relates Jacobian
    */
    const Jacobian operator* (const rw::math::Rotation3D<>& r, const Jacobian& v);

    /*@}*/
}
}    // namespace rw::math

namespace rw { namespace common {
    class OutputArchive;
    class InputArchive;
    namespace serialization {
        /**
         * @copydoc rw::common::serialization::write
         * @relatedalso rw::math::Jacobian
         */
        template<>
        void write (const rw::math::Jacobian& sobject, rw::common::OutputArchive& oarchive,
                    const std::string& id);

        /**
         * @copydoc rw::common::serialization::read
         * @relatedalso rw::math::Jacobian
         */
        template<>
        void read (rw::math::Jacobian& sobject, rw::common::InputArchive& iarchive,
                   const std::string& id);
    }    // namespace serialization
}}       // namespace rw::common

#endif    // end include guard
