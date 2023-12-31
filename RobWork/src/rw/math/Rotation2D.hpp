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

#ifndef RW_MATH_ROTATION2D_HPP
#define RW_MATH_ROTATION2D_HPP

/**
 * @file Rotation2D.hpp
 */

#if !defined(SWIG)
#include <rw/math/Vector2D.hpp>

#include <rw/common/Serializable.hpp>

#include <Eigen/Core>
#endif

namespace rw { namespace math {

    template< class T > class Rotation2DVector;

    /** @addtogroup math */
    /* @{*/

#if !defined(SWIGJAVA)

    /**
     * @brief A 2x2 rotation matrix \f$ \mathbf{R}\in SO(2) \f$
     *
     * @f$
     *  \mathbf{R}=
     *  \left[
     *  \begin{array}{cc}
     *  {}^A\hat{X}_B & {}^A\hat{Y}_B
     *  \end{array}
     *  \right]
     *  =
     *  \left[
     *  \begin{array}{cc}
     *  r_{11} & r_{12} \\
     *  r_{21} & r_{22}
     *  \end{array}
     *  \right]
     * @f$
     */
#endif
    template< class T = double > class Rotation2D
    {
      public:
        //! Value type.
        typedef T value_type;

        //! The type of the internal Boost matrix implementation.
        typedef Eigen::Matrix< T, 2, 2 > EigenMatrix2x2;

        /**
           @brief A rotation matrix with uninitialized storage.
         */
        Rotation2D () {}

#if !defined(SWIGJAVA)

        /**
         * @brief Constructs an initialized 2x2 rotation matrix
         *
         * @param r11 \f$ r_{11} \f$
         * @param r12 \f$ r_{12} \f$
         * @param r21 \f$ r_{21} \f$
         * @param r22 \f$ r_{22} \f$
         *
         * @f$
         *  \mathbf{R} =
         *  \left[
         *  \begin{array}{cc}
         *  r_{11} & r_{12} \\
         *  r_{21} & r_{22}
         *  \end{array}
         *  \right]
         * @f$
         */

#endif
        Rotation2D (T r11, T r12, T r21, T r22)
        {
            _m[0][0] = r11;
            _m[0][1] = r12;
            _m[1][0] = r21;
            _m[1][1] = r22;
        }

#if !defined(SWIGJAVA)
        /**
         * @brief Constructs an initialized 2x2 rotation matrix
         * @f$ \robabx{a}{b}{\mathbf{R}} =
         * \left[
         *  \begin{array}{cc}
         *   \robabx{a}{b}{\mathbf{i}} & \robabx{a}{b}{\mathbf{j}}
         *  \end{array}
         * \right]
         * @f$
         *
         * @param i @f$ \robabx{a}{b}{\mathbf{i}} @f$
         * @param j @f$ \robabx{a}{b}{\mathbf{j}} @f$
         */

#endif
        Rotation2D (const rw::math::Vector2D< T >& i, const rw::math::Vector2D< T >& j)
        {
            _m[0][0] = i[0];
            _m[0][1] = j[0];
            _m[1][0] = i[1];
            _m[1][1] = j[1];
        }

#if !defined(SWIGJAVA)
        /**
         * @brief Constructs an initialized 2x2 rotation matrix
         * @f$ \robabx{a}{b}{\mathbf{R}} =
         * \left[
         *  \begin{array}{cc}
         *   \robabx{a}{b}{\mathbf{i}} & \robabx{a}{b}{\mathbf{j}}
         *  \end{array}
         * \right]
         * @f$
         *
         * @param theta
         */

#endif
        Rotation2D (const T theta)
        {
            _m[0][0] = cos (theta);
            _m[0][1] = -sin (theta);
            _m[1][0] = sin (theta);
            _m[1][1] = cos (theta);
        }

        /**
           @brief Construct an initialized 2x2 rotation matrix.

           The second of column of the matrix is deduced from the first column.

           @param i [in] The first column of the rotation matrix.
        */
        Rotation2D (const rw::math::Vector2D< T >& i)
        {
            _m[0][0] = i[0];
            _m[0][1] = -i[1];
            _m[1][0] = i[1];
            _m[1][1] = i[0];
        }

        /**
           @brief Construct a rotation matrix from an Eigen matrix.
         */
        template< class R > explicit Rotation2D (const EigenMatrix2x2& m)
        {
            _m[0][0] = m (0, 0);
            _m[0][1] = m (0, 1);
            _m[1][0] = m (1, 0);
            _m[1][1] = m (1, 1);
        }

#if !defined(SWIGJAVA)
        /**
         * @brief Constructs a 2x2 rotation matrix set to identity
         * @return a 2x2 identity rotation matrix
         *
         * @f$
         * \mathbf{R} =
         * \left[
         * \begin{array}{cc}
         * 1 & 0\\
         * 0 & 1
         * \end{array}
         * \right]
         * @f$
         */

#endif
        static const Rotation2D& identity ()
        {
            static Rotation2D id (1, 0, 0, 1);
            return id;
        }

#if !defined(SWIG)
        /**
         * @brief Returns reference to matrix element
         * @param row [in] row
         * @param column [in] column
         * @return reference to the element
         */
        T& operator() (size_t row, size_t column) { return _m[row][column]; }

        /**
         * @brief Returns reference to matrix element
         * @param row [in] row
         * @param column [in] column
         * @return reference to the element
         */
        const T& operator() (size_t row, size_t column) const { return _m[row][column]; }
#else
        MATRIXOPERATOR (T);
#endif
        /**
         * @brief Comparison operator.
         *
         * The comparison operator makes a element wise comparison.
         * Returns true only if all elements are equal.
         *
         * @param rhs [in] Rotation2D to compare with
         * @return True if equal.
         */
        bool operator== (const Rotation2D< T >& rhs) const
        {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (!(_m[i][j] == rhs (i, j))) {
                        return false;
                    }
                }
            }
            return true;
        }

        /**
         * @brief Comparison operator.
         *
         * The comparison operator makes a element wise comparison.
         * Returns true if any of the elements are different.
         *
         * @param rhs [in] Rotation2D to compare with
         * @return True if not equal.
         */
        bool operator!= (const Rotation2D< T >& rhs) const { return !(*this == rhs); }

        /**
         * @brief Returns a boost 2x2 matrix @f$ \mathbf{M}\in SO(2)
         * @f$ that represents this rotation
         *
         * @return @f$ \mathbf{M}\in SO(2) @f$
         */
        EigenMatrix2x2 e ()
        {
            EigenMatrix2x2 matrix;
            matrix (0, 0) = _m[0][0];
            matrix (0, 1) = _m[0][1];
            matrix (1, 0) = _m[1][0];
            matrix (1, 1) = _m[1][1];
            return matrix;
        }

        /**
         * @brief Calculates \f$ \robabx{a}{c}{\mathbf{R}} =
         * \robabx{a}{b}{\mathbf{R}} \robabx{b}{c}{\mathbf{R}} \f$
         *
         * @param aRb [in] \f$ \robabx{a}{b}{\mathbf{R}} \f$
         *
         * @param bRc [in] \f$ \robabx{b}{c}{\mathbf{R}} \f$
         *
         * @return \f$ \robabx{a}{c}{\mathbf{R}} \f$
         */
        const Rotation2D operator* (const Rotation2D& bRc) const
        {
            return Rotation2D ((*this) (0, 0) * bRc (0, 0) + (*this) (0, 1) * bRc (1, 0),
                               (*this) (0, 0) * bRc (0, 1) + (*this) (0, 1) * bRc (1, 1),
                               (*this) (1, 0) * bRc (0, 0) + (*this) (1, 1) * bRc (1, 0),
                               (*this) (1, 0) * bRc (0, 1) + (*this) (1, 1) * bRc (1, 1));
        }

        /**
         * @brief Calculates \f$ \robabx{a}{c}{\mathbf{v}} =
         * \robabx{a}{b}{\mathbf{R}} \robabx{b}{c}{\mathbf{v}} \f$
         *
         * @param aRb [in] \f$ \robabx{a}{b}{\mathbf{R}} \f$
         * @param bVc [in] \f$ \robabx{b}{c}{\mathbf{v}} \f$
         * @return \f$ \robabx{a}{c}{\mathbf{v}} \f$
         */
        const rw::math::Vector2D< T > operator* (const rw::math::Vector2D< T >& bVc) const
        {
            return rw::math::Vector2D< T > ((*this) (0, 0) * bVc (0) + (*this) (0, 1) * bVc (1),
                                            (*this) (1, 0) * bVc (0) + (*this) (1, 1) * bVc (1));
        }
#if !defined(SWIG)
        /**
         * @brief Writes rotation matrix to stream
         * @param os [in/out] output stream to use
         * @param r [in] rotation matrix to print
         * @return the updated output stream
         */
        friend std::ostream& operator<< (std::ostream& os, const Rotation2D& r)
        {
            return os << "Rotation2D {" << r (0, 0) << ", " << r (0, 1) << ", " << r (1, 0) << ", "
                      << r (1, 1) << "}";
        }
#else
        TOSTRING (rw::math::Rotation2D< T >);
#endif

      private:
        T _m[2][2];
        // Base _atrix;
    };

    /**
     * @brief Casts Rotation2D<T> to Rotation2D<Q>
     * @param rot [in] Rotation2D with type T
     * @return Rotation2D with type R
     */
    template< class R, class T > const Rotation2D< R > cast (const Rotation2D< T >& rot)
    {
        Rotation2D< R > res (Rotation2D< R >::identity ());
        for (size_t i = 0; i < 2; i++)
            for (size_t j = 0; j < 2; j++)
                res (i, j) = static_cast< R > (rot (i, j));
        return res;
    }

    /**
     * @brief The inverse @f$ \robabx{b}{a}{\mathbf{R}} =
     * \robabx{a}{b}{\mathbf{R}}^{-1} @f$ of a rotation matrix
     *
     * @relates Rotation2D
     *
     * @param aRb [in] the rotation matrix @f$ \robabx{a}{b}{\mathbf{R}} @f$
     *
     * @return the matrix inverse @f$ \robabx{b}{a}{\mathbf{R}} =
     * \robabx{a}{b}{\mathbf{R}}^{-1} @f$
     *
     * @f$ \robabx{b}{a}{\mathbf{R}} = \robabx{a}{b}{\mathbf{R}}^{-1} =
     * \robabx{a}{b}{\mathbf{R}}^T @f$
     */
    template< class T > const Rotation2D< T > inverse (const Rotation2D< T >& aRb)
    {
        return Rotation2D< T > (aRb (0, 0), aRb (1, 0), aRb (0, 1), aRb (1, 1));
    }

    /**
     * @brief Find the transpose of \b aRb.
     *
     * The transpose of a rotation matrix is the same as the inverse.
     */
    template< class T > const Rotation2D< T > transpose (const Rotation2D< T >& aRb)
    {
        return Rotation2D< T > (aRb (0, 0), aRb (1, 0), aRb (0, 1), aRb (1, 1));
    }
#if !defined(SWIG)
    extern template class rw::math::Rotation2D< double >;
    extern template class rw::math::Rotation2D< float >;
#else

#if SWIG_VERSION < 0x040000
    SWIG_DECLARE_TEMPLATE (Rotation2Dd, rw::math::Rotation2D< double >);
    ADD_DEFINITION (Rotation2Dd, Rotation2D, sdurw_math);
#else
    SWIG_DECLARE_TEMPLATE (Rotation2D, rw::math::Rotation2D< double >);
#endif

    SWIG_DECLARE_TEMPLATE (Rotation2Df, rw::math::Rotation2D< float >);
#endif
    using Rotation2Dd = Rotation2D< double >;
    using Rotation2Df = Rotation2D< float >;

    /**@}*/
}}    // namespace rw::math

namespace rw { namespace common {
    class OutputArchive;
    class InputArchive;
    namespace serialization {
        /**
         * @copydoc rw::common::serialization::write
         * @relatedalso rw::math::Rotation2D
         */
        template<>
        void write (const rw::math::Rotation2D< double >& sobject,
                    rw::common::OutputArchive& oarchive, const std::string& id);

        /**
         * @copydoc rw::common::serialization::write
         * @relatedalso rw::math::Rotation2D
         */
        template<>
        void write (const rw::math::Rotation2D< float >& sobject,
                    rw::common::OutputArchive& oarchive, const std::string& id);

        /**
         * @copydoc rw::common::serialization::read
         * @relatedalso rw::math::Rotation2D
         */
        template<>
        void read (rw::math::Rotation2D< double >& sobject, rw::common::InputArchive& iarchive,
                   const std::string& id);

        /**
         * @copydoc rw::common::serialization::read
         * @relatedalso rw::math::Rotation2D
         */
        template<>
        void read (rw::math::Rotation2D< float >& sobject, rw::common::InputArchive& iarchive,
                   const std::string& id);

    }    // namespace serialization
}}       // namespace rw::common

#endif    // end include guard
