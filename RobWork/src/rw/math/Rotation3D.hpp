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

#ifndef RW_MATH_ROTATION3D_HPP
#define RW_MATH_ROTATION3D_HPP

/**
 * @file Rotation3D.hpp
 */
#if !defined(SWIG)
#include <rw/common/Serializable.hpp>
#include <rw/core/macros.hpp>
#include <rw/math/Vector3D.hpp>

#include <Eigen/Core>
#include <limits>
#endif

namespace rw { namespace math {

    template< class T > class Rotation3DVector;
    /** @addtogroup math */
    /* @{*/

#if !defined(SWIGJAVA)
    /**
     * @brief A 3x3 rotation matrix \f$ \mathbf{R}\in SO(3) \f$
     *
     * @f$
     *  \mathbf{R}=
     *  \left[
     *  \begin{array}{ccc}
     *  {}^A\hat{X}_B & {}^A\hat{Y}_B & {}^A\hat{Z}_B
     *  \end{array}
     *  \right]
     *  =
     *  \left[
     *  \begin{array}{ccc}
     *  r_{11} & r_{12} & r_{13} \\
     *  r_{21} & r_{22} & r_{23} \\
     *  r_{31} & r_{32} & r_{33}
     *  \end{array}
     *  \right]
     * @f$
     */

#endif
    template< class T = double > class Rotation3D
    {
      public:
        //! Value type.
        typedef T value_type;

        //! @brief The type of the internal Eigen matrix implementation.
        typedef Eigen::Matrix< T, 3, 3 > EigenMatrix3x3;

        /**
           @brief A rotation matrix with uninitialized storage.
         */
        Rotation3D ()
        {
            _m (0, 0) = 1;
            _m (0, 1) = 0;
            _m (0, 2) = 0;
            _m (1, 0) = 0;
            _m (1, 1) = 1;
            _m (1, 2) = 0;
            _m (2, 0) = 0;
            _m (2, 1) = 0;
            _m (2, 2) = 1;
        }

#if !defined(SWIGJAVA)
        /**
         * @brief Constructs an initialized 3x3 rotation matrix
         *
         * @param r11 \f$ r_{11} \f$
         * @param r12 \f$ r_{12} \f$
         * @param r13 \f$ r_{13} \f$
         * @param r21 \f$ r_{21} \f$
         * @param r22 \f$ r_{22} \f$
         * @param r23 \f$ r_{23} \f$
         * @param r31 \f$ r_{31} \f$
         * @param r32 \f$ r_{32} \f$
         * @param r33 \f$ r_{33} \f$
         *
         * @f$
         *  \mathbf{R} =
         *  \left[
         *  \begin{array}{ccc}
         *  r_{11} & r_{12} & r_{13} \\
         *  r_{21} & r_{22} & r_{23} \\
         *  r_{31} & r_{32} & r_{33}
         *  \end{array}
         *  \right]
         * @f$
         */

#endif
        Rotation3D (T r11, T r12, T r13, T r21, T r22, T r23, T r31, T r32, T r33)
        {
            _m (0, 0) = r11;
            _m (0, 1) = r12;
            _m (0, 2) = r13;
            _m (1, 0) = r21;
            _m (1, 1) = r22;
            _m (1, 2) = r23;
            _m (2, 0) = r31;
            _m (2, 1) = r32;
            _m (2, 2) = r33;
        }

#if !defined(SWIGJAVA)
        /**
         * @brief Constructs an initialized 3x3 rotation matrix
         * @f$ \robabx{a}{b}{\mathbf{R}} =
         * \left[
         *  \begin{array}{ccc}
         *   \robabx{a}{b}{\mathbf{i}} & \robabx{a}{b}{\mathbf{j}} & \robabx{a}{b}{\mathbf{k}}
         *  \end{array}
         * \right]
         * @f$
         *
         * @param i @f$ \robabx{a}{b}{\mathbf{i}} @f$
         * @param j @f$ \robabx{a}{b}{\mathbf{j}} @f$
         * @param k @f$ \robabx{a}{b}{\mathbf{k}} @f$
         */

#endif
        Rotation3D (const rw::math::Vector3D< T >& i, const rw::math::Vector3D< T >& j,
                    const rw::math::Vector3D< T >& k)
        {
            _m (0, 0) = i[0];
            _m (0, 1) = j[0];
            _m (0, 2) = k[0];
            _m (1, 0) = i[1];
            _m (1, 1) = j[1];
            _m (1, 2) = k[1];
            _m (2, 0) = i[2];
            _m (2, 1) = j[2];
            _m (2, 2) = k[2];
        }

#if !defined(SWIGJAVA)
        /**
         * @brief Initialize Rotation3D from other rotation types
         * @param rotVec [in] rotation type such as \b EAA, \b RPY, or \b Quaternion
         */
        explicit Rotation3D (const Rotation3DVector< T >& rotVec);

        /**
         * @brief Constructs a 3x3 rotation matrix set to identity
         * @return a 3x3 identity rotation matrix
         *
         * @f$
         * \mathbf{R} =
         * \left[
         * \begin{array}{ccc}
         * 1 & 0 & 0 \\
         * 0 & 1 & 0 \\
         * 0 & 0 & 1
         * \end{array}
         * \right]
         * @f$
         */

#endif
        static const Rotation3D& identity ();

        /**
         * @brief Normalizes the rotation matrix to satisfy SO(3).
         *
         * Makes a normalization of the rotation matrix such that the columns
         * are normalized and othogonal s.t. it belongs to SO(3).
         */
        void normalize ();
#if !defined(SWIG)
        /**
         * @brief Returns reference to matrix element
         * @param row [in] row
         * @param column [in] column
         * @return reference to the element
         */
        inline T& operator() (size_t row, size_t column) { return _m (row, column); }

        /**
         * @brief Returns reference to matrix element
         * @param row [in] row
         * @param column [in] column
         * @return reference to the element
         */
        inline const T& operator() (size_t row, size_t column) const { return _m (row, column); }
#else
        MATRIXOPERATOR (T);
#endif
        /**
         * @brief Returns the i'th row of the rotation matrix
         * @param i [in] Index of the row to return. Only valid indices are 0, 1 and 2.
         */
        const rw::math::Vector3D< T > getRow (size_t i) const
        {
            RW_ASSERT (i < 3);
            return rw::math::Vector3D< T > (_m (i, 0), _m (i, 1), _m (i, 2));
        }

        /**
         * @brief Returns the i'th column of the rotation matrix
         * @param i [in] Index of the column to return. Only valid indices are 0, 1 and 2.
         */
        const rw::math::Vector3D< T > getCol (size_t i) const
        {
            RW_ASSERT (i < 3);
            return rw::math::Vector3D< T > (_m (0, i), _m (1, i), _m (2, i));
        }

        /**
         * @brief Comparison operator.
         *
         * The comparison operator makes a element wise comparison.
         * Returns true only if all elements are equal.
         *
         * @param rhs [in] Rotation to compare with
         * @return True if equal.
         */
        bool operator== (const Rotation3D< T >& rhs) const
        {
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if (!(_m (i, j) == rhs (i, j)))
                        return false;
            return true;
        }

        /**
         * @brief Comparison operator.
         *
         * The comparison operator makes a element wise comparison.
         * Returns true if any of the elements are different.
         *
         * @param rhs [in] Rotation to compare with
         * @return True if not equal.
         */
        bool operator!= (const Rotation3D< T >& rhs) const { return !(*this == rhs); }

        /**
         * @brief Compares rotations with a given precision
         *
         * Performs an element wise comparison. Two elements are considered equal if the difference
         * are less than \b precision.
         *
         * @param rot [in] Rotation to compare with
         * @param precision [in] The precision to use for testing
         * @return True if all elements are less than \b precision apart.
         */
        bool equal (const Rotation3D< T >& rot,
                    const T precision = std::numeric_limits< T >::epsilon ()) const
        {
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if (fabs (_m (i, j) - rot (i, j)) > precision)
                        return false;
            return true;
        }

        /**
         * @brief Verify that this rotation is a proper rotation
         *
         * @return True if this rotation is considered a proper rotation
         */
        bool isProperRotation () const;

        /**
         * @brief Verify that this rotation is a proper rotation
         *
         * @return True if this rotation is considered a proper rotation
         */
        bool isProperRotation (T precision) const;

        /**
         * @brief Returns a Eigen 3x3 matrix @f$ \mathbf{M}\in SO(3)
         * @f$ that represents this rotation
         *
         * @return @f$ \mathbf{M}\in SO(3) @f$
         */
        const EigenMatrix3x3& e () const { return _m; };

        /**
         * @brief Returns a Eigen 3x3 matrix @f$ \mathbf{M}\in SO(3)
         * @f$ that represents this rotation
         *
         * @return @f$ \mathbf{M}\in SO(3) @f$
         */
        EigenMatrix3x3& e () { return _m; };

        /**
         * @brief Calculates \f$ \robabx{a}{c}{\mathbf{R}} =
         * \robabx{a}{b}{\mathbf{R}} \robabx{b}{c}{\mathbf{R}} \f$
         *
         * @param bRc [in] \f$ \robabx{b}{c}{\mathbf{R}} \f$
         *
         * @return \f$ \robabx{a}{c}{\mathbf{R}} \f$
         */
        inline const Rotation3D operator* (const Rotation3D& bRc) const
        {
            return multiply (*this, bRc);
        }

        /**
         * @brief Calculates \f$ \robabx{a}{c}{\mathbf{R}} =
         * \robabx{a}{b}{\mathbf{R}} \robabx{b}{c}{\mathbf{R}} \f$
         *
         * @param rhs [in] \f$ \robabx{b}{c}{\mathbf{R}} \f$
         *
         * @return \f$ \robabx{a}{c}{\mathbf{R}} \f$
         */
        template< class R >
        inline const Rotation3D operator* (const Eigen::MatrixBase< R >& rhs) const
        {
            return Rotation3D< T > (this->e () * rhs);
        }

        /**
         * @brief Calculates \f$ \robabx{a}{c}{\mathbf{R}} =
         * \robabx{a}{b}{\mathbf{R}} \robabx{b}{c}{\mathbf{R}} \f$
         *
         * @param lhs [in] \f$ \robabx{b}{c}{\mathbf{R}} \f$
         * @param rhs [in] \f$ \robabx{b}{c}{\mathbf{R}} \f$
         * @return \f$ \robabx{a}{c}{\mathbf{R}} \f$
         */
        template< class R >
        friend inline Rotation3D operator* (const Eigen::MatrixBase< R >& lhs,
                                            const Rotation3D< T >& rhs)
        {
            return Rotation3D< T > (lhs * rhs.e ());
        }

        /**
         * @brief Calculates \f$ \robabx{a}{c}{\mathbf{v}} =
         * \robabx{a}{b}{\mathbf{R}} \robabx{b}{c}{\mathbf{v}} \f$
         *
         * @param bVc [in] \f$ \robabx{b}{c}{\mathbf{v}} \f$
         * @return \f$ \robabx{a}{c}{\mathbf{v}} \f$
         */
        inline const rw::math::Vector3D< T > operator* (const rw::math::Vector3D< T >& bVc) const
        {
            return multiply (*this, bVc);
        }

        /**
         * @brief Construct a rotation matrix from a 3x3 Eigen matrix
         * It is the responsibility of the user that 3x3 matrix is indeed a
           rotation matrix.
         */
        template< class R > explicit Rotation3D (const EigenMatrix3x3& r)
        {
            _m (0, 0) = r (0, 0);
            _m (0, 1) = r (0, 1);
            _m (0, 2) = r (0, 2);
            _m (1, 0) = r (1, 0);
            _m (1, 1) = r (1, 1);
            _m (1, 2) = r (1, 2);
            _m (2, 0) = r (2, 0);
            _m (2, 1) = r (2, 1);
            _m (2, 2) = r (2, 2);
        }

        /**
         * @brief Construct a rotation matrix from a 3x3 Eigen matrix
         * It is the responsibility of the user that 3x3 matrix is indeed a
           rotation matrix.
        */
        template< class R > explicit Rotation3D (const Eigen::MatrixBase< R >& m)
        {
            RW_ASSERT (m.cols () == 3);
            RW_ASSERT (m.rows () == 3);
            _m (0, 0) = T (m.row (0) (0));
            _m (0, 1) = T (m.row (0) (1));
            _m (0, 2) = T (m.row (0) (2));
            _m (1, 0) = T (m.row (1) (0));
            _m (1, 1) = T (m.row (1) (1));
            _m (1, 2) = T (m.row (1) (2));
            _m (2, 0) = T (m.row (2) (0));
            _m (2, 1) = T (m.row (2) (1));
            _m (2, 2) = T (m.row (2) (2));
        }

        /**
         * @brief Creates a skew symmetric matrix from a Vector3D. Also
         * known as the cross product matrix of v.
         *
         * @relates Rotation3D
         *
         * @param v [in] vector to create Skew matrix from
         */
        static Rotation3D< T > skew (const rw::math::Vector3D< T >& v)
        {
            return Rotation3D< T > (0, -v (2), v (1), v (2), 0, -v (0), -v (1), v (0), 0);
        }

        // Faster-than-boost matrix multiplications below.

        /**
         *  @brief Write to \b result the product \b a * \b b.
         */
        static void multiply (const Rotation3D< T >& a, const Rotation3D< T >& b,
                              Rotation3D< T >& result);

        /**
         *  @brief Write to \b result the product \b a * \b b.
         */
        static void multiply (const Rotation3D< T >& a, const rw::math::Vector3D< T >& b,
                              rw::math::Vector3D< T >& result);

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
        static const Rotation3D< T > multiply (const Rotation3D< T >& aRb,
                                               const Rotation3D< T >& bRc);

        /**
         * @brief Calculates \f$ \robabx{a}{c}{\mathbf{v}} =
         * \robabx{a}{b}{\mathbf{R}} \robabx{b}{c}{\mathbf{v}} \f$
         *
         * @param aRb [in] \f$ \robabx{a}{b}{\mathbf{R}} \f$
         * @param bVc [in] \f$ \robabx{b}{c}{\mathbf{v}} \f$
         * @return \f$ \robabx{a}{c}{\mathbf{v}} \f$
         */
        static const rw::math::Vector3D< T > multiply (const Rotation3D< T >& aRb,
                                                       const rw::math::Vector3D< T >& bVc);
#if !defined(SWIGJAVA)
        /**
         * @brief Calculate the inverse.
         * @note This function changes the object that it is invoked on, but this is about x5 faster
         * than rot = inverse( rot )
         * @see rw::math::inverse(const rw::math::Rotation3D< T > &) for the (slower) version that
         * does not change the rotation object itself.
         * @return the inverse rotation.
         */
#endif
        inline Rotation3D< T >& inverse ()
        {
            T tmpVal  = _m (0, 1);
            _m (0, 1) = _m (1, 0);
            _m (1, 0) = tmpVal;

            tmpVal    = _m (0, 2);
            _m (0, 2) = _m (2, 0);
            _m (2, 0) = tmpVal;

            tmpVal    = _m (1, 2);
            _m (1, 2) = _m (2, 1);
            _m (2, 1) = tmpVal;
            return *this;
        }

        /**
         * @brief Calculate the inverse.
         * @param copy [in] if coopy is false, This function changes the object that it is invoked
         * on, but this is about x5 faster than rot = inverse( rot ). else it changes the object,
         * making it a bit slower.
         * @return the inverse rotation.
         */
        Rotation3D< T > inverse (bool copy);

        /**
         * @brief Calculate the inverse. of a const Rotation3D. For this function copy is always
         * true
         * @param copy [in] always true
         * @return the inverse rotation.
         */
        Rotation3D< T > inverse (bool copy) const;

        T tr () const { return (*this) (0, 0) + (*this) (1, 1) + (*this) (2, 2); }

#if defined(SWIG)
        TOSTRING (rw::math::Rotation3D< T >);
#endif

      private:
        EigenMatrix3x3 _m;
    };

    /**
     * @brief Casts Rotation3D<T> to Rotation3D<Q>
     *
     * @relates Rotation3D
     *
     * @param rot [in] Rotation3D with type T
     * @return Rotation3D with type Q
     */
    template< class Q, class T > const Rotation3D< Q > cast (const Rotation3D< T >& rot)
    {
        Rotation3D< Q > res;
        for (size_t i = 0; i < 3; i++)
            for (size_t j = 0; j < 3; j++)
                res (i, j) = static_cast< Q > (rot (i, j));
        return res;
    }

#if !defined(SWIGJAVA)
    /**
     * @brief Calculates the inverse @f$ \robabx{b}{a}{\mathbf{R}} =
     * \robabx{a}{b}{\mathbf{R}}^{-1} @f$ of a rotation matrix
     *
     * @relates Rotation3D
     *
     * @see Rotation3D::inverse() for a faster version that modifies the existing rotation object
     * instead of allocating a new one.
     *
     * @param aRb [in] the rotation matrix @f$ \robabx{a}{b}{\mathbf{R}} @f$
     *
     * @return the matrix inverse @f$ \robabx{b}{a}{\mathbf{R}} =
     * \robabx{a}{b}{\mathbf{R}}^{-1} @f$
     *
     * @f$ \robabx{b}{a}{\mathbf{R}} = \robabx{a}{b}{\mathbf{R}}^{-1} =
     * \robabx{a}{b}{\mathbf{R}}^T @f$
     */

#endif
    template< class T >
    const rw::math::Rotation3D< T > inverse (const rw::math::Rotation3D< T >& aRb)
    {
        return Rotation3D< T > (aRb (0, 0),
                                aRb (1, 0),
                                aRb (2, 0),

                                aRb (0, 1),
                                aRb (1, 1),
                                aRb (2, 1),

                                aRb (0, 2),
                                aRb (1, 2),
                                aRb (2, 2));
    }

    /**
     * @brief Writes rotation matrix to stream
     *
     * @relates Rotation3D
     *
     * @param os [in/out] output stream to use
     * @param r [in] rotation matrix to print
     * @return the updated output stream
     */
    template< class T > std::ostream& operator<< (std::ostream& os, const Rotation3D< T >& r)
    {
        return os << "Rotation3D(" << r (0, 0) << ", " << r (0, 1) << ", " << r (0, 2) << ", "
                  << r (1, 0) << ", " << r (1, 1) << ", " << r (1, 2) << ", " << r (2, 0) << ", "
                  << r (2, 1) << ", " << r (2, 2) << ")";
    }
#if !defined(SWIG)
    // Explicit template specifications.
    extern template class rw::math::Rotation3D< double >;
    extern template class rw::math::Rotation3D< float >;
#else

#if SWIG_VERSION < 0x040000
    SWIG_DECLARE_TEMPLATE (Rotation3Dd, rw::math::Rotation3D< double >);
    ADD_DEFINITION (Rotation3Dd, Rotation3D, sdurw_math)
#else
    SWIG_DECLARE_TEMPLATE (Rotation3D, rw::math::Rotation3D< double >);
#endif

    SWIG_DECLARE_TEMPLATE (Rotation3Df, rw::math::Rotation3D< float >);
#endif
    using Rotation3Dd = Rotation3D< double >;
    using Rotation3Df = Rotation3D< float >;

    /**@}*/
}}    // namespace rw::math

namespace rw { namespace common {
    class OutputArchive;
    class InputArchive;
    namespace serialization {
        /**
         * @copydoc rw::common::serialization::write
         * @relatedalso rw::math::Rotation3D
         */
        template<>
        void write (const rw::math::Rotation3D< double >& sobject,
                    rw::common::OutputArchive& oarchive, const std::string& id);

        /**
         * @copydoc rw::common::serialization::write
         * @relatedalso rw::math::Rotation3D
         */
        template<>
        void write (const rw::math::Rotation3D< float >& sobject,
                    rw::common::OutputArchive& oarchive, const std::string& id);

        /**
         * @copydoc rw::common::serialization::read
         * @relatedalso rw::math::Rotation3D
         */
        template<>
        void read (rw::math::Rotation3D< double >& sobject, rw::common::InputArchive& iarchive,
                   const std::string& id);

        /**
         * @copydoc rw::common::serialization::read
         * @relatedalso rw::math::Rotation3D
         */
        template<>
        void read (rw::math::Rotation3D< float >& sobject, rw::common::InputArchive& iarchive,
                   const std::string& id);
    }    // namespace serialization
}}       // namespace rw::common

namespace boost { namespace serialization {
    /**
     * @brief Boost serialization.
     * @param archive [in] the boost archive to read from or write to.
     * @param R [in/out] the rotation matrix to read/write.
     * @param version [in] class version (currently version 0).
     * @relatedalso rw::math::Rotation3D
     */
    template< class Archive, class T >
    void serialize (Archive& archive, rw::math::Rotation3D< T >& R, const unsigned int version)
    {
        archive& R (0, 0);
        archive& R (0, 1);
        archive& R (0, 2);
        archive& R (1, 0);
        archive& R (1, 1);
        archive& R (1, 2);
        archive& R (2, 0);
        archive& R (2, 1);
        archive& R (2, 2);
    }
}}    // namespace boost::serialization

#endif    // end include guard
