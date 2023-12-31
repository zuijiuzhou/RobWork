/*
 * Covariance3D.hpp
 *
 *  Created on: 04/08/2010
 *      Author: jimali
 */

#ifndef COVARIANCE3D_HPP_
#define COVARIANCE3D_HPP_

#if !defined(SWIG)
#include <rw/geometry/Geometry.hpp>
#include <rw/geometry/IndexedTriMesh.hpp>
#include <rw/geometry/TriangleUtil.hpp>
#include <rw/math/EigenDecomposition.hpp>
#include <rw/math/LinearAlgebra.hpp>
#include <rw/math/Vector3D.hpp>

#include <vector>
#endif

namespace rw { namespace geometry {

    /**
     *  @brief class for estimating the covariance of different data
     */
    template< class T = double > class Covariance
    {
      public:
        //! @brief Default constructor.
        Covariance () {}

        /**
         * @brief Constructor.
         * @param matrix [in] Eigen matrix.
         */
        Covariance (const Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic >& matrix) :
            _covar (matrix)
        {}

        //! @brief Destructor.
        virtual ~Covariance () {}

        /**
         * @brief Get the covariance matrix.
         * @return Eigen matrix.
         */
        const Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic >& getMatrix () { return _covar; }

        /**
         * @brief Do eigen decomposition.
         * @return the EigenDecomposition.
         */
        rw::math::EigenDecomposition< T > eigenDecompose ()
        {
            typedef std::pair< Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic >,
                               Eigen::Matrix< T, Eigen::Dynamic, 1 > >
                ResultType;
            // std::cout << "Covar: " << _covar << std::endl;
            ResultType res = rw::math::LinearAlgebra::eigenDecompositionSymmetric (_covar);
            // std::cout << "res: " <<  res.first << std::endl;
            // std::cout << "res: " <<  res.second << std::endl;
            return rw::math::EigenDecomposition< T > (res.first, res.second);
        }

        /**
         * @brief initialize covariance using a geometry object.
         * @param geom [in] the geometry.
         */
        void initialize (rw::geometry::Geometry& geom)
        {
            using namespace rw::geometry;
            GeometryData::Ptr data = geom.getGeometryData ();
            TriMesh::Ptr mesh      = data->getTriMesh (false);

            if (dynamic_cast< rw::geometry::IndexedTriMesh< T >* > (mesh.get ())) {
                rw::geometry::IndexedTriMesh< T >* imesh = dynamic_cast< rw::geometry::IndexedTriMesh< T >* > (mesh.get ());
                initialize (imesh->getVertices ());
            }
            else {
                rw::core::Ptr< rw::geometry::IndexedTriMeshN0< T > > ipmesh =
                    TriangleUtil::toIndexedTriMesh< rw::geometry::IndexedTriMeshN0< T > > (*mesh);
                initialize (ipmesh->getVertices ());
            }
        }

        /**
         * @brief Initialize covariance from points.
         * @param points [in] the points.
         */
        void initialize (const std::vector< rw::math::Vector3D< T > >& points)
        {
            doInitialize< typename std::vector< rw::math::Vector3D< T > >::const_iterator, 3 > (
                points.begin (), points.end ());
        }

        /**
         * @brief Initialization method.
         * @param first [in] iterator for first element.
         * @param last [in] iterator for last element.
         */
        template< class RandomAccessIterator, int DIM >
        void doInitialize (RandomAccessIterator first, RandomAccessIterator last)
        {
            using namespace rw::math;

            // const size_t nrOfPoints = points.size();

            _covar = Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic >::Zero (DIM, DIM);
            Eigen::Matrix< T, Eigen::Dynamic, 1 > centroid =
                Eigen::Matrix< T, Eigen::Dynamic, 1 >::Zero (DIM);

            // calculate centroid
            size_t nrOfPoints              = 0;
            RandomAccessIterator first_tmp = first;
            for (; first_tmp != last; ++first_tmp) {
                nrOfPoints++;
                for (size_t j = 0; j < DIM; j++)
                    centroid[j] += (*first_tmp)[j];
            }

            // scale according to nr points
            centroid = centroid * ((T) 1.0 / nrOfPoints);

            // next we compute the covariance elements
            Eigen::VectorXd p = Eigen::VectorXd::Zero (DIM);
            for (; first != last; ++first) {
                for (size_t k = 0; k < DIM; k++) {
                    p[k] = (*first)[k] - centroid[k];
                }

                for (size_t j = 0; j < DIM; j++) {
                    for (size_t k = j; k < DIM; k++) {
                        _covar (k, j) += (*first)[k] * (*first)[j];
                    }
                }
            }

            // fill in all covariance elements
            for (size_t j = 1; j < DIM; j++)
                for (size_t k = 0; k < j; k++)
                    _covar (k, j) = _covar (j, k);
        }

        // template<class POINT_LIST, class WEIGHT_LIST>
        // Covariance3D<T> doInitialize(const std::vector<Vector3D<T> >& points, const
        // std::vector<double>& weights);

      private:
        Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > _covar;
    };
#if !defined(SWIG)
    extern template class rw::geometry::Covariance< double >;
    extern template class rw::geometry::Covariance< float >;
#else
#if SWIG_VERSION < 0x040000
    SWIG_DECLARE_TEMPLATE (Covariance_d, rw::geometry::Covariance< double >);
    ADD_DEFINITION (Covariance_d, Covariance,sdurw_geometry)
#else
    SWIG_DECLARE_TEMPLATE (Covariance, rw::geometry::Covariance< double >);
#endif
#endif
}}    // namespace rw::geometry

#endif /* COVARIANCE3D_HPP_ */
