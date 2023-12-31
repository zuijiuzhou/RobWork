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

#ifndef RW_GRAPHICS_DRAWABLENODE_HPP
#define RW_GRAPHICS_DRAWABLENODE_HPP

/**
 * @file DrawableNode.hpp
 */

#if !defined(SWIG)
#include <rw/graphics/SceneNode.hpp>

#include <rw/core/Ptr.hpp>
#include <rw/math/Transform3D.hpp>

#endif

namespace rw { namespace kinematics {
    class State;
}}    // namespace rw::kinematics
namespace rw { namespace graphics {
    class SceneCamera;
}}    // namespace rw::graphics

namespace rw { namespace graphics {

    //! @addtogroup graphics
    // @{

    /**
     * @brief Abstract base class for all drawable classes
     *
     * Classes that are able to draw them self, may inherit from this class.
     *
     * The drawable class use a draw mask to distinguish between different
     * groups to draw. E.g. when taking snapshots with a simulated camera
     * virtual objects such as the red laser vector or the lines showing
     * the camera view angle is should not be rendered. Hence objects that
     * are virtual should be set to virtual.
     *
     * A call to draw enabling Physical and User1 defined objects look like:
     * \code
     * drawable->draw(DrawableNode::Physical | DrawableNode::User1);
     * \endcode
     */
    class DrawableNode : public SceneNode
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< DrawableNode > Ptr;

        /**
         * @brief draw mask is used to filter which drawables to exclude from rendering.
         */
        typedef enum {
            Physical        = 1,       //! A physical object in the scene
            Virtual         = 2,       //! A virtual object, e.g. lines showing camera view angle
            DrawableObject  = 4,       //! An object that is "just" a drawable
            CollisionObject = 8,       //! An object that is also a CollisionObject
            User1           = 1024,    //! User defined group 1...
            User2           = 2048,    //!< User2
            User3           = 4096,    //!< User3
            User4           = 8096,    //!< User4
            ALL             = 0x7FFFFFFF
        } DrawableTypeMask;

        /**
         * @brief when calling render on the draw mode or type
         * can be specified.
         */
        enum DrawType {
            //! Render in solid
            SOLID,  
            //! Render in wireframe   
            WIRE, 
            //! Render both solid and wireframe     
            OUTLINE    
        };

        //! @brief Information for rendering.
        struct RenderInfo
        {
            /**
             * @brief Construct new rendering information.
             * @param mask [in] (optional) the draw type mask. Default is DrawableObject.
             */
            RenderInfo (unsigned int mask = DrawableNode::DrawableObject) :
                _mask (mask), _drawType (SOLID), _state (NULL), _renderTransparent (false),
                _renderSolid (false), _disableNormalRender (false), _cam (NULL)
            {}

            //! @brief The DrawableTypeMask.
            unsigned int _mask;
            //! @brief The DrawType.
            DrawType _drawType;
            //! @brief Pointer to the state.
            rw::kinematics::State* _state;
            //! @brief Render transparently.
            bool _renderTransparent;
            //! @brief Render as a solid.
            bool _renderSolid;
            //! @brief Disabling rendering of normals.
            bool _disableNormalRender;
            //! @brief Rendering camera
            rw::core::Ptr< SceneCamera > _cam;
            //! @brief Transform  World to model
            rw::math::Transform3D<> _wTm;
        };

        //! @brief Destructor.
        virtual ~DrawableNode () {}

        /**
         * @brief draws the object.
         */
        virtual void draw (const DrawableNode::RenderInfo& info = RenderInfo ()) const = 0;

        /**
         * @brief enables or disables highlighting of the drawable class
         *
         * @param b [in] a if true highlight is enabled if false disabled
         */
        virtual void setHighlighted (bool b) = 0;

        /**
         * @brief Returns whether the DrawableNode is highlighted
         *
         * @return true/false
         */
        virtual bool isHighlighted () const = 0;

        /**
         * @brief Sets the DrawType
         *
         * @param drawType [in] the DrawType to be used
         */
        virtual void setDrawType (DrawType drawType) = 0;

        /**
         * @brief Sets up the color alpha value.
         *
         * @param alpha [in] \f$ 0.0 \f$ corresponds to fully transparent and
         * \f$ 1.0\f$ to completely solid.
         */
        virtual void setTransparency (float alpha) = 0;

        /**
         * @brief Gets the color alpha value.
         * @return alpha value in the interval \f$ [0.0;1.0] \f$
         */
        virtual float getTransparency () = 0;

        /**
         * @brief Check if node is transparent.
         * @return true if transparent, false otherwise.
         */
        bool isTransparent () { return getTransparency () < (1.0 - 0.00001); }

        /**
         * @brief Specifies the scale of the object
         * @param scale [in] the scale
         */
        virtual void setScale (float scale) = 0;

        /**
         * @brief gets the scale of the object
         * @return scale [in] the scale
         */
        virtual float getScale () const = 0;

        /**
         * @brief enable or disable this drawable. When disabled the drawable
         * will not render anything.
         */
        virtual void setVisible (bool enable) = 0;

        /**
         * @brief checks if this drawable is enabled
         */
        virtual bool isVisible () = 0;

        /**
         * @brief gets the transformation of the drawable object
         * @return transform of the drawable object
         */
        virtual const rw::math::Transform3D<>& getTransform () const = 0;

        /**
         * @brief Sets the transformation of the drawable object
         * @param t3d [in] transform of drawable object
         */
        virtual void setTransform (const rw::math::Transform3D<>& t3d) = 0;

        /**
         * @brief the group(s) that this drawable belong to
         * @param mask [in] drawable mask
         */
        virtual void setMask (unsigned int mask) = 0;

        /**
         * @brief Get the DrawableTypeMask for the node.
         * @return the type mask.
         */
        virtual unsigned int getMask () const = 0;

        //! @copydoc SceneNode::asDrawableNode
        DrawableNode* asDrawableNode () { return this; }

      protected:
        /**
         * @brief constructor
         */
        DrawableNode (const std::string& name) : SceneNode (name, SceneNode::DrawableType) {}

      private:
        DrawableNode ();
        DrawableNode (const DrawableNode&);
        DrawableNode& operator= (const DrawableNode&);
    };

    /*@}*/
}}    // namespace rw::graphics

#endif    // end include guard
