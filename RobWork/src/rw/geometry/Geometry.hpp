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

#ifndef RW_GEOMETRY_GEOMETRY_HPP_
#define RW_GEOMETRY_GEOMETRY_HPP_
#if !defined(SWIG)
#include "GeometryData.hpp"

#include <rw/core/Ptr.hpp>
#include <rw/math/Transform3D.hpp>
#endif
namespace rw { namespace kinematics {
    class Frame;
}}    // namespace rw::kinematics

namespace rw { namespace geometry {
    //! @addtogroup geometry
    // @{

    /**
     * @brief a class for representing a geometry that is scaled
     * and transformed, and which is attached to a frame.
     *
     * Each geometry must have a unique ID. This is either auto
     * generated or specified by user. The ids are used in collision
     * detection and other algorithms where the object need an association
     * other than its memory address.
     */
    class Geometry
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< Geometry > Ptr;
        //! @brief smart pointer type to this const class
        typedef rw::core::Ptr< const Geometry > CPtr;

        /**
         * @brief A geometry may belong to a specific group of geometries. These groups
         * are used for fast exclude filtering of geometry data in collision detection,
         * visualization and such. There are 4 predefined groups used by RobWork.
         *
         */
        typedef enum {
            PhysicalGroup  = 1,       //! A physical object in the scene
            VirtualGroup   = 2,       //! A virtual object, e.g. lines showing camera view angle
            DrawableGroup  = 4,       //! An object that is "just" a drawable
            CollisionGroup = 8,       //! An object that is also a CollisionObject
            User1Group     = 1024,    //! User defined group 1...
            User2Group     = 2048,    //!< User2
            User3Group     = 4096,    //!< User3
            User4Group     = 8096,    //!< User4
            ALL            = 0xFFFFFFFF
        } GeometryGroupMask;

        /**
         * @brief constructor - autogenerated id from geometry type.
         * @param data
         * @param scale
         */
        Geometry (GeometryData::Ptr data, double scale = 1.0);

        /**
         * @brief constructor giving a specified id.
         * @param data [in] pointer to geometry data
         * @param name [in] Unique name to be assigned for the geometry
         * @param scale [in] scaling factor
         */
        Geometry (GeometryData::Ptr data, const std::string& name, double scale = 1.00);

        /**
         * @brief constructor - autogenerated id from geometry type.
         * @param data [in] pointer to geometry data
         * @param t3d [in] transform
         * @param scale [in] scaling factor
         */
        Geometry (GeometryData::Ptr data, const rw::math::Transform3D<>& t3d, double scale = 1.0);

        //! @brief destructor
        virtual ~Geometry ();

        /**
         * @brief gets the scaling factor applied when using this geometry
         * @return the scale as double
         */
        double getScale () const { return _scale; }

        /**
         * @brief set the scaling factor that should be applied to
         * this geometry when used.
         * @param scale [in] scale factor
         */
        void setScale (double scale) { _scale = scale; }

        /**
         * @brief set transformation
         * @param t2d [in] the new transform
         */
        void setTransform (const rw::math::Transform3D<>& t3d) { _transform = t3d; }

        /**
         * @brief get transformation
         * @return the Current transform
         */
        const rw::math::Transform3D<>& getTransform () const { return _transform; }

        /**
         * @brief get geometry data
         * @return the geometry data stored
         */
        GeometryData::Ptr getGeometryData () { return _data; }

        /**
         * @brief get geometry data
         */
        const GeometryData::Ptr getGeometryData () const { return _data; }

        /**
         * @brief set transformation
         * @param data [in] the new geometry data
         */
        void setGeometryData (GeometryData::Ptr data) { _data = data; }

        /**
         * @brief get name of this geometry
         * @return name as string
         */
        const std::string& getName () const { return _name; }

        /**
         * @brief get file path of this geometry
         * @return the file path as string
         */
        const std::string& getFilePath () const { return _filePath; }

        /**
         * @brief get identifier of this geometry
         * @return the id of the geometry
         */
        const std::string& getId () const { return getName (); }

        /**
         * @brief set name of this geometry
         * @param name [in] the new name of the geometry
         */
        void setName (const std::string& name) { _name = name; }

        /**
         * @brief set file path this geometry
         * @param name [in] path to a geometry file
         */
        void setFilePath (const std::string& name) { _filePath = name; }

        /**
         * @brief set identifier of this geometry
         * @param id [in] new id
         */
        void setId (const std::string& id) { setName (id); }

        /**
         * @brief set the color of the geometry
         * @param red [in] the amount of red color 0-255
         * @param green [in] the amount of green color 0-255
         * @param blue [in] the amount of red color 0-255
         */
        void setColor (unsigned char red, unsigned char green, unsigned char blue)
        {
            _colorRGB[0] = red / 255.0f;
            _colorRGB[1] = green / 255.0f;
            _colorRGB[2] = blue / 255.0f;
        }

        /**
         * @brief set the color of the geometry
         * @param red [in] the amount of red color 0-1
         * @param green [in] the amount of green color 0-1
         * @param blue [in] the amount of red color 0-1
         */
        void setColor (float red, float green, float blue);
        /**
         * @brief Set the reference frame.
         * @param frame [in] new reference frame.
         */
        void setFrame (kinematics::Frame* frame) { _refFrame = frame; }

        /**
         * @brief Get the reference frame.
         * @return the reference frame.
         */
        rw::kinematics::Frame* getFrame () { return _refFrame; }

        //! @copydoc getFrame()
        const rw::kinematics::Frame* getFrame () const { return _refFrame; }

        /**
         * @brief Set the draw mask.
         * @param mask [in] the draw mask.
         */
        void setMask (int mask) { _mask = mask; }

        /**
         * @brief Get the draw mask.
         * @return the draw mask.
         */
        int getMask () { return _mask; }

        /**
         * @brief util function for creating a Sphere geometry
         */
        static Geometry::Ptr makeSphere (double radi);
        /**
         * @brief util function for creating a Box geometry
         */
        static Geometry::Ptr makeBox (double x, double y, double z);
        /**
         * @brief util function for creating a Cone geometry
         */
        static Geometry::Ptr makeCone (double height, double radiusTop, double radiusBot);
        /**
         * @brief util function for creating a Cylinder geometry
         */
        static Geometry::Ptr makeCylinder (float radius, float height);

        /**
         * @brief Construct a grid.
         * @param dim_x [in] number of cells in first direction.
         * @param dim_y [in] number of cells in second direction.
         * @param size_x [in] size of one cell.
         * @param size_y [in] size of one cell.
         * @param xdir [in] the direction of the first dimension.
         * @param ydir [in] the direction of the second dimension.
         * @return a new grid geometry.
         */
        static Geometry::Ptr
        makeGrid (int dim_x, int dim_y, double size_x = 1.0, double size_y = 1.0,
                  const rw::math::Vector3D<double>& xdir = rw::math::Vector3D<double>::x (),
                  const rw::math::Vector3D<double>& ydir = rw::math::Vector3D<double>::y ());

        /**
         * @brief get the color stored for the object
         * @param color [out] the array to store the color in
         */
        void getColor (float color[3])
        {
            color[0] = _colorRGB[0];
            color[1] = _colorRGB[1];
            color[2] = _colorRGB[2];
        }

      private:
        rw::kinematics::Frame* _refFrame;
        GeometryData::Ptr _data;
        // GeometryData *_bv;
        rw::math::Transform3D<> _transform;
        double _scale;
        std::string _name;
        std::string _filePath;
        int _mask;
        float _colorRGB[3];
    };
    //! @}
}}    // namespace rw::geometry

#endif /* GEOMETRY_HPP_ */
