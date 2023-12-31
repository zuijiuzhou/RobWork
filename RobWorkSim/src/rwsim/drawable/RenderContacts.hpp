#ifndef RWLIBS_DRAWABLE_RENDERCONTACTS_HPP_
#define RWLIBS_DRAWABLE_RENDERCONTACTS_HPP_

//! @file drawable/RenderContacts.hpp

#include <rw/graphics/Render.hpp>
#include <rwlibs/opengl/rwgl.hpp>

namespace rw { namespace sensor {
    class Contact3D;
}}    // namespace rw::sensor

namespace rwsim { namespace drawable {
    //! @addtogroup rwsim_drawable
    //! @{

    /**
     * @brief renderer for contacts
     */
    class RenderContacts : public rw::graphics::Render
    {
      public:
        /**
         * @brief constructor
         */
        RenderContacts ();

        /**
         * @brief destructor
         */
        virtual ~RenderContacts ();

        /**
         * @brief add a contact to render
         */
        void addContact (const rw::sensor::Contact3D& contacts);

        /**
         * @brief add a number of contacts to render
         */
        void addContacts (const std::vector< rw::sensor::Contact3D >& contacts);

        /**
         * @brief set the contacts to render
         */
        void setContacts (const std::vector< rw::sensor::Contact3D >& contacts);

        /**
         * @brief set the color used for the model
         * @param r [in] red color value
         * @param g [in] green color value
         * @param b [in] blue color value
         */
        void setColor (double r, double g, double b);

        //! @copydoc rw::graphics::Render::draw(const DrawableNode::RenderInfo& info,
        //! DrawableNode::DrawType type, double alpha) const
        virtual void draw (const rw::graphics::DrawableNode::RenderInfo& info, DrawType type,
                           double alpha) const;

      private:
        std::vector< rw::sensor::Contact3D > _contacts;
        struct GLData;
        const GLData* const _gl;
    };
    //! @}
}}     // namespace rwsim::drawable
#endif /*DRAWABLECONTACT_HPP_*/
