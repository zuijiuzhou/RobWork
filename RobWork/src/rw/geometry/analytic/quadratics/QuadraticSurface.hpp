/********************************************************************************
 * Copyright 2017 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#ifndef RW_GEOMETRY_ANALYTIC_QUADRATICS_QUADRATICSURFACE_HPP_
#define RW_GEOMETRY_ANALYTIC_QUADRATICS_QUADRATICSURFACE_HPP_

/**
 * @file QuadraticSurface.hpp
 *
 * \copydoc rw::geometry::QuadraticSurface
 */
#if !defined(SWIG)
#include <rw/geometry/analytic/quadratics/QuadraticCurve.hpp>

#include <rw/geometry/PlainTriMesh.hpp>
#include <rw/geometry/analytic/ImplicitSurface.hpp>
#include <rw/math/Transform3D.hpp>
#include <rw/math/Vector3D.hpp>

#include <Eigen/Core>
#include <vector>
#endif
namespace rw { namespace geometry {
    class TriMesh;

    //! @addtogroup geometry
#if !defined(SWIG)
    //! @{
#endif
    /**
     * @brief A quadratic surface.
     *
     * The general quadratic surface is described as an implicit surface of the form:
     *
     * \f$ x^T A x + 2 a^T x + u = 0\f$
     *
     * where
     *
     * A is a symmetric matrix, \f$ A \in \mathbb{R}^{3\times3} \f$ , and \f$ a \in \mathbb{R}^3, u
     * \in \mathbb{R}\f$
     */

    #if !defined(SWIGJAVA)
    class QuadraticSurface : public ImplicitSurface
    #else 
    class QuadraticSurface
    #endif 
    {
      public:
        //! @brief Smart pointer type for QuadraticSurface
        typedef rw::core::Ptr< QuadraticSurface > Ptr;

        //! @brief Smart pointer type for const QuadraticSurface
        typedef rw::core::Ptr< const QuadraticSurface > CPtr;

        /**
         * @brief A trimming region is defined using an ImplicitSurface.
         *
         * A point is only considered part of this surface, if all trimming conditions evaluate to a
         * negative value.
         */
        typedef rw::geometry::ImplicitSurface::CPtr TrimmingRegion;

        /**
         * @brief Construct new quadratic surface of the implicit form \f$ x^T A x + 2 a^T x + u =
         * 0\f$ when A is diagonal.
         *
         * Some functions, such as #getTriMesh and #extremums, work on a diagonalized surface.
         * When this constructor is used, some effort is saved as the surface is already known to be
         * diagonalized.
         *
         * For a diagonalized surface, all scaled, cloned and translated surfaces will also be
         * diagonalized surfaces.
         *
         * @param A [in] the diagonal of the A matrix.
         * @param a [in] the vector \f$ a \in \mathbb{R}^3\f$ .
         * @param u [in] the scalar offset \f$ u \in \mathbb{R} \f$ .
         * @param conditions [in] (optional) list of trimming conditions.
         */
        QuadraticSurface (
            const Eigen::Diagonal< Eigen::Matrix3d >& A, const Eigen::Vector3d& a, double u,
            const std::vector< TrimmingRegion >& conditions = std::vector< TrimmingRegion > ());

        //! @copydoc QuadraticSurface(const Eigen::Diagonal<Eigen::Matrix3d>&, const
        //! Eigen::Vector3d&, double, const std::vector<TrimmingRegion>&)
        QuadraticSurface (
            const Eigen::DiagonalMatrix< double, 3, 3 >& A, const Eigen::Vector3d& a, double u,
            const std::vector< TrimmingRegion >& conditions = std::vector< TrimmingRegion > ());

        /**
         * @brief Construct new quadratic surface of the implicit form \f$ x^T A x + 2 a^T x + u =
         * 0\f$ when A is non-diagonal.
         * @param A [in] a view of the upper part of the symmetric matrix \f$ A \in
         * \mathbb{R}^{3\times3} \f$ . Use the Eigen function A.selfadjointView<Eigen::Upper>() to
         * extract the upper part.
         * @param a [in] the vector \f$ a \in \mathbb{R}^3\f$ .
         * @param u [in] the scalar offset \f$ u \in \mathbb{R} \f$ .
         * @param conditions [in] (optional) list of trimming conditions.
         */
        QuadraticSurface (
            const Eigen::SelfAdjointView< const Eigen::Matrix3d, Eigen::Upper >& A,
            const Eigen::Vector3d& a, double u,
            const std::vector< TrimmingRegion >& conditions = std::vector< TrimmingRegion > ());

        //! @copydoc QuadraticSurface(const Eigen::SelfAdjointView<const Eigen::Matrix3d,
        //! Eigen::Upper>&, const Eigen::Vector3d&, double, const std::vector<TrimmingRegion>&)
        QuadraticSurface (
            const Eigen::SelfAdjointView< Eigen::Matrix3d, Eigen::Upper >& A,
            const Eigen::Vector3d& a, double u,
            const std::vector< TrimmingRegion >& conditions = std::vector< TrimmingRegion > ());

        /**
         * @brief Construct new quadratic surface of the implicit form \f$ x^T A x + 2 a^T x + u =
         * 0\f$ when A is non-diagonal.
         * @param A [in] a view of the lower part of the symmetric matrix \f$ A \in
         * \mathbb{R}^{3\times3} \f$ . Use the Eigen function A.selfadjointView<Eigen::Lower>() to
         * extract the lower part.
         * @param a [in] the vector \f$ a \in \mathbb{R}^3\f$ .
         * @param u [in] the scalar offset \f$ u \in \mathbb{R} \f$ .
         * @param conditions [in] (optional) list of trimming conditions.
         */
        QuadraticSurface (
            const Eigen::SelfAdjointView< const Eigen::Matrix3d, Eigen::Lower >& A,
            const Eigen::Vector3d& a, double u,
            const std::vector< TrimmingRegion >& conditions = std::vector< TrimmingRegion > ());

        //! @copydoc QuadraticSurface(const Eigen::SelfAdjointView<const Eigen::Matrix3d,
        //! Eigen::Lower>&, const Eigen::Vector3d&, double, const std::vector<TrimmingRegion>&)
        QuadraticSurface (
            const Eigen::SelfAdjointView< Eigen::Matrix3d, Eigen::Lower >& A,
            const Eigen::Vector3d& a, double u,
            const std::vector< TrimmingRegion >& conditions = std::vector< TrimmingRegion > ());

        //! @brief Destructor.
        virtual ~QuadraticSurface ();

        // From ImplicitSurface
        //! @copydoc ImplicitSurface::transform(const rw::math::Transform3D<>&) const
        QuadraticSurface::Ptr transform (const rw::math::Transform3D<>& T) const;

        //! @copydoc ImplicitSurface::transform(const rw::math::Vector3D<double>&) const
        QuadraticSurface::Ptr transform (const rw::math::Vector3D<double>& P) const;

        //! @copydoc ImplicitSurface::scale
        QuadraticSurface::Ptr scale (double factor) const;

        //! @copydoc ImplicitSurface::clone
        QuadraticSurface::Ptr clone () const;

        //! @copydoc ImplicitSurface::extremums
        virtual std::pair< double, double > extremums (const rw::math::Vector3D<double>& direction) const;

        //! @copydoc ImplicitSurface::getTriMesh
        virtual rw::core::Ptr< TriMesh >
        getTriMesh (const std::vector< rw::math::Vector3D<double> >& border =
                        std::vector< rw::math::Vector3D<double> > ()) const;

        //! @copydoc ImplicitSurface::setDiscretizationResolution
        virtual void setDiscretizationResolution (double resolution)
        {
            _stepsPerRevolution = resolution;
        }

        //! @copydoc ImplicitSurface::equals
        virtual bool equals (const Surface& surface, double threshold) const;

#if !defined(SWIG)
        //! @copydoc ImplicitSurface::operator()(const rw::math::Vector3D<double>&) const
        virtual double operator() (const rw::math::Vector3D<double>& x) const;
#else 
        CALLOPERATOR(double,const rw::math::Vector3D<double>& );
#endif 

        //! @copydoc ImplicitSurface::insideTrimmingRegion
        virtual bool insideTrimmingRegion (const rw::math::Vector3D<double>& P) const;

        //! @copydoc ImplicitSurface::normal
        virtual rw::math::Vector3D<double> normal (const rw::math::Vector3D<double>& x) const;

        //! @copydoc ImplicitSurface::gradient
        virtual rw::math::Vector3D<double> gradient (const rw::math::Vector3D<double>& x) const;

        //! @copydoc ImplicitSurface::reuseTrimmingRegions
        virtual void reuseTrimmingRegions (rw::geometry::ImplicitSurface::Ptr surface) const;

        //! @brief Get the 3 x 3 symmetric matrix for the second order term in the implicit
        //! formulation.
        const Eigen::Matrix3d& A () const { return _A; }

        //! @brief Get the 3d vector for the first order term in the implicit formulation.
        const Eigen::Vector3d& a () const { return _a; }

        //! @brief Get the scalar for the zero order term in the implicit formulation.
        double u () const { return _u; }

        /**
         * @brief Get the determinant of the \f$ \mathbf{A} \f$ matrix.
         * @return the determinant.
         */
        double determinantA () const { return _determinantA; }

        /**
         * @brief Normalize the implicit expression such that the largest coefficient becomes one.
         *
         * For a quadratic surface, a scaling of \f$ \mathbf{A}, \mathbf{a} \f$ and u with a common
         * factor, will give the exact same surface. This means that the numerical values can get
         * arbitrarily big or small. This functions scales the expression such that the largest
         * element becomes 1.
         *
         * @return a mathematically identical surface, where the coefficients of the defining
         * equation is normalized.
         */
        QuadraticSurface::Ptr normalize () const;

        /**
         * @brief Get the trimming conditions for the surface.
         * @return ImplicitSurface vector specifying the boundary of the surface. If surface is
         * unbounded, the length of the vector is zero.
         */
        const std::vector< TrimmingRegion >& getTrimmingConditions () const { return _conditions; }

        /**
         * @brief Set the trimming conditions of this surface.
         * @param conditions [in] a vector of conditions.
         */
        void setTrimmingConditions (const std::vector< TrimmingRegion >& conditions)
        {
            _conditions = conditions;
        }

        /**
         * @brief Add a trimming condition to this surface.
         * @param condition [in] the condition to add.
         */
        void addTrimmingCondition (const TrimmingRegion& condition)
        {
            _conditions.push_back (condition);
        }

        /**
         * @brief Get a diagonalization of the surface.
         * @return the diagonalized surface, and the rotation transforming this surface into the
         * diagonalized surface.
         */
        std::pair< QuadraticSurface, rw::math::Rotation3D<> > diagonalize () const;

        /**
         * @brief Check if this surface is diagonalized.
         * @return true if A is digaonalized, false otherwise.
         */
        bool diagonalized () const { return _diagonal; }

/** @name Normal forms of Quadratic Surfaces
 * Functions for creation of standard Quadratic Surfaces.
 */
#if !defined(SWIG)
///@{
#endif

#if !defined(SWIGJAVA)
        /**
         * @brief Create an ellipsoid with radii \b a, \b b, and \b c respectively.
         *
         * \image html geometry/quadrics_ellipsoid.gif "Normal form of Quadratic Surface:
         * Ellipsoid."
         *
         * @param a [in] radius in the \f$ x_1 \f$ direction.
         * @param b [in] radius in the \f$ x_2 \f$ direction.
         * @param c [in] radius in the \f$ x_3 \f$ direction.
         * @return a QuadraticSurface representation of an ellipsoid.
         */

         #endif 
        static QuadraticSurface::Ptr makeEllipsoid (double a, double b, double c);


#if !defined(SWIGJAVA)
        /**
         * @brief Create a spheroid (special case of the ellipsoid).
         *
         * \image html geometry/quadrics_spheroid.gif "Normal form of Quadratic Surface: Spheroid
         * (special case of ellipsoid)."
         *
         * @param a [in] radius in the \f$ x_1 \f$ and \f$ x_2 \f$ directions.
         * @param b [in] radius in the \f$ x_3 \f$ direction.
         * @return a QuadraticSurface representation of a spheroid.
         */

         #endif 
        static QuadraticSurface::Ptr makeSpheroid (double a, double b);


#if !defined(SWIGJAVA)
        /**
         * @brief Create a sphere (special case of the ellipsoid and spheroid).
         *
         * \image html geometry/quadrics_sphere.gif "Normal form of Quadratic Surface: Sphere
         * (special case of ellipsoid and spheroid)."
         *
         * @param radius [in] radius of the sphere.
         * @return a QuadraticSurface representation of a sphere.
         */

         #endif 
        static QuadraticSurface::Ptr makeSphere (double radius);

#if !defined(SWIGJAVA)
        /**
         * @brief Create an elliptic paraboloid.
         *
         * \image html geometry/quadrics_elliptic_paraboloid.gif "Normal form of Quadratic Surface:
         * Elliptic Paraboloid."
         *
         * @param a [in] radius of the ellipse in the \f$ x_1\f$ direction when \f$ x_3=1\f$ .
         * @param b [in] radius of the ellipse in the \f$ x_2\f$ direction when \f$ x_3=1\f$ .
         * @return a QuadraticSurface representation of an elliptic paraboloid.
         */

         #endif 
        static QuadraticSurface::Ptr makeEllipticParaboloid (double a, double b);


#if !defined(SWIGJAVA)
        /**
         * @brief Create a circular paraboloid (special case of elliptic paraboloid).
         *
         * \image html geometry/quadrics_circular_paraboloid.gif "Normal form of Quadratic Surface:
         * Circular Paraboloid (special case of elliptic paraboloid)."
         *
         * @param a [in] radius of the circle when \f$ x_3=1\f$ .
         * @return a QuadraticSurface representation of a circular paraboloid.
         */

         #endif 
        static QuadraticSurface::Ptr makeCircularParaboloid (double a);

#if !defined(SWIGJAVA)
        /**
         * @brief Create a hyperbolic paraboloid.
         *
         * \image html geometry/quadrics_hyperbolic_paraboloid.gif "Normal form of Quadratic
         * Surface: Hyperbolic Paraboloid."
         *
         * @param a [in] width in the \f$ x_1\f$ direction when \f$ x_3=1\f$ .
         * @param b [in] width in the \f$ x_2\f$ direction when \f$ x_3=-1\f$ .
         * @return a QuadraticSurface representation of a hyperbolic paraboloid.
         */

         #endif 
        static QuadraticSurface::Ptr makeHyperbolicParaboloid (double a, double b);

#if !defined(SWIGJAVA)
        /**
         * @brief Create an elliptic hyperboloid of one sheet.
         *
         * \image html geometry/quadrics_elliptic_hyperboloid_onesheet.gif "Normal form of Quadratic
         * Surface: Elliptic Hyperboloid of One Sheet."
         *
         * @param a [in] radius of the ellipse in the \f$ x_1\f$ direction when \f$ x_3=0\f$ .
         * @param b [in] radius of the ellipse in the \f$ x_2\f$ direction when \f$ x_3=0\f$ .
         * @param c [in] radius is scaled with the factor \f$ \frac{1}{c}\sqrt{x_3^2+c^2}\f$ .
         * @return a QuadraticSurface representation of an elliptic hyperboloid of one sheet.
         */

         #endif 
        static QuadraticSurface::Ptr makeEllipticHyperboloidOneSheet (double a, double b, double c);

#if !defined(SWIGJAVA)
        /**
         * @brief Create a circular hyperboloid of one sheet (special case of the elliptic
         * hyperboloid of one sheet).
         *
         * \image html geometry/quadrics_circular_hyperboloid_onesheet.gif "Normal form of Quadratic
         * Surface: Circular Hyperboloid of One Sheet (special case of elliptic hyperboloid of one
         * sheet)."
         *
         * @param a [in] radius of the circle in the \f$ x_1\f$ and \f$ x_2\f$ directions when
         * \f$ x_3=0\f$ .
         * @param b [in] radius is scaled along \f$ x_3\f$ , to \f$ \frac{a}{c}\sqrt{x_3^2+c^2}\f$ .
         * @return a QuadraticSurface representation of a circular hyperboloid of one sheet.
         */

         #endif 
        static QuadraticSurface::Ptr makeCircularHyperboloidOneSheet (double a, double b);


#if !defined(SWIGJAVA)
        /**
         * @brief Create an elliptic hyperboloid of two sheets.
         *
         * \image html geometry/quadrics_elliptic_hyperboloid_twosheets.gif "Normal form of
         * Quadratic Surface: Elliptic Hyperboloid of Two Sheets."
         *
         * @param a [in] radius of the ellipse in the \f$ x_1\f$ direction when
         * \f$ x_3=\pm\sqrt{2}c\f$ .
         * @param b [in] radius of the ellipse in the \f$ x_2\f$ direction when
         * \f$ x_3=\pm\sqrt{2}c\f$ .
         * @param c [in] distance from origo to each of the the two sheets.
         * @return a QuadraticSurface representation of an elliptic hyperboloid of two sheets.
         */

         #endif 
        static QuadraticSurface::Ptr makeEllipticHyperboloidTwoSheets (double a, double b,
                                                                       double c);


#if !defined(SWIGJAVA)
        /**
         * @brief Create a circular hyperboloid of two sheets (special case of the elliptic
         * hyperboloid of two sheets).
         *
         * \image html geometry/quadrics_circular_hyperboloid_twosheets.gif "Normal form of
         * Quadratic Surface: Circular Hyperboloid of Two Sheets (special case of elliptic
         * hyperboloid of two sheets)."
         *
         * @param a [in] radius of the circle in the \f$ x_1\f$ and \f$ x_2\f$ directions when
         * \f$ x_3=\pm\sqrt{2}b\f$ .
         * @param b [in] distance from origo to each of the the two sheets.
         * @return a QuadraticSurface representation of a circular hyperboloid of two sheets.
         */

         #endif 
        static QuadraticSurface::Ptr makeCircularHyperboloidTwoSheets (double a, double b);


#if !defined(SWIGJAVA)
        /**
         * @brief Create an elliptic cone.
         *
         * The cone is a singular (or degenerate) surface as it has a point where the gradient
         * vanishes. In the origo the gradient will always be zero, and no normal can be determined.
         *
         * \image html geometry/quadrics_elliptic_cone.gif "Normal form of Quadratic Surface:
         * Elliptic Cone."
         *
         * @param a [in] radius of the ellipse in the \f$ x_1\f$ direction when \f$ x_3=c\f$ .
         * @param b [in] radius of the ellipse in the \f$ x_2\f$ direction when \f$ x_3=c\f$ .
         * @param c [in] rate of change for the radius.
         * @return a QuadraticSurface representation of an elliptic cone.
         */

         #endif 
        static QuadraticSurface::Ptr makeEllipticCone (double a, double b, double c);

#if !defined(SWIGJAVA)
        /**
         * @brief Create a circular cone (special case of the elliptic cone).
         *
         * \image html geometry/quadrics_circular_cone.gif "Normal form of Quadratic Surface:
         * Circular Cone (special case of elliptic cone)."
         *
         * @param a [in] radius of the circle in the \f$ x_1\f$ and \f$ x_2\f$ directions when
         * \f$ x_3=c\f$ .
         * @param b [in] rate of change for the radius.
         * @return a QuadraticSurface representation of a circular cone.
         */

         #endif 
        static QuadraticSurface::Ptr makeCircularCone (double a, double b);


#if !defined(SWIGJAVA)
        /**
         * @brief Create an elliptic cylinder.
         *
         * \image html geometry/quadrics_elliptic_cylinder.gif "Normal form of Quadratic Surface:
         * Elliptic Cylinder."
         *
         * @param a [in] radius in the \f$ x_1\f$ direction.
         * @param b [in] radius in the \f$ x_2\f$ direction.
         * @return a QuadraticSurface representation of an elliptic cylinder.
         */

         #endif 
        static QuadraticSurface::Ptr makeEllipticCylinder (double a, double b);


#if !defined(SWIGJAVA)
        /**
         * @brief Create a circular cylinder (special case of the elliptic cylinder).
         *
         * \image html geometry/quadrics_circular_cylinder.gif "Normal form of Quadratic Surface:
         * Circular Cylinder (special case of elliptic cylinder)."
         *
         * @param radius [in] radius in the \f$ x_1\f$ and \f$ x_2\f$ directions.
         * @param outward [in] (optional) set to false to create inner surface of cylinder, with
         * normals pointing inwards.
         * @return a QuadraticSurface representation of a circular cylinder.
         */

         #endif 
        static QuadraticSurface::Ptr makeCircularCylinder (double radius, bool outward = true);

#if !defined(SWIGJAVA)
        /**
         * @brief Create a hyperbolic cylinder.
         *
         * \image html geometry/quadrics_hyperbolic_cylinder.gif "Normal form of Quadratic Surface:
         * Hyperbolic Cylinder."
         *
         * @param a [in] width in the \f$ x_1\f$ direction at \f$ x_2=0\f$ .
         * @param b [in] controls the rate of change in the \f$ x_1\f$ direction.
         * @return a QuadraticSurface representation of a hyperbolic cylinder.
         */

         #endif 
        static QuadraticSurface::Ptr makeHyperbolicCylinder (double a, double b);

#if !defined(SWIGJAVA)
        /**
         * @brief Create a parabolic cylinder.
         *
         * \image html geometry/quadrics_parabolic_cylinder.gif "Normal form of Quadratic Surface:
         * Parabolic Cylinder."
         *
         * @param a [in] controls the focal length of the parabola.
         * @return a QuadraticSurface representation of a parabolic cylinder.
         */

         #endif 
        static QuadraticSurface::Ptr makeParabolicCylinder (double a);
#if !defined(SWIG)
///@}
#endif

        /**
         * @brief Represent a plane as a QuadraticSurface.
         *
         * A plane is a particularly simple type of quadratic surface, where
         * \f$ \mathbf{A}=\mathbf{0}\f$ .
         *
         * Even though a plane is not strictly a quadratic surface, is is often convenient to be
         * able to treat it like a quadratic surface.
         *
         * @param n [in] the normal of the plane.
         * @param d [in] the distance from the plane to the origo.
         * @return a QuadraticSurface representing a plane.
         */
        static QuadraticSurface::Ptr makePlane (const rw::math::Vector3D<double>& n, double d);

      private:
        QuadraticSurface (const Eigen::Matrix3d& A, bool diagonal, double determinantA,
                          const Eigen::Vector3d& a, double u,
                          const std::vector< TrimmingRegion >& conditions,
                          double stepsPerRevolution);
        #if !defined(SWIGJAVA)
        // From ImplicitSurface
        inline rw::geometry::ImplicitSurface::Ptr
        doTransformImplicitSurface (const rw::math::Transform3D<>& T) const
        {
            return transform (T);
        }
        inline rw::geometry::ImplicitSurface::Ptr doTransformImplicitSurface (const rw::math::Vector3D<double>& P) const
        {
            return transform (P);
        }
        inline rw::geometry::ImplicitSurface::Ptr doScaleImplicitSurface (double factor) const
        {
            return scale (factor);
        }
        inline rw::geometry::ImplicitSurface::Ptr doCloneImplicitSurface () const { return clone (); }
        #endif 
        rw::core::Ptr< TriMesh > getTriMeshDiagonal (
            const std::vector< rw::math::Vector3D<double> >& border,
            const rw::math::Rotation3D<>& R = rw::math::Rotation3D<>::identity ()) const;
        std::pair< double, double > extremumsDiagonal (const rw::math::Vector3D<double>& dir) const;
        std::vector< rw::geometry::QuadraticCurve > findSilhouette (std::size_t u, std::size_t v, std::size_t e,
                                                      double eSplit) const;
        typedef enum Place { FRONT, BACK, BOTH } Place;
        void makeSurface (const std::vector< rw::math::Vector3D<double> > fullPolygon, std::size_t u,
                          std::size_t v, std::size_t e, double eSplit, Place place,
                          const rw::math::Rotation3D<>& R,
                          rw::geometry::PlainTriMeshN1D::Ptr mesh) const;

        const Eigen::Matrix3d _A;
        const Eigen::Vector3d _a;
        const double _u;
        const double _determinantA;
        const bool _diagonal;
        std::vector< TrimmingRegion > _conditions;

        double _stepsPerRevolution;
    };
#if !defined(SWIG)
//! @}
#endif
}}    // namespace rw::geometry

#endif /* RW_GEOMETRY_ANALYTIC_QUADRATICS_QUADRATICSURFACE_HPP_ */
