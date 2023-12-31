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

#ifndef RW_MATH_TRANSFORM3D_HPP
#define RW_MATH_TRANSFORM3D_HPP

/**
 * @file Transform3D.hpp
 */

#if !defined(SWIG)
#include <rw/math/Rotation3D.hpp>
#include <rw/math/Rotation3DVector.hpp>
#include <rw/math/Vector3D.hpp>

#include <Eigen/Core>
#include <cassert>
#include <limits>
#endif

namespace rw { namespace math {

    /** @addtogroup math */
    /*@{*/
#if !defined(SWIGJAVA)
    /**
     * @brief A 4x4 homogeneous transform matrix @f$ \mathbf{T}\in SE(3) @f$
     *
     * @f$
     * \mathbf{T} =
     * \left[
     *  \begin{array}{cc}
     *  \mathbf{R} & \mathbf{d} \\
     *  \begin{array}{ccc}0 & 0 & 0\end{array} & 1
     *  \end{array}
     * \right]
     * @f$
     *
     */

     #endif 
    template< class T = double > class Transform3D
    {
      public:
        //! Value type.
        typedef T value_type;

        //! @brief Type for the internal Eigen matrix.
        typedef Eigen::Matrix< T, 4, 4 > EigenMatrix4x4;

        /**
         * @brief Default Constructor.
         *
         * Initializes with 0 translation and Identity matrix as rotation
         */
        Transform3D () : _d (), _R (rw::math::Rotation3D< T >::identity ()) {}

        /**
         * @brief Constructs a homogeneous transform
         * @param d [in] @f$ \mathbf{d} @f$ A 3x1 translation vector
         * @param R [in] @f$ \mathbf{R} @f$ A 3x3 rotation matrix
         */
        Transform3D (const rw::math::Vector3D< T >& d, const rw::math::Rotation3D< T >& R) : _d (d), _R (R) {}

        /**
           @brief A homogeneous transform with a rotation of \b R and a
           translation of zero.
        */
        explicit Transform3D (const rw::math::Rotation3D< T >& R) : _d (0, 0, 0), _R (R) {}

        /**
           @brief A homogeneous transform with a rotation of zero and a
           translation of \b d.
        */
        explicit Transform3D (const rw::math::Vector3D< T >& d) : _d (d), _R (rw::math::Rotation3D< T >::identity ()) {}

        /**
         * @brief Copy Constructor
         * @param Transform3D [in] Values to initialize the transform
         */
        Transform3D (const rw::math::Transform3D< T >& t) : _d (t._d), _R (t._R) {}

        /**
         * @brief Constructs a homogeneous transform
         *
         * Calling this constructor is equivalent to the transform
         * Transform3D(d, r.toRotation3D()).
         *
         * @param d [in] @f$ \mathbf{d} @f$ A 3x1 translation vector
         * @param r [in] @f$ \mathbf{r} @f$ A 3x1 rotation vector
         */
        Transform3D (const rw::math::Vector3D< T >& d, const rw::math::Rotation3DVector< T >& r) :
            _d (d), _R (r.toRotation3D ())
        {}
        

        /**
         * @brief Creates a Transform3D from matrix_expression
         * @param r [in] an Eigen Vector
         */
        template< class R > explicit Transform3D (const Eigen::MatrixBase< R >& r)
        {
            _d[0] = T(r.row (0) (3));
            _d[1] = T(r.row (1) (3));
            _d[2] = T(r.row (2) (3));
            _R = Rotation3D<T>(r.block(0,0,3,3));
        }
#if !defined(SWIGJAVA)
        /**
         * @brief Constructs a homogeneous transform using the original
         * Denavit-Hartenberg notation
         *
         * @param alpha [in] @f$ \alpha_i @f$
         * @param a [in] @f$ a_i @f$
         * @param d [in] @f$ d_i @f$
         * @param theta [in] @f$ \theta_i @f$
         * @return @f$ ^{i-1}\mathbf{T}_i @f$
         *
         * @f$
         *  \robabx{i-1}{i}{\mathbf{T}}=
         *  \left[
         *    \begin{array}{cccc}
         *      c\theta_i & -s\theta_i c\alpha_i &  s\theta_i s\alpha_i & a_i c\theta_i \\
         *      s\theta_i &  c\theta_i c\alpha_i & -c\theta_i s\alpha_i & a_i s\theta_i \\
         *      0         &  s\alpha_i           &  c\alpha_i           & d_i \\
         *      0         &  0                   & 0                    & 1
         *    \end{array}
         *  \right]
         * @f$
         */

         #endif
        static const Transform3D DH (T alpha, T a, T d, T theta);

#if !defined(SWIGJAVA)
        /**
         * @brief Constructs a homogeneous transform using the Craig (modified)
         * Denavit-Hartenberg notation
         *
         * @param alpha [in] @f$ \alpha_{i-1} @f$
         * @param a [in] \f$ a_{i-1} \f$
         * @param d [in] \f$ d_i \f$
         * @param theta [in] \f$ \theta_i \f$
         * @return @f$ \robabx{i-1}{i}{\mathbf{T}} @f$
         *
         * @note The Craig (modified) Denavit-Hartenberg notation differs from
         * the original Denavit-Hartenberg notation and is given as
         *
         * @f$
         * \robabx{i-1}{i}{\mathbf{T}} =
         * \left[
         * \begin{array}{cccc}
         * c\theta_i & -s\theta_i & 0 & a_{i-1} \\
         * s\theta_i c\alpha_{i-1} & c\theta_i c\alpha_{i-1} & -s\alpha_{i-1} & -s\alpha_{i-1}d_i \\
         * s\theta_i s\alpha_{i-1} & c\theta_i s\alpha_{i-1} &  c\alpha_{i-1} &  c\alpha_{i-1}d_i \\
         * 0 & 0 & 0 & 1
         * \end{array}
         * \right]
         * @f$
         *
         */

         #endif
        static const Transform3D craigDH (T alpha, T a, T d, T theta);

#if !defined(SWIGJAVA)
        /**
         * @brief Constructs a homogeneous transform using the Gordon (modified)
         * Denavit-Hartenberg notation
         *
         * @param alpha [in] @f$ \alpha_i @f$
         * @param a [in] @f$ a_i @f$
         * @param beta [in] @f$ \beta_i @f$
         * @param b [in] @f$ b_i @f$
         * @return @f$ ^{i-1}\mathbf{T}_i @f$
         *
         * @note The Gordon (modified) Denavit-Hartenberg differs from
         * the original Denavit-Hartenberg as it branches between parallel
         * and non-parallel z-axes.
         *
         * @f$ z_{i-1} @f$ is close to parallel to @f$ z_i @f$
         * @f$
         *  \robabx{i-1}{i}{\mathbf{T}}=
         *  \left[
         *    \begin{array}{cccc}
         *       c\beta_i & s\alpha_i s\beta_i &  c\alpha_i s\beta_i &  a_i c\beta_i \\
         *       0        & c\alpha_i          & -s\alpha_i          &  b_i \\
         *      -s\beta_i & s\alpha_i c\beta_i &  c\alpha_i c\beta_i & -a_i s\beta \\
         *      0         & 0                  & 0                    & 1
         *    \end{array}
         *  \right]
         * @f$
         */

         #endif 
        static const Transform3D DHHGP (T alpha, T a, T beta, T b);

#if !defined(SWIGJAVA)
        /**
         * @brief Constructs the identity transform
         * @return the identity transform
         *
         * @f$
         * \mathbf{T} =
         * \left[
         * \begin{array}{cccc}
         * 1 & 0 & 0 & 0\\
         * 0 & 1 & 0 & 0\\
         * 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 1
         * \end{array}
         * \right]
         * @f$
         */

         #endif 
        static const Transform3D& identity ();

#if !defined(SWIG)
        /**
         * @brief Returns matrix element reference
         * @param row [in] row, row must be @f$ < 3 @f$
         * @param col [in] col, col must be @f$ < 4 @f$
         * @return reference to matrix element
         */
        T& operator() (std::size_t row, std::size_t col)
        {
            assert (row < 3);
            assert (col < 4);
            if (row < 3 && col < 3)
                return _R (row, col);
            else
                return _d (row);
        }

        /**
         * @brief Returns const matrix element reference
         * @param row [in] row, row must be @f$ < 3 @f$
         * @param col [in] col, col must be @f$ < 4 @f$
         * @return const reference to matrix element
         */
        const T& operator() (std::size_t row, std::size_t col) const
        {
            assert (row < 3);
            assert (col < 4);
            if (row < 3 && col < 3)
                return _R (row, col);
            else
                return _d (row);
        }
#else
        MATRIXOPERATOR (T);
#endif

        /**
         * @brief Comparison operator.
         *
         * The comparison operator makes a element wise comparison.
         * Returns true only if all elements are equal.
         *
         * @param rhs [in] Transform to compare with
         * @return True if equal.
         */
        bool operator== (const Transform3D< T >& rhs) const
        {
            return (R () == rhs.R ()) && (P () == rhs.P ());
        }

        /**
         * @brief Comparison operator.
         *
         * The comparison operator makes a element wise comparison.
         * Returns true if any of the elements are different.
         *
         * @param rhs [in] Transform to compare with
         * @return True if not equal.
         */
        bool operator!= (const Transform3D< T >& rhs) const { return !(*this == rhs); }

        /**
         * @brief Compares the transformations with a given precision
         *
         * Performs an element wise comparison. Two elements are considered equal if the difference
         * are less than \b precision.
         *
         * @param t3d [in] Transform to compare with
         * @param precision [in] The precision to use for testing
         * @return True if all elements are less than \b precision apart.
         */
        bool equal (const Transform3D< T >& t3d,
                    const T precision = std::numeric_limits< T >::epsilon ()) const
        {
            if (!R ().equal (t3d.R (), precision))
                return false;
            for (size_t i = 0; i < 3; i++)
                if (fabs (P ()[i] - t3d.P ()[i]) > precision)
                    return false;
            return true;
        }

#if !defined(SWIGJAVA)
        /**
         * @brief Calculates @f$ \robabx{a}{c}{\mathbf{T}} = \robabx{a}{b}{\mathbf{T}}
         * \robabx{b}{c}{\mathbf{T}} @f$
         * @param bTc [in] @f$ \robabx{b}{c}{\mathbf{T}} @f$
         * @return @f$ \robabx{a}{c}{\mathbf{T}} @f$
         *
         * @f$
         * \robabx{a}{c}{\mathbf{T}} =
         * \left[
         *  \begin{array}{cc}
         *  \robabx{a}{b}{\mathbf{R}}\robabx{b}{c}{\mathbf{R}} & \robabx{a}{b}{\mathbf{d}} +
         * \robabx{a}{b}{\mathbf{R}}\robabx{b}{c}{\mathbf{d}} \\ \begin{array}{ccc}0 & 0 &
         * 0\end{array} & 1 \end{array} \right]
         * @f$
         */

         #endif 
        const Transform3D operator* (const Transform3D& bTc) const
        {
            return Transform3D (_d + _R * bTc._d, _R * bTc._R);
        }

        /**
         * @brief Calculates @f$ \robax{a}{\mathbf{p}} = \robabx{a}{b}{\mathbf{T}}
         * \robax{b}{\mathbf{p}} \f$ thus transforming point @f$ \mathbf{p} @f$ from frame @f$ b @f$
         * to frame @f$ a @f$
         * @param bP [in] @f$ \robax{b}{\mathbf{p}} @f$
         * @return @f$ \robax{a}{\mathbf{p}} @f$
         */
        const rw::math::Vector3D< T > operator* (const rw::math::Vector3D< T >& bP) const { return _R * bP + _d; }

        /**
         * @brief Gets the rotation part @f$ \mathbf{R} @f$ from @f$ \mathbf{T} @f$
         * @return @f$ \mathbf{R} @f$
         */
        rw::math::Rotation3D< T >& R () { return _R; }

        /**
         * @brief Gets the rotation part @f$ \mathbf{R} @f$ from @f$ \mathbf{T} @f$
         * @return @f$ \mathbf{R} @f$
         */
        const rw::math::Rotation3D< T >& R () const { return _R; }

        /**
         * \brief Gets the position part @f$ \mathbf{d} @f$ from @f$ \mathbf{T} @f$
         * \return @f$ \mathbf{d} @f$
         */
        rw::math::Vector3D< T >& P () { return _d; }

        /**
         * @brief Gets the position part @f$ \mathbf{d} @f$ from @f$ \mathbf{T} @f$
         * @return @f$ \mathbf{d} @f$
         */
        const rw::math::Vector3D< T >& P () const { return _d; }

#if !defined(SWIG)
        /**
         * @brief Outputs transform to stream
         * @param os [in/out] an output stream
         * @param t [in] the transform that is to be sent to the output stream
         * @return os
         */
        friend std::ostream& operator<< (std::ostream& os, const Transform3D< T >& t)
        {
            // This format matches the Lua notation.
            return os << "Transform3D(" << t.P () << ", " << t.R () << ")";
        }
#else
        TOSTRING (rw::math::Transform3D< T >);
#endif

        /**
           @brief Write to \b result the product \b a * \b b.
        */
        static inline void multiply (const Transform3D< T >& a, const Transform3D< T >& b,
                                     Transform3D< T >& result)
        {
            rw::math::Rotation3D< T >::multiply (a.R (), b.R (), result.R ());
            rw::math::Rotation3D< T >::multiply (a.R (), b.P (), result.P ());
            result.P () += a.P ();
        }

        /**
         * @brief computes the inverse of t1 and multiplies it with t2.
         * The result is saved in t1. t1 = inv(t1) * t2
         */
        static inline Transform3D< T >& invMult (Transform3D< T >& t1, const Transform3D< T >& t2)
        {
            const T p0 = t1.P () (0), p1 = t1.P () (1), p2 = t1.P () (2);

            const T r01 = t1.R () (0, 1);
            const T r12 = t1.R () (1, 2);
            const T r02 = t1.R () (0, 2);

            t1.P () (0) = (-p0 + t2.P () (0)) * t1.R () (0, 0) +
                          (-p1 + t2.P () (1)) * t1.R () (1, 0) +
                          (-p2 + t2.P () (2)) * t1.R () (2, 0);

            t1.P () (1) = (-p0 + t2.P () (0)) * r01 + (-p1 + t2.P () (1)) * t1.R () (1, 1) +
                          (-p2 + t2.P () (2)) * t1.R () (2, 1);

            t1.P () (2) = (-p0 + t2.P () (0)) * r02 + (-p1 + t2.P () (1)) * r12 +
                          (-p2 + t2.P () (2)) * t1.R () (2, 2);

            t1.R () (0, 1) = t1.R () (0, 0) * t2.R () (0, 1) + t1.R () (1, 0) * t2.R () (1, 1) +
                             t1.R () (2, 0) * t2.R () (2, 1);
            t1.R () (0, 2) = t1.R () (0, 0) * t2.R () (0, 2) + t1.R () (1, 0) * t2.R () (1, 2) +
                             t1.R () (2, 0) * t2.R () (2, 2);
            t1.R () (0, 0) = t1.R () (0, 0) * t2.R () (0, 0) + t1.R () (1, 0) * t2.R () (1, 0) +
                             t1.R () (2, 0) * t2.R () (2, 0);

            t1.R () (1, 0) = r01 * t2.R () (0, 0) + t1.R () (1, 1) * t2.R () (1, 0) +
                             t1.R () (2, 1) * t2.R () (2, 0);
            t1.R () (1, 2) = r01 * t2.R () (0, 2) + t1.R () (1, 1) * t2.R () (1, 2) +
                             t1.R () (2, 1) * t2.R () (2, 2);
            t1.R () (1, 1) = r01 * t2.R () (0, 1) + t1.R () (1, 1) * t2.R () (1, 1) +
                             t1.R () (2, 1) * t2.R () (2, 1);

            t1.R () (2, 0) =
                r02 * t2.R () (0, 0) + r12 * t2.R () (1, 0) + t1.R () (2, 2) * t2.R () (2, 0);
            t1.R () (2, 1) =
                r02 * t2.R () (0, 1) + r12 * t2.R () (1, 1) + t1.R () (2, 2) * t2.R () (2, 1);
            t1.R () (2, 2) =
                r02 * t2.R () (0, 2) + r12 * t2.R () (1, 2) + t1.R () (2, 2) * t2.R () (2, 2);
            return t1;
        }

        /**
         * @brief computes the inverse of t1 and multiplies it with t2.
         * The result is saved in t1. t1 = inv(t1) * t2
         */
        static inline Transform3D< T >& invMult (const Transform3D< T >& t1,
                                                 const Transform3D< T >& t2, Transform3D< T >& t3)
        {
            const T p0 = t1.P () (0), p1 = t1.P () (1), p2 = t1.P () (2);

            const T r01 = t1.R () (0, 1);
            const T r12 = t1.R () (1, 2);
            const T r02 = t1.R () (0, 2);

            t3.P () (0) = (-p0 + t2.P () (0)) * t1.R () (0, 0) +
                          (-p1 + t2.P () (1)) * t1.R () (1, 0) +
                          (-p2 + t2.P () (2)) * t1.R () (2, 0);

            t3.P () (1) = (-p0 + t2.P () (0)) * r01 + (-p1 + t2.P () (1)) * t1.R () (1, 1) +
                          (-p2 + t2.P () (2)) * t1.R () (2, 1);

            t3.P () (2) = (-p0 + t2.P () (0)) * r02 + (-p1 + t2.P () (1)) * r12 +
                          (-p2 + t2.P () (2)) * t1.R () (2, 2);

            t3.R () (0, 1) = t1.R () (0, 0) * t2.R () (0, 1) + t1.R () (1, 0) * t2.R () (1, 1) +
                             t1.R () (2, 0) * t2.R () (2, 1);
            t3.R () (0, 2) = t1.R () (0, 0) * t2.R () (0, 2) + t1.R () (1, 0) * t2.R () (1, 2) +
                             t1.R () (2, 0) * t2.R () (2, 2);
            t3.R () (0, 0) = t1.R () (0, 0) * t2.R () (0, 0) + t1.R () (1, 0) * t2.R () (1, 0) +
                             t1.R () (2, 0) * t2.R () (2, 0);

            t3.R () (1, 0) = r01 * t2.R () (0, 0) + t1.R () (1, 1) * t2.R () (1, 0) +
                             t1.R () (2, 1) * t2.R () (2, 0);
            t3.R () (1, 2) = r01 * t2.R () (0, 2) + t1.R () (1, 1) * t2.R () (1, 2) +
                             t1.R () (2, 1) * t2.R () (2, 2);
            t3.R () (1, 1) = r01 * t2.R () (0, 1) + t1.R () (1, 1) * t2.R () (1, 1) +
                             t1.R () (2, 1) * t2.R () (2, 1);

            t3.R () (2, 0) =
                r02 * t2.R () (0, 0) + r12 * t2.R () (1, 0) + t1.R () (2, 2) * t2.R () (2, 0);
            t3.R () (2, 1) =
                r02 * t2.R () (0, 1) + r12 * t2.R () (1, 1) + t1.R () (2, 2) * t2.R () (2, 1);
            t3.R () (2, 2) =
                r02 * t2.R () (0, 2) + r12 * t2.R () (1, 2) + t1.R () (2, 2) * t2.R () (2, 2);
            return t3;
        }

        /**
         * @brief creates a transformation that is positioned in \b eye and looking toward
         * \b center along -z where \b up indicates the upward direction along which the y-axis
         * is placed. Same convention as for gluLookAt
         * and is handy for placing a cameraview.
         * @param eye [in] position of view
         * @param center [in] point to look toward
         * @param up [in] the upward direction (the
         * @return Transformation
         */
        static Transform3D< T > makeLookAt (const rw::math::Vector3D< T >& eye, const rw::math::Vector3D< T >& center,
                                            const rw::math::Vector3D< T >& up)
        {
            rw::math::Vector3D< T > f (center - eye);
            f = normalize (f);
            rw::math::Vector3D< T > s (cross (f, up));
            s = normalize (s);
            rw::math::Vector3D< T > u (cross (s, f));
            u = normalize (u);

            rw::math::Rotation3D< T > R (s[0], s[1], s[2], u[0], u[1], u[2], -f[0], -f[1], -f[2]);

            return inverse (Transform3D (R * -eye, R));
        }

        /**
         * @brief Returns a Eigen 4x4 matrix @f$ \mathbf{M}\in SE(3)
         * @f$ that represents this homogeneous transformation
         *
         * @return @f$ \mathbf{M}\in SE(3) @f$
         */
        Eigen::Matrix<T,4,4> e () const;

      private:
        rw::math::Vector3D< T > _d;
        rw::math::Rotation3D< T > _R;
    };

// Explicit template specifications.
#if !defined(SWIG)
    extern template class rw::math::Transform3D< double >;
    extern template class rw::math::Transform3D< float >;
#else
 
#if SWIG_VERSION < 0x040000
    SWIG_DECLARE_TEMPLATE (Transform3Dd, rw::math::Transform3D< double >);
    ADD_DEFINITION (Transform3Dd, Transform3D,sdurw_math)
#else
    SWIG_DECLARE_TEMPLATE (Transform3D, rw::math::Transform3D< double >);
#endif

    SWIG_DECLARE_TEMPLATE (Transform3Df, rw::math::Transform3D< float >);
#endif

    using Transform3Dd = Transform3D< double >;
    using Transform3Df = Transform3D< float >;

#if !defined(SWIGJAVA)
    /**
     * @brief Calculates
     * @f$ \robabx{b}{a}{\mathbf{T}} = \robabx{a}{b}{\mathbf{T}}^{-1} @f$
     *
     * @relates Transform3D
     *
     * @param aTb [in] the transform matrix @f$ \robabx{a}{b}{\mathbf{T}} @f$
     * @return @f$ \robabx{b}{a}{\mathbf{T}} = \robabx{a}{b}{\mathbf{T}}^{-1} @f$
     *
     * @f$
     * \robabx{a}{b}{\mathbf{T}}^{-1} =
     * \left[
     *  \begin{array}{cc}
     *  \robabx{a}{b}{\mathbf{R}}^{T} & - \robabx{a}{b}{\mathbf{R}}^{T} \robabx{a}{b}{\mathbf{d}} \\
     *  \begin{array}{ccc}0 & 0 & 0\end{array} & 1
     *  \end{array}
     * \right]
     *
     * @f$
     */

     #endif 
    template< class T > const Transform3D< T > inverse (const Transform3D< T >& aTb)
    {
        return Transform3D< T > (-(inverse (aTb.R ()) * aTb.P ()), inverse (aTb.R ()));
    }

    /**
     * @brief Cast Transform3D<T> to Transform3D<Q>
     * @param trans [in] Transform3D with type T
     * @return Transform3D with type Q
     */
    template< class Q, class T > const Transform3D< Q > cast (const Transform3D< T >& trans)
    {
        Transform3D< Q > res;
        for (size_t i = 0; i < 3; i++)
            for (size_t j = 0; j < 4; j++)
                res (i, j) = static_cast< Q > (trans (i, j));
        return res;
    }

    /*@}*/

}}    // namespace rw::math

namespace rw { namespace common {
    class OutputArchive;
    class InputArchive;
    namespace serialization {
        /**
         * @copydoc rw::common::serialization::write
         * @relatedalso rw::math::Transform3D
         */
        template<>
        void write (const rw::math::Transform3D< double >& sobject,
                    rw::common::OutputArchive& oarchive, const std::string& id);

        /**
         * @copydoc rw::common::serialization::write
         * @relatedalso rw::math::Transform3D
         */
        template<>
        void write (const rw::math::Transform3D< float >& sobject,
                    rw::common::OutputArchive& oarchive, const std::string& id);

        /**
         * @copydoc rw::common::serialization::read
         * @relatedalso rw::math::Transform3D
         */
        template<>
        void read (rw::math::Transform3D< double >& sobject, rw::common::InputArchive& iarchive,
                   const std::string& id);

        /**
         * @copydoc rw::common::serialization::read
         * @relatedalso rw::math::Transform3D
         */
        template<>
        void read (rw::math::Transform3D< float >& sobject, rw::common::InputArchive& iarchive,
                   const std::string& id);
    }    // namespace serialization
}}       // namespace rw::common

namespace boost { namespace serialization {
    /**
     * @brief Boost serialization.
     * @param archive [in] the boost archive to read from or write to.
     * @param transform [in/out] the transformation to read/write.
     * @param version [in] class version (currently version 0).
     * @relatedalso rw::math::Transform3D
     */
    template< class Archive, class T >
    void serialize (Archive& archive, rw::math::Transform3D< T >& transform,
                    const unsigned int version)
    {
        archive& transform.P ();
        archive& transform.R ();
    }
}}    // namespace boost::serialization

#endif    // end include guard
