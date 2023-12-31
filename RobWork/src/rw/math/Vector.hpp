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

#ifndef RW_MATH_VECTOR_HPP
#define RW_MATH_VECTOR_HPP

/**
 * @file Vector.hpp
 */
#if !defined(SWIG)
#include <rw/core/macros.hpp>

#include <Eigen/Core>
#endif

namespace rw { namespace math {

    /**
     * @brief Configuration vector
     */
    template< class T = double > class Vector
    {
      public:
        //! The type of the internal Eigen vector implementation.
        typedef Eigen::Matrix< T, Eigen::Dynamic, 1 > Base;

        /**
         * @brief A configuration of vector of length \b dim.
         */
        explicit Vector (size_t dim) : _vec (dim) {}

        /**
         * @brief Default constructor.
         *
         * The vector will be of dimension zero.
         */
        Vector () : _vec (0) {}

        /**
         * @brief Creates a Vector of length \b n and initialized with values from \b values
         *
         * The method reads n values from \b values and do not check whether reading out of bounds.
         *
         * @param n [in] Length of q.
         * @param values [in] Values to initialize with
         */
        Vector (size_t n, const T* values) : _vec (n)
        {
            for (size_t i = 0; i < n; i++)
                _vec (i) = values[i];
        }

        /**
         * @brief Creates a Vector of length \b n and initialize all values in Vector to \b value
         *
         * @param n [in] Length of q.
         * @param value [in] Value to initialize
         */
        Vector (size_t n, T value) : _vec (n)
        {
            for (size_t i = 0; i < n; i++)
                _vec (i) = value;
        }

        /**
         * @brief Returns Vector of length \b n initialized with 0's
         */
        static Vector zero (int n) { return Vector (Base::Zero (n)); }

        /**
         * @brief The dimension of the configuration vector.
         */
        size_t size () const { return e ().size (); }

        /**
           @brief True if the configuration is of dimension zero.
         */
        bool empty () const { return size () == 0; }

        /**
         * @brief Construct a configuration vector from a Boost vector
         * expression.
         *
         * @param r [in] An expression for a vector of doubles
         */
        template< class R > explicit Vector (const Eigen::MatrixBase< R >& r) : _vec (r) {}

        /**
         * @brief Accessor for the internal Eigen vector state.
         */
        const Base& e () const { return _vec; }

        /**
         * @brief Accessor for the internal Eigen vector state.
         */
        Base& e () { return _vec; }

        /**
           @brief Start of sequence iterator.
        */
        // const_iterator begin() const { return e().begin(); }

        /**
           @brief End of sequence iterator.
        */
        // const_iterator end() const { return e().end(); }

        /**
           @brief Start of sequence iterator.
        */
        // iterator begin() { return e().begin(); }

        /**
           @brief End of sequence iterator.
        */
        // iterator end() { return e().end(); }

        /**
         * @brief Extracts a sub part (range) of this Vector.
         * @param start [in] Start index
         * @param cnt [in] the number of elements to include
         * @return
         */
        const Vector getSubPart (size_t start, size_t cnt) const
        {
            RW_ASSERT (start + cnt <= size ());

            Vector res (cnt);
            for (size_t i = 0; i < cnt; i++) {
                res (i) = (*this)[start + i];
            }
            return res;
        }

        /**
         * @brief Set a part of the vector.
         * @param index [in] first index.
         * @param part [in] the subpart to set.
         */
        void setSubPart (size_t index, const Vector& part)
        {
            RW_ASSERT (index + part.size () <= size ());
            for (size_t i = 0; i < part.size (); i++) {
                (*this)[index + i] = part (i);
            }
        }

        //----------------------------------------------------------------------
        // Norm utility methods

        /**
         * @brief Returns the Euclidean norm (2-norm) of the configuration
         * @return the norm
         */
        T norm2 () const { return e ().norm (); }

        /**
         * @brief Returns the Manhatten norm (1-norm) of the configuration
         * @return the norm
         */
        T norm1 () const { return e ().sum (); }

        /**
         * @brief Returns the infinte norm (\f$\inf\f$-norm) of the configuration
         * @return the norm
         */
        T normInf () const
        {
            Eigen::VectorXd tmp = e ().template cast< double > ();
            return (T) tmp.lpNorm< Eigen::Infinity > ();
        }

        //----------------------------------------------------------------------
        // Various operators

#if !defined(SWIG)
        /**
         * @brief Returns reference to vector element
         * @param i [in] index in the vector
         * @return const reference to element
         */
        const T& operator() (size_t i) const { return e () (i); }

        /**
         * @brief Returns reference to vector element
         * @param i [in] index in the vector
         * @return reference to element
         */
        T& operator() (size_t i) { return e () (i); }

        /**
         * @brief Returns reference to vector element
         * @param i [in] index in the vector
         * @return const reference to element
         */
        const T& operator[] (size_t i) const { return e () (i); }

        /**
         * @brief Returns reference to vector element
         * @param i [in] index in the vector
         * @return reference to element
         */
        T& operator[] (size_t i) { return e () (i); }
#else
        ARRAYOPERATOR (T);
#endif
        /**
           @brief Scalar division.
         */
        const Vector operator/ (T s) const { return Vector (e () / s); }

        /**
         * @brief Scalar multiplication.
         */
        const Vector operator* (T s) const { return Vector (e () * s); }
#if !defined(SWIG)
        /**
         * @brief Scalar multiplication.
         */
        friend const Vector operator* (T s, const Vector& v) { return Vector (s * v.e ()); }
#endif 
        /**
         * @brief Vector subtraction.
         */
        const Vector operator- (const Vector& b) const { return Vector (e () - b.e ()); }

        /**
         * @brief Vector addition.
         */
        const Vector operator+ (const Vector& b) const { return Vector (e () + b.e ()); }

        /**
         * @brief Scalar multiplication.
         */
        Vector& operator*= (T s)
        {
            e () *= s;
            return *this;
        }

        /**
         * @brief Scalar division.
         */
        Vector& operator/= (T s)
        {
            e () /= s;
            return *this;
        }

        /**
         * @brief Vector addition.
         */
        Vector& operator+= (const Vector& v)
        {
            _vec += v.e ();
            return *this;
        }

        /**
         * @brief Vector subtraction.
         */
        Vector& operator-= (const Vector& v)
        {
            _vec -= v.e ();
            return *this;
        }

        /**
         * @brief Unary minus.
         */
        const Vector operator- () const { return Vector (-e ()); }

        /**
         * @brief Compares whether this is less than \b q
         *
         * The less operator is defined such that the first index is the most significant. That is
         * if (*this)[0] < q[0] then true is returned. If (*this)[0] > q[0] false is returned and
         * only if (*this)[0] == q[0] is the next index considered.
         */
        bool operator< (const Vector& q) const
        {
            RW_ASSERT (size () == q.size ());
            for (size_t i = 0; i < size (); i++) {
                if (_vec[i] < q[i])
                    return true;
                else if (_vec[i] > q[i])
                    return false;
            }
            return false;
        }

      private:
        Base _vec;
    };

    /**
     * @brief Compares \b q1 and \b q2 for equality.
     *
     * \b q1 and \b q2 are considered equal if and only if they have equal
     * length and if q1(i) == q2(i) for all i.
     *
     * @relates Vector
     *
     * @param q1 [in]
     * @param q2 [in]
     * @return True if q1 equals q2, false otherwise.
     */
    template< class A > bool operator== (const Vector< A >& q1, const Vector< A >& q2)
    {
        if (q1.size () != q2.size ())
            return false;

        for (size_t i = 0; i < q1.size (); i++)
            if (q1 (i) != q2 (i))
                return false;
        return true;
    }

    /**
       @brief Inequality operator

       The inverse of operator==().
     */
    template< class A > inline bool operator!= (const Vector< A >& q1, const Vector< A >& q2)
    {
        return !(q1 == q2);
    }

    /**
     * @brief Streaming operator.
     *
     * @relates Vector
     */
    template< class A > std::ostream& operator<< (std::ostream& out, const Vector< A >& v)
    {
        if (v.size () == 0)
            return out << "Q[0]{}";
        else {
            out << "Q[" << (int) v.size () << "]{";
            for (size_t i = 0; i < v.size () - 1; i++)
                out << v[i] << ", ";
            return out << v[v.size () - 1] << "}";
        }
    }

    /**
     * @brief Input streaming operator
     *
     * Parse input stream according to how operator<< streams out
     *
     * @relates Vector
     * @param in [in] Input stream
     * @param q [in] Target of q read in
     * @return reference to \b in
     */
    template< class A > std::istream& operator>> (std::istream& in, Vector< A >& q)
    {
        char ch1, ch2;
        do {
            in.get (ch1);
        } while (ch1 == ' ' || ch1 == '\t');    // Ignore space and tab, but not line changes.

        int size = -1;

        if (ch1 == 'Q') {
            in.get (ch2);
            if (ch1 != 'Q' || ch2 != '[')
                RW_THROW ("Content of input stream does not match format of Q");
            in >> size;

            in.get (ch1);
            in.get (ch2);
            if (ch1 != ']' || ch2 != '{')
                RW_THROW ("Content of input stream does not match format of Q");
        }
        else if (ch1 != '{') {
            RW_THROW ("Content of input stream does not match format of Q");
        }

        std::vector< double > res;
        while (ch1 != '}') {
            double d;
            in >> d;
            if (!in.eof ()) {
                res.push_back (d);
            }
            in.get (ch1);
        }

        if (ch1 != '}')
            RW_THROW ("Content of input stream does not match format of Q");

        if (size > -1 && (int) res.size () != size) {
            RW_THROW ("Length of Q does not match device");
        }

        q = Vector< A > (res.size (), &res[0]);
        return in;
    }

    /**
       @brief The dot product (inner product) of \b a and \b b.

       @relates Vector
    */
    template< class A > A dot (const Vector< A >& a, const Vector< A >& b)
    {
        return a.e ().dot (b.e ());
    }

    /**
     * @brief concatenates q1 onto q2 such that the returned q has
     * the configurations of q1 in [0;q1.size()[ and has q2 in
     * [q1.size();q1.size()+q2.size()[
     * @param q1 [in] the first Vector
     * @param q2 [in] the second Vector
     * @return the concatenation of q1 and q2
     */
    template< class A > rw::math::Vector< A > concat (const Vector< A >& q1, const Vector< A >& q2)
    {
        Vector< A > q (q1.size () + q2.size ());
        for (size_t i = 0; i < q1.size (); i++)
            q (i) = q1 (i);
        for (size_t i = 0; i < q2.size (); i++)
            q (q1.size () + i) = q2 (i);
        return q;
    }
#if !defined(SWIG)
    extern template class rw::math::Vector< double >;
    extern template class rw::math::Vector< float >;
#else

#if SWIG_VERSION < 0x040000
    SWIG_DECLARE_TEMPLATE (Vectord, rw::math::Vector< double >);
    ADD_DEFINITION (Vectord, Vector,sdurw_math)
#else
    SWIG_DECLARE_TEMPLATE(Vector, rw::math::Vector< double >);
#endif

    SWIG_DECLARE_TEMPLATE (Vectorf, rw::math::Vector< float >);
#endif
    using Vectord = Vector< double >;
    using Vectorf = Vector< float >;

    /*@}*/

}}    // namespace rw::math

#endif    // end include guard
