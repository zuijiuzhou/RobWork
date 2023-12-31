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

#ifndef RW_GRAPHICS_MODEL3D_HPP_
#define RW_GRAPHICS_MODEL3D_HPP_

//! @file Model3D.hpp
#if !defined(SWIG)
#include <rw/graphics/TextureData.hpp>

#include <rw/geometry/Model3D.hpp>

#include <vector>

#endif

namespace rw { namespace graphics {
    using rw::geometry::Model3D;
    using Model3DTextureType = TextureData;
}}    // namespace rw::graphics

#endif