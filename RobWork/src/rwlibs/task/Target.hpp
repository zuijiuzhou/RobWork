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

#ifndef RWLIBS_TASK_TARGET_HPP
#define RWLIBS_TASK_TARGET_HPP

#include <rwlibs/task/Entity.hpp>
#include <rwlibs/task/TypeRepository.hpp>

#include <rw/core/Ptr.hpp>
#include <rw/math/Q.hpp>
#include <rw/math/Transform3D.hpp>

namespace rwlibs { namespace task {

    /** @addtogroup task */
    /*@{*/

    /**
     * @brief Base class for targets
     */
    class TargetBase : public Entity
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< TargetBase > Ptr;

        /**
         * @brief Constructs TargetBase with a given type
         * @param targetType [in] Type of the target
         */
        TargetBase (int targetType = -1) : Entity (EntityType::Target), _targetType (targetType) {}

        /**
         * @brief Destructor
         */
        virtual ~TargetBase () {}

        /**
         * @brief Returns the type of target
         */
        Type targetType () { return _targetType; }

        /**
         * @brief Returns the value of the target.
         *
         * The user need to provide the type as template argument.
         *
         * May throw a rw::core::Exception if type conversion is invalid.
         * @return Value of target
         */
        template< class T > T& getValue ();

      protected:
        //! @brief The type of the target.
        Type _targetType;
    };

    /**
     * @brief Template class implementing Target
     */
    template< class T > class Target : public TargetBase
    {
      public:
        //! @brief smart pointer type to this class
        typedef rw::core::Ptr< Target< T > > Ptr;
        /**
         * @brief Construct Target with value \b value.
         * @param value [in] Value of target
         */
        Target (const T& value) : _value (value)
        {
            _targetType = TypeRepository::instance ().get< T > (true);
        }

        /**
         * @brief Returns the value of the target
         * @return Value of target
         */
        T& get () { return _value; }

        /**
         * @brief Returns the value of the target
         * @return Value of target
         */
        const T& get () const { return _value; }

        /**
         * @brief Make a copy of the target.
         * @return new identical target.
         */
        rw::core::Ptr< Target< T > > clone ()
        {
            return rw::core::ownedPtr (new Target< T > (*this));
        }

      private:
        T _value;
    };

    /**
     * Definition of Target with type rw::math::Q
     */
    typedef Target< rw::math::Q > QTarget;

    /**
     * Definition of Target with type rw::math::Transform3D
     */
    typedef Target< rw::math::Transform3D<> > CartesianTarget;

    extern template class Target< rw::math::Q >;
    extern template class Target< rw::math::Transform3D<> >;

    template< class T > T& TargetBase::getValue ()
    {
        Target< T >* target = dynamic_cast< Target< T >* > (this);
        if (target != NULL) {
            return target->get ();
        }
        RW_THROW ("Unable to convert target to specified type");
    }

    /** @} */

}}    // namespace rwlibs::task

#endif    // end include guard
