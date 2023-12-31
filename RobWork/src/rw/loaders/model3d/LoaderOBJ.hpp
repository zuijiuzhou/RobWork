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

#ifndef RW_GRAPHICS_LOADEROBJ_
#define RW_GRAPHICS_LOADEROBJ_

//! @file LoaderOBJ.hpp

#include <rw/loaders/Model3DLoader.hpp>

#include <string>

namespace rw { namespace loaders {

    //! @addtogroup graphics
    // @{

    /**
     * @brief Class for loading in IVG files.
     * TODO: add documentation on IVG format
     */
    class LoaderOBJ : public Model3DLoader
    {
      public:
        /**
         * @brief constructor
         */
        LoaderOBJ (){};

        /**
         * @brief destructor
         */
        virtual ~LoaderOBJ (){};

        //! @copydoc Model3DLoader::load
        rw::graphics::Model3D::Ptr load (const std::string& filename);

        //! @copydoc Model3DLoader::getModelFormats
        std::vector< std::string > getModelFormats () {return {".OBJ"};}
    };

    //! @}

}}    // namespace rw::loaders

#endif    // end include guard
