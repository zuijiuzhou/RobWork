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

#ifndef RW_TRAJECTORY_CUBICSPLINEINTERPOLATOR_HPP
#define RW_TRAJECTORY_CUBICSPLINEINTERPOLATOR_HPP

/**
 * @file CubicSplineInterpolator.hpp
 */
#if !defined(SWIG)
#include <rw/trajectory/Interpolator.hpp>

#include <rw/core/macros.hpp>
#include <rw/math/Transform3DVector.hpp>
#endif
namespace rw { namespace trajectory {

    /** @addtogroup trajectory */
    /*@{*/

    /**
     * @brief This class represents a 3-degree polynomial function, used
     * in Cubic Splines hence the name CubicSegment.
     *
     * \f$ \bf{f}(t)= \bf{a} + \bf{b}\cdot t + \bf{c}\cdot t^2 \bf{d}\cdot t^3 \f$
     */
    template< class T > class CubicSplineInterpolator : public Interpolator< T >
    {
      public:
        CubicSplineInterpolator (const T& a, const T& b, const T& c, const T& d, double duration) :
            _a (a), _b (b), _c (c), _d (d), _duration (duration)
        {
            if (duration <= 0)
                RW_THROW ("Duration of segment must be positive");
        }

        virtual ~CubicSplineInterpolator () {}

        /**
         * @copydoc Interpolator::x
         *
         * @note The cubic polynomial is given by a 3-degree polynomial:
         * \f$ \bf{f}(t)= \bf{a} + \bf{b}\cdot t + \bf{c}\cdot t^2 \bf{d}\cdot t^3 \f$
         */
        T x (double t) const
        {
            // t /= _duration;
            double tpow2 = t * t;
            double tpow3 = tpow2 * t;
            return _a + _b * t + _c * tpow2 + _d * tpow3;
        }

        /**
         * @copydoc Interpolator::dx
         *
         * @note The derivative is a 2-degree polynomial:
         * \f$ \bf{df}(t)= \bf{b} + 2\cdot \bf{c}\cdot t + 3\cdot \bf{d}\cdot t^2 \f$
         */
        T dx (double t) const { return _b + _c * 2 * t + _d * 3 * t * t; }

        /**
         * @copydoc Interpolator::ddx
         *
         * @note The second derivative is a 1-degree polynomial:
         * \f$ \bf{df}(t)= 2\cdot \bf{c} + 6\cdot \bf{d}\cdot t \f$
         */
        T ddx (double t) const { return _c * 2 + _d * t * 6; }

        /**
         * @copydoc Interpolator::duration
         */
        double duration () const { return _duration; }

      private:
        T _a;
        T _b;
        T _c;
        T _d;
        double _duration;
    };

    template< class T >
    class CubicSplineInterpolator< rw::math::Rotation3D< T > >
        : public Interpolator< rw::math::Rotation3D< T > >
    {
      public:
        CubicSplineInterpolator (const rw::math::Rotation3D< T >& a,
                                 const rw::math::Rotation3D< T >& b,
                                 const rw::math::Rotation3D< T >& c,
                                 const rw::math::Rotation3D< T >& d, double duration);

        virtual ~CubicSplineInterpolator () {}

        /**
         * @copydoc Interpolator::x
         *
         * @note The cubic polynomial is given by a 3-degree polynomial:
         * \f$ \bf{f}(t)= \bf{a} + \bf{b}\cdot t + \bf{c}\cdot t^2 \bf{d}\cdot t^3 \f$
         */
        rw::math::Rotation3D< T > x (double t) const;

        /**
         * @copydoc Interpolator::dx
         *
         * @note The derivative is a 2-degree polynomial:
         * \f$ \bf{df}(t)= \bf{b} + 2\cdot \bf{c}\cdot t + 3\cdot \bf{d}\cdot t^2 \f$
         */
        rw::math::Rotation3D< T > dx (double t) const;

        /**
         * @copydoc Interpolator::ddx
         *
         * @note The second derivative is a 1-degree polynomial:
         * \f$ \bf{df}(t)= 2\cdot \bf{c} + 6\cdot \bf{d}\cdot t \f$
         */
        rw::math::Rotation3D< T > ddx (double t) const;

        /**
         * @copydoc Interpolator::duration
         */
        double duration () const { return _duration; }

      private:
        double _duration;
    };

    template< class T >
    class CubicSplineInterpolator< rw::math::Transform3D< T > >
        : public Interpolator< rw::math::Transform3D< T > >
    {
      public:
        CubicSplineInterpolator (const rw::math::Transform3D< T >& a,
                                 const rw::math::Transform3D< T >& b,
                                 const rw::math::Transform3D< T >& c,
                                 const rw::math::Transform3D< T >& d, double duration);

        virtual ~CubicSplineInterpolator () {}

        /**
         * @copydoc Interpolator::x
         *
         * @note The cubic polynomial is given by a 3-degree polynomial:
         * \f$ \bf{f}(t)= \bf{a} + \bf{b}\cdot t + \bf{c}\cdot t^2 \bf{d}\cdot t^3 \f$
         */
        rw::math::Transform3D< T > x (double t) const;

        /**
         * @copydoc Interpolator::dx
         *
         * @note The derivative is a 2-degree polynomial:
         * \f$ \bf{df}(t)= \bf{b} + 2\cdot \bf{c}\cdot t + 3\cdot \bf{d}\cdot t^2 \f$
         */
        rw::math::Transform3D< T > dx (double t) const;

        /**
         * @copydoc Interpolator::ddx
         *
         * @note The second derivative is a 1-degree polynomial:
         * \f$ \bf{df}(t)= 2\cdot \bf{c} + 6\cdot \bf{d}\cdot t \f$
         */
        rw::math::Transform3D< T > ddx (double t) const;

        /**
         * @copydoc Interpolator::duration
         */
        double duration () const { return _duration; }

      private:
        double _duration;
    };

    template< class T >
    class CubicSplineInterpolator< rw::math::Transform3DVector< T > >
        : public Interpolator< rw::math::Transform3DVector< T > >
    {
      public:
        CubicSplineInterpolator (const rw::math::Transform3DVector< T >& a,
                                 const rw::math::Transform3DVector< T >& b,
                                 const rw::math::Transform3DVector< T >& c,
                                 const rw::math::Transform3DVector< T >& d, double duration) :
            _quatInt (a.toQuaternion (), b.toQuaternion (), c.toQuaternion (), d.toQuaternion (),
                      duration),
            _vecInt (a.toVector3D (), b.toVector3D (), c.toVector3D (), d.toVector3D (), duration),
            _duration (duration)
        {
            if (duration <= 0)
                RW_THROW ("Duration of segment must be positive");
        }

        virtual ~CubicSplineInterpolator () {}

        /**
         * @copydoc Interpolator::x
         *
         * @note The cubic polynomial is given by a 3-degree polynomial:
         * \f$ \bf{f}(t)= \bf{a} + \bf{b}\cdot t + \bf{c}\cdot t^2 \bf{d}\cdot t^3 \f$
         */
        rw::math::Transform3DVector< T > x (double t) const
        {
            return rw::math::Transform3DVector< T > (_vecInt.x (t), _quatInt.x (t));
        }

        /**
         * @copydoc Interpolator::dx
         *
         * @note The derivative is a 2-degree polynomial:
         * \f$ \bf{df}(t)= \bf{b} + 2\cdot \bf{c}\cdot t + 3\cdot \bf{d}\cdot t^2 \f$
         */
        rw::math::Transform3DVector< T > dx (double t) const
        {
            return rw::math::Transform3DVector< T > (_vecInt.dx (t), _quatInt.dx (t));
        }

        /**
         * @copydoc Interpolator::ddx
         *
         * @note The second derivative is a 1-degree polynomial:
         * \f$ \bf{df}(t)= 2\cdot \bf{c} + 6\cdot \bf{d}\cdot t \f$
         */
        rw::math::Transform3DVector< T > ddx (double t) const
        {
            return rw::math::Transform3DVector< T > (_vecInt.ddx (t), _quatInt.ddx (t));
        }

        /**
         * @copydoc Interpolator::duration
         */
        double duration () const { return _duration; }

      private:
        CubicSplineInterpolator< rw::math::Quaternion< T > > _quatInt;
        CubicSplineInterpolator< rw::math::Vector3D< T > > _vecInt;
        double _duration;
    };

    /** @} */

}}    // namespace rw::trajectory

#endif /*RW_TRAJECTORY_CUBICSPLINEINTERPOLATOR_HPP*/
