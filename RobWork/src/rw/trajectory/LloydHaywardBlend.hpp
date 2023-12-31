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

#ifndef RW_TRAJECTORY_LLOYDHAYWARDBLEND_HPP
#define RW_TRAJECTORY_LLOYDHAYWARDBLEND_HPP

/**
 * @file LloydHaywardBlend.hpp
 */
#if !defined(SWIG)
#include <rw/trajectory/Blend.hpp>
#include <rw/trajectory/Interpolator.hpp>
#include <rw/trajectory/InterpolatorUtil.hpp>

#include <rw/math/Rotation3D.hpp>
#include <rw/math/Transform3D.hpp>
#endif
namespace rw { namespace trajectory {

    /** @addtogroup trajectory */
    /*@{*/

    /**
     * @brief Implements LloydHayward blending
     *
     * The LloydHayward blend [1], makes a smooth continous differentiable transition between
     * two interpolators.
     *
     * [1]: J. Lloyd, V. Hayward. Real-Time Trajectory Generation Using Blend Functions,
     *      Proc. Int. Conf. on Robotics and Automation, 1991, pp. 784-798.
     */
    template< class T > class LloydHaywardBlend : public Blend< T >
    {
      public:
        /**
         * @brief Constructs LloydHaywardBlend between \b interpolator1 and \b interpolator2.
         *
         * The blend starts \b tau before the end of \b interpolator1 and finished \b tau after the
         * start of \b interpolator2. The constant \b kappa specifies characteristics of the blend
         * as described in [1].
         *
         * @param interpolator1 [in] First interpolator, no ownership transferred
         * @param interpolator2 [in] Second interpolator, no ownership transferred
         * @param tau [in] Blend time
         * @param kappa [in] Blend characteristic (default 15/2 for acceleration minimal blend
         * between linie segments)
         */
        LloydHaywardBlend (Interpolator< T >* interpolator1, Interpolator< T >* interpolator2,
                           double tau, double kappa = 15 / 2)
        {
            _x1    = interpolator1;
            _x2    = interpolator2;
            _tau   = tau;
            _kappa = kappa;
        }

        /**
         * @brief Destructor
         */
        virtual ~LloydHaywardBlend () {}

        /**
         * @copydoc Blend::x
         */
        virtual T x (double t) const
        {
            double s     = t / (2 * _tau);
            double s3    = s * s * s;
            double s4    = s3 * s;
            double s5    = s4 * s;
            double alpha = 6 * s5 - 15 * s4 + 10 * s3;
            double s6    = s5 * s;
            double beta  = s6 - 3 * s5 + 3 * s4 - s3;
            // Get position of blended path segments
            T x1 = _x1->x (_x1->duration () - _tau + t);
            T x2 = _x2->x (t - _tau);
            // Get velocity of the path segments
            T v1 = _x1->dx (_x1->duration () - _tau + t);
            T v2 = _x2->dx (t - _tau);
            T vd = (v2 - v1) * _tau / 0.5;
            return x1 + alpha * (x2 - x1) - vd * _kappa * beta;
        }

        /**
         * @copydoc Blend::dx
         */
        virtual T dx (double t) const
        {
            double s      = t / (2 * _tau);
            double s2     = s * s;
            double s3     = s2 * s;
            double s4     = s3 * s;
            double dalpha = 30 * s4 - 60 * s3 + 30 * s2;
            double s5     = s4 * s;
            double dbeta  = 6 * s5 - 15 * s4 + 12 * s3 - 3 * s2;

            double alpha = 6 * s5 - 15 * s4 + 10 * s3;

            double index_x1 = _x1->duration () - _tau + t;
            double index_x2 = t - _tau;

            // Get position of path segments
            T x1 = _x1->x (index_x1);
            T x2 = _x2->x (index_x2);
            // Get velocity of path segments
            T v1 = _x1->dx (index_x1) * _tau / 0.5;
            T v2 = _x2->dx (index_x2) * _tau / 0.5;

            T dv = (v2 - v1);
            return (v1 + dalpha * (x2 - x1) + alpha * (dv) -_kappa * dv * dbeta) * 0.5 / _tau;
        }

        /**
         * @copydoc Blend::ddx
         */
        virtual T ddx (double t) const
        {
            double s       = t / (2 * _tau);
            double s2      = s * s;
            double s3      = s2 * s;
            double s4      = s3 * s;
            double s5      = s4 * s;
            double alpha   = 6 * s5 - 15 * s4 + 10 * s3;
            double dalpha  = 30 * s4 - 60 * s3 + 30 * s2;
            double ddalpha = 120 * s3 - 180 * s2 + 60 * s;
            double ddbeta  = 30 * s4 - 60 * s3 + 36 * s2 - 6 * s;

            double index_x1 = _x1->duration () - _tau + t;
            double index_x2 = t - _tau;
            T x1            = _x1->x (index_x1);
            T x2            = _x2->x (index_x2);

            double scale = _tau / 0.5;
            T v1         = _x1->dx (index_x1) * scale;
            T v2         = _x2->dx (index_x2) * scale;

            T a1 = _x1->ddx (index_x1) * scale * scale;
            T a2 = _x2->ddx (index_x2) * scale * scale;

            T dv  = (v2 - v1);
            T ddv = (a2 - a1);

            return (a1 + ddalpha * (x2 - x1) + 2 * dalpha * dv + alpha * ddv -
                    _kappa * dv * ddbeta) /
                   (scale * scale);
        }

        /**
         * @copydoc Blend::tau1()
         *
         * @note For ParabolicBlend getTau1()==getTau2()
         */
        virtual double tau1 () const { return _tau; }

        /**
         * @copydoc Blend::tau2()
         *
         * @note For ParabolicBlend getTau1()==getTau2()
         */
        virtual double tau2 () const { return _tau; }

        /**
         * @brief Returns the kappa value used in the blend
         */
        double kappa () const { return _kappa; }

      private:
        Interpolator< T >* _x1;
        Interpolator< T >* _x2;
        double _tau;
        double _kappa;
    };

    /**
     * @brief Template specialization of LloydHaywardBlend for using a rw::math::Rotation3D<T>
     *
     * The transform is encoded as a vector storing the position and the orientation as a
     * quaternion.
     */
    template< class T >
    class LloydHaywardBlend< rw::math::Rotation3D< T > > : public Blend< rw::math::Rotation3D< T > >
    {
      public:
        /**
         * @brief Constructs LloydHaywardBlend between \b inter1 and \b inter2.
         *
         * The blend starts \b tau before the end of \b inter1 and finished \b tau after the start
         * of \b inter2. The constant \b kappa specifies characteristics of the blend as described
         * in [1].
         *
         * @param inter1 [in] First interpolator, no ownership transferred
         * @param inter2 [in] Second interpolator, no ownership transferred
         * @param tau [in] Blend time
         * @param kappa [in] Blend characteristic (default 15/2 for acceleration minimal blend
         * between linie segments)
         */
        LloydHaywardBlend (Interpolator< rw::math::Rotation3D< T > >* inter1,
                           Interpolator< rw::math::Rotation3D< T > >* inter2, double tau,
                           double kappa  = 15 / 2)
        {
            RW_THROW ("Rotation3D not supported in LloydHaywardBlend");
        }

        /**
         * @brief Destructor
         */
        virtual ~LloydHaywardBlend () {}

        /**
         * @cond
         * @copydoc Blend::x
         * @endcond
         */
        rw::math::Rotation3D< T > x (double t) const { return rw::math::Rotation3D<T>::identity (); }

        /**
         * @cond
         * @copydoc Blend::dx
         * @endcond
         */
        rw::math::Rotation3D< T > dx (double t) const
        {
            return rw::math::Rotation3D<T>::identity ();
        }

        /**
         * @cond
         * @copydoc Blend::ddx
         * @endcond
         */
        rw::math::Rotation3D< T > ddx (double t) const
        {
            return rw::math::Rotation3D<T>::identity ();
        }

        /**
         * @cond
         * @copydoc Blend::tau1()
         * @endcond
         *
         * @note For ParabolicBlend getTau1()==getTau2()
         */
        double tau1 () const { return 0; }

        /**
         * @cond
         * @copydoc Blend::tau2()
         * @endcond
         *
         * @note For ParabolicBlend getTau1()==getTau2()
         */
        double tau2 () const { return 0; }

        /**
         * @brief Returns the kappa value used in the blend
         */
        double kappa () const { return 0; }
    };

    /**
     * @brief Template specialization of LloydHaywardBlend for using a rw::math::Transform3D<T>
     *
     * The transform is encoded as a vector storing the position and the orientation as a
     * quaternion.
     */
    template< class T >
    class LloydHaywardBlend< rw::math::Transform3D< T > >
        : public Blend< rw::math::Transform3D< T > >
    {
      public:
        /**
         * @brief Constructs LloydHaywardBlend between \b inter1 and \b inter2.
         *
         * The blend starts \b tau before the end of \b inter1 and finished \b tau after the start
         * of \b inter2. The constant \b kappa specifies characteristics of the blend as described
         * in [1].
         *
         * @param inter1 [in] First interpolator, no ownership transferred
         * @param inter2 [in] Second interpolator, no ownership transferred
         * @param tau [in] Blend time
         * @param kappa [in] Blend characteristic (default 15/2 for acceleration minimal blend
         * between linie segments)
         */
        LloydHaywardBlend (Interpolator< rw::math::Transform3D< T > >* inter1,
                           Interpolator< rw::math::Transform3D< T > >* inter2, double tau,
                           double kappa = 15 / 2) :
            _wrap1 (inter1),
            _wrap2 (inter2), _blend (&_wrap1, &_wrap2, tau, kappa)
        {}

        /**
         * @brief Destructor
         */
        virtual ~LloydHaywardBlend () {}

        /**
         * @cond
         * @copydoc Blend::x
         * @endcond
         */
        rw::math::Transform3D< T > x (double t) const
        {
            return InterpolatorUtil::vecToTrans< V, T > (_blend.x (t));
        }

        /**
         * @cond
         * @copydoc Blend::dx
         * @endcond
         */
        rw::math::Transform3D< T > dx (double t) const
        {
            return InterpolatorUtil::vecToTrans< V, T > (_blend.dx (t));
        }

        /**
         * @cond
         * @copydoc Blend::ddx
         * @endcond
         */
        rw::math::Transform3D< T > ddx (double t) const
        {
            return InterpolatorUtil::vecToTrans< V, T > (_blend.ddx (t));
        }

        /**
         * @cond
         * @copydoc Blend::tau1()
         * @endcond
         *
         * @note For ParabolicBlend getTau1()==getTau2()
         */
        double tau1 () const { return _blend.tau1 (); }

        /**
         * @cond
         * @copydoc Blend::tau2()
         * @endcond
         *
         * @note For ParabolicBlend getTau1()==getTau2()
         */
        double tau2 () const { return _blend.tau2 (); }

        /**
         * @brief Returns the kappa value used in the blend
         */
        double kappa () const { return _blend.kappa (); }

      private:
        typedef Eigen::Matrix< T, 7, 1 > V;
        InterpolatorUtil::Transform2VectorWrapper< V, T > _wrap1;
        InterpolatorUtil::Transform2VectorWrapper< V, T > _wrap2;
        LloydHaywardBlend< V > _blend;
    };

    /** @} */

}}    // namespace rw::trajectory

#endif    // RW_TRAJECTORY_LLOYDHAYWARDBLEND_HPP
