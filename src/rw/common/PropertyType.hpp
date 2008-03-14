#ifndef RW_COMMON_PROPERTYTYPE_HPP
#define RW_COMMON_PROPERTYTYPE_HPP

#include <string>

namespace rw { namespace common {

    /** @addtogroup common */
    /*@{*/

	/**
	 * @brief Represents type of a property
	 *
	 * PropertyType has a number of predefined property types. Besides it generates unique
	 * id's for new user defined types
	 */
    class PropertyType
    {
    public:
    	/**
    	 * @brief Predefined types
    	 */
        enum { UNKNOWN = 0, STRING = 1, FLOAT, DOUBLE, INT, BOOL } Types;

        /**
         * brief Constructs PropertyType with type UNKNOWN
         */
        PropertyType() : _id(UNKNOWN) {}

        /**
         * @brief Construct PropertyType with the specified type
         *
         * @param id [in] either one of the predefined types or a user defined
         * type, generated by GetNewID
         */
        PropertyType(int id) : _id(id) {}

        /**
         * @brief Constructs a new ID for a property type. The ID will be unique
         * @return the id
         */
        static int GetNewID();

        /**
         * @brief Returns id of the type
         * @return the id
         */
        int getId() const { return _id; }

        /**
           @brief The type of a value T resolved at compile time.
        */
        static PropertyType getType(const std::string&) { return PropertyType(STRING); }
        static PropertyType getType(float) { return PropertyType(FLOAT); }
        static PropertyType getType(double) { return PropertyType(DOUBLE); }
        static PropertyType getType(int) { return PropertyType(INT); }
        static PropertyType getType(bool) { return PropertyType(BOOL); }
        template <class T>
        static PropertyType getType(const T&) { return PropertyType(UNKNOWN); }

    private:
        int _id;
        static int _NextID;
    };

    /** @} */

}} // end namespaces

#endif // end include guard
