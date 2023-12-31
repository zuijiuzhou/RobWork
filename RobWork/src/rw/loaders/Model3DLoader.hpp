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

#ifndef RW_GRAPHICS_MODEL3DLOADER_HPP_
#define RW_GRAPHICS_MODEL3DLOADER_HPP_

//! @file Model3DLoader.hpp
#if !defined(SWIG)
#include <rw/common/FileCache.hpp>
#include <rw/core/ExtensionPoint.hpp>
#include <rw/graphics/Model3D.hpp>
#endif

namespace rw { namespace loaders {
    /** @addtogroup loaders */
    /*@{*/

    /**
     * @brief interface for classes that are able to load 3d models
     */
    class Model3DLoader
    {
      public:
        //! smart pointer type
        typedef rw::core::Ptr< Model3DLoader > Ptr;

        //! destructor
        virtual ~Model3DLoader (){};
        /**
         * @brief load a Model3D from file \b filename
         * @param filename [in] name of file to load
         * @return a model3d if loaded successfully else NULL (or exception)
         */
        virtual rw::graphics::Model3D::Ptr load (const std::string& filename) = 0;

        // virtual void save(Model3DPtr model, const std::string& filename) = 0;

        /**
         * @brief get the list of supported 3D model formats (as extensions)
         * @return
         */
        virtual std::vector< std::string > getModelFormats () = 0;

        /**
         * @addtogroup extensionpoints
         * @extensionpoint{rw::loaders::Model3DLoader::Factory,rw::loaders::Model3DLoader,rw.loaders.Model3DLoader}
         */

        /**
         * @brief a factory for Model3DLoaders. This factory defines an
         * extension point for Model3DLoaders.
         */
        class Factory : public rw::core::ExtensionPoint< Model3DLoader >
        {
          public:
            //! constructor
            Factory () :
                rw::core::ExtensionPoint< Model3DLoader > ("rw.loaders.Model3DLoader",
                                                           "Example extension point"){};

            /**
             * get loader for a specific file format (extension)
             * @param format [in] extension of file
             * @return
             */
            static rw::core::Ptr< Model3DLoader > getModel3DLoader (const std::string& format);

            /**
             * test if a loader exist for a specific file format (extension)
             * @param format [in] extension of file
             * @return
             */
            static bool hasModel3DLoader (const std::string& format);

            /**
             * @brief get a list of supported formats
             * @return
             */
            static std::vector< std::string > getSupportedFormats ();

            ///// FOR BACKWARDS COMPATIBILITY WITH Model3DFactory

            /**
             * @brief Factory method for constructing a Drawable based on
             * a string.
             *
             * The method probes the string to see if it describes a geometric
             * primitive or a file name. In case of a geometric primitive it
             * forwards to call to constructFromGeometry.
             * Otherwise it calls loadModel
             * otherwise
             */
            // static rw::graphics::Model3D::Ptr getModel(const std::string& str, const std::string&
            // name);

            /**
             * @brief Factory method constructing a Drawable from a file.
             * @param filename [in] path and name of file to load
             * @param name [in] the id/name of the drawable
             * @return drawable
             *
             * The factory determines which type of Drawable to used
             * based on the filename extension. In case no extension
             * exists if test whether a file with the same name or a .stl, .stla, .stlb,
             * .3ds, .ac or .ac3d exists.
             *
             * An exception is thrown if the file can't be loaded.
             */
            // static rw::graphics::Model3D::Ptr loadModel(const std::string &filename, const
            // std::string& name);

            /**
             * @brief Factory method constructing a Drawable based on
             * a Geometry ID string.
             *
             * The method constructs a Drawable representing the geometry
             * described in the string
             *
             * An exception is thrown if the string cannot be parsed correctly.
             *
             * @param str [in] Geometry ID string
             * @param name [in] the id/name of the drawable
             * @param useCache [in] True to use caching. Default false
             * @return Point to drawable object
             */
            // static rw::graphics::Model3D::Ptr constructFromGeometry(const std::string& str, const
            // std::string& name, bool useCache=false);

          private:
            // typedef rw::common::FileCache<std::string, rw::graphics::Model3D, std::string>
            // FactoryCache; static FactoryCache& getCache();
        };
    };

    // typedef Model3DLoader::Factory Model3DFactory;

    //! @}

}}     // namespace rw::loaders
#endif /* RW_GRAPHICS_MODEL3DLOADER_HPP_ */
