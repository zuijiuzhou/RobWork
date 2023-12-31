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

#ifndef RW_GEOMETRY_OBB_HPP_
#define RW_GEOMETRY_OBB_HPP_

#if !defined(SWIG)
#include <rw/geometry/BV.hpp>
#include <rw/geometry/PlainTriMesh.hpp>
#include <rw/geometry/TriMesh.hpp>
#include <rw/geometry/Triangle.hpp>
#include <rw/math/LinearAlgebra.hpp>
#include <rw/math/Rotation3D.hpp>
#include <rw/math/Transform3D.hpp>
#include <rw/math/Vector3D.hpp>
#endif

#include <rw/common/Traits.hpp>

namespace rw {
namespace geometry {

    /**
     * @brief class representing an Oriented Bounding Box (OBB)
     */

#if !defined(SWIGJAVA)
    template< class T = double > class OBB : public OBV< OBB< T > >
#else
    template< class T = double > class OBB
#endif
    {
      private:
        rw::math::Transform3D< T > _t3d;
        rw::math::Vector3D< T > _halfLng;

      public:
        typedef T value_type;

        /**
         * @brief constructor
         */
        OBB () :
            _t3d (rw::math::Transform3D< T >::identity ()),
            _halfLng (rw::math::Vector3D< T > (0, 0, 0))

        {}

        OBB (rw::math::Transform3D< T > t3d, rw::math::Vector3D< T > halfLng) :
            _t3d (t3d), _halfLng (halfLng)
        {}

        const rw::math::Transform3D< T >& getTransform () const { return _t3d; }

        //! @brief set the transformation of this OBB
        inline void setTransform (rw::math::Transform3D< T > t3d) { _t3d = t3d; }

        //! @brief get the halflengths of this OBB
        inline const rw::math::Vector3D< T >& getHalfLengths () const { return _halfLng; }

        //! @brief set the halflengths of the OBB
        inline void setHalfLengths (const rw::math::Vector3D< T >& halfLng) { _halfLng = halfLng; }

        //! @brief calculate the volume of this OBB
        T calcVolume () const { return _halfLng (0) * 2 * _halfLng (1) * 2 * _halfLng (2) * 2; }

        //! @brief calculates the total area of the box
        T calcArea () const
        {
            const T& h = _halfLng (0);
            const T& w = _halfLng (1);
            const T& l = _halfLng (2);
            return 2 * (h * 2 * w * 2) + 2 * (h * 2 * l * 2) + 2 * (w * 2 * l * 2);
        }
#if !defined(SWIG)
        /**
         * @brief Ouputs OBB to stream
         * @param os [in/out] stream to use
         * @param obb [in] oriented bounding box
         * @return the resulting stream
         */
        friend std::ostream& operator<< (std::ostream& os, const OBB< T >& obb)
        {
            return os << " OBB { \n"
                      << obb._halfLng << "\n\t" << obb._t3d.P () << "\n\t" << obb._t3d.R ()
                      << "\n}";
            // return os << eaa._eaa;
        }
#else
        TOSTRING (rw::geometry::OBB< T >);
#endif
        static OBB< T > buildTightOBB (const rw::geometry::TriMesh& tris, size_t index = 0)
        {
            using namespace rw::math;
            using namespace rw::geometry;
            // 1. Compute convex hull
            // std::cout << "build hull from: " << tris.size() <<  std::endl;
            const int nrOfTris = (int) tris.getSize ();

            // 2. Compute centroid for convex hull
            // 2.1 centroid is computed using the triangles of the convex hull
            Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > covar (
                Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic >::Zero (3, 3));
            Vector3D< T > centroid (0, 0, 0);
            // we only use triangle centers the vertices directly
            for (int i = 0; i < nrOfTris; i++) {
                // std::cout << "i" << i << std::endl;
                // calc triangle centroid
                rw::geometry::Triangle< double > t = tris.getTriangle (i);
                centroid += cast< T > (t[0] + t[1] + t[2]);
                for (size_t j = 0; j < 3; j++)
                    for (size_t k = 0; k < 3; k++)
                        covar (j, k) +=
                            (T) (t[0](j) * t[0](k) + t[1](j) * t[1](k) + t[2](j) * t[2](k));
            }
            // std::cout << "COVAR: " << covar << std::endl;
            int n = nrOfTris * 3;
            // centroid = centroid/n;
            // 3. Compute Covariance matrix
            // 3.1 using the variables from 2.1 we create the covariance matrix
            for (size_t j = 0; j < 3; j++)
                for (size_t k = 0; k < 3; k++)
                    covar (j, k) = covar (j, k) - centroid[j] * centroid[k] / n;
            // 4. get eigenvectors from the covariance matrix
            typedef std::pair< Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic >,
                               Eigen::Matrix< T, Eigen::Dynamic, 1 > >
                ResultType;
            // std::cout << "COVAR: " << covar << std::endl;
            ResultType res = LinearAlgebra::eigenDecompositionSymmetric (covar);
            // 4.1 create the rotationmatrix from the normalized eigenvectors
            // find max and the second maximal eigenvalue
            size_t maxEigIdx = 2, midEigIdx = 1, minEigIdx = 0;
            T maxEigVal = res.second (maxEigIdx);
            T midEigVal = res.second (midEigIdx);
            T minEigVal = res.second (minEigIdx);
            if (maxEigVal < midEigVal) {
                std::swap (midEigVal, maxEigVal);
                std::swap (midEigIdx, maxEigIdx);
            }
            if (minEigVal > midEigVal) {
                std::swap (midEigVal, minEigVal);
                std::swap (midEigIdx, minEigIdx);
                if (midEigVal > maxEigVal) {
                    std::swap (midEigVal, maxEigVal);
                    std::swap (midEigIdx, maxEigIdx);
                }
            }
            // std::cout << "EIGEN values: " << maxEigVal << " " << minEigVal << std::endl;
            // std::cout << "EIGEN values: " << res.second(0) << " " << res.second(1) <<" "  <<
            // res.second(2) << std::endl;
            // specify x and y axis, x will be the axis with largest spred
            Vector3D< T > maxAxis (
                res.first (0, maxEigIdx), res.first (1, maxEigIdx), res.first (2, maxEigIdx));
            Vector3D< T > midAxis (
                res.first (0, midEigIdx), res.first (1, midEigIdx), res.first (2, midEigIdx));
            // Vector3D<T> maxAxis( res.first(maxEigIdx,0), res.first(maxEigIdx,1),
            // res.first(maxEigIdx,2) ); Vector3D<T> midAxis( res.first(midEigIdx,0),
            // res.first(midEigIdx,1), res.first(midEigIdx,2) );

            // compute the z axis as the cross product
            Vector3D< T > crossAxis = cross (maxAxis, midAxis);
            // generate the rotation matrix
            // std::cout << "Max Axis: " << maxAxis << std::endl;
            // std::cout << "Mid Axis: " << midAxis << std::endl;
            // std::cout << "cross Axis: " << crossAxis << std::endl;
            Rotation3D< T > rot (normalize (maxAxis), normalize (midAxis), normalize (crossAxis));
            // 5. find extreme vertices relative to eigenvectors
            // we use the generated rotation matrix to rotate each point and then save
            // max and min values of each axis
            Rotation3D< T > rotInv             = inverse (rot);
            rw::geometry::Triangle< double > t = tris.getTriangle (0);
            Vector3D< T > p                    = rotInv * cast< T > (t[0]);
            Vector3D< T > max = p, min = p;
            for (int i = 0; i < nrOfTris; i++) {
                rw::geometry::Triangle< double > tri = tris.getTriangle (i);
                for (int pidx = 0; pidx < 3; pidx++) {
                    Vector3D< T > p = rotInv * cast< T > (tri[pidx]);
                    for (int j = 0; j < 3; j++) {
                        if (p (j) > max (j))
                            max (j) = p (j);
                        else if (p (j) < min (j))
                            min (j) = p (j);
                    }
                }
            }
            // 6. use them to generate OBB
            // std::cout << "Max-Min: " << (max-min) << std::endl;
            // compute halflength of box and its midpoint
            Vector3D< T > midPoint   = rot * (0.5 * (max + min));
            Vector3D< T > halfLength = 0.5 * (max - min);
            // std::cout << "halflength: " << halfLength << std::endl;
            // std::cout << "midpoint: " << midPoint << std::endl;
            Transform3D< T > trans (midPoint, rot);
            // std::cout << "Trans mid: " << trans.P() << std::endl;
            return OBB< T > (trans, halfLength);
        }

        /**
         * @brief Creates a TriMesh representing the OBB.
         *
         * The triangles of the mesh is by default placed in the "global" frame corresponding to the
         * frame in which the OBB is defined. In case the parameter \b local is set to true, the
         * reference frame will be the frame of the OBB itself.
         */
        TriMesh::Ptr createMesh (bool local = false) const
        {
            typename PlainTriMesh< rw::geometry::Triangle< T > >::Ptr mesh =
                rw::core::ownedPtr (new PlainTriMesh< rw::geometry::Triangle< T > > (12));

            T x = _halfLng (0);
            T y = _halfLng (1);
            T z = _halfLng (2);

            rw::math::Vector3D< T > p1 (x, y, z);
            rw::math::Vector3D< T > p2 (x, y, -z);
            rw::math::Vector3D< T > p3 (-x, y, -z);
            rw::math::Vector3D< T > p4 (-x, y, z);

            rw::math::Vector3D< T > p5 (x, -y, z);
            rw::math::Vector3D< T > p6 (x, -y, -z);
            rw::math::Vector3D< T > p7 (-x, -y, -z);
            rw::math::Vector3D< T > p8 (-x, -y, z);
            if (local == false) {
                p1 = _t3d * p1;
                p2 = _t3d * p2;
                p3 = _t3d * p3;
                p4 = _t3d * p4;
                p5 = _t3d * p5;
                p6 = _t3d * p6;
                p7 = _t3d * p7;
                p8 = _t3d * p8;
            }

            (*mesh)[0] = rw::geometry::Triangle< T > (p1, p2, p3);
            (*mesh)[1] = rw::geometry::Triangle< T > (p3, p4, p1);

            (*mesh)[2] = rw::geometry::Triangle< T > (p1, p5, p6);
            (*mesh)[3] = rw::geometry::Triangle< T > (p6, p2, p1);
            //_faces.push_back(Face<float>(p1, p5, p6));
            //_faces.push_back(Face<float>(p6, p2, p1));

            (*mesh)[4] = rw::geometry::Triangle< T > (p3, p2, p6);
            (*mesh)[5] = rw::geometry::Triangle< T > (p6, p7, p3);
            //_faces.push_back(Face<float>(p3, p2, p6));
            //_faces.push_back(Face<float>(p6, p7, p3));

            (*mesh)[6] = rw::geometry::Triangle< T > (p5, p8, p7);
            (*mesh)[7] = rw::geometry::Triangle< T > (p7, p6, p5);
            //_faces.push_back(Face<float>(p5, p8, p7));
            //_faces.push_back(Face<float>(p7, p6, p5));

            (*mesh)[8] = rw::geometry::Triangle< T > (p1, p4, p8);
            (*mesh)[9] = rw::geometry::Triangle< T > (p8, p5, p1);
            //_faces.push_back(Face<float>(p1, p4, p8));
            //_faces.push_back(Face<float>(p8, p5, p1));

            (*mesh)[10] = rw::geometry::Triangle< T > (p4, p3, p7);
            (*mesh)[11] = rw::geometry::Triangle< T > (p7, p8, p4);
            //_faces.push_back(Face<float>(p4, p3, p7));
            //_faces.push_back(Face<float>(p7, p8, p4));
            return mesh;
        }
    };
}    // namespace geometry

//! define traits of the OBB
template< typename T > struct Traits< rw::geometry::OBB< T > >
{
    typedef T value_type;
};

}    // namespace rw

#if !defined(SWIG)
extern template class rw::geometry::OBV< rw::geometry::OBB< double > >;
extern template class rw::geometry::OBB< double >;
extern template class rw::geometry::OBV< rw::geometry::OBB< float > >;
extern template class rw::geometry::OBB< float >;
#else
SWIG_DECLARE_TEMPLATE (ObvOBB, rw::geometry::OBV< rw::geometry::OBB< double > >);
#if SWIG_VERSION < 0x040000
SWIG_DECLARE_TEMPLATE (OBB_d, rw::geometry::OBB< double >);
ADD_DEFINITION (OBB_d, OBB, sdurw_geometry)
#else
SWIG_DECLARE_TEMPLATE (OBB, rw::geometry::OBB< double >);
#endif
SWIG_DECLARE_TEMPLATE (ObvOBB_f, rw::geometry::OBV< rw::geometry::OBB< float > >);
SWIG_DECLARE_TEMPLATE (OBB_f, rw::geometry::OBB< float >);
#endif

#endif /*OBB_HPP_*/
