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

#ifndef RW_COMMON_INIARCHIVE_HPP
#define RW_COMMON_INIARCHIVE_HPP

#if !defined(SWIG)
#include <rw/common/InputArchive.hpp>
#include <rw/common/OutputArchive.hpp>
#include <rw/core/macros.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#endif

namespace rw { namespace common {
    /**
     * @brief archive for loading and saving serializable classes to an ini-file format.
     */

#if defined(SWIGJAVA)
    class INIArchive : public InputArchive
#else
    class INIArchive : public InputArchive, public virtual OutputArchive
#endif 
    {
      public:
        /**
         * @brief constructor
         */
        INIArchive () : _ofs (NULL), _ifs (NULL), _fstr (NULL), _iostr (NULL), _isopen (false) {}

        INIArchive (const std::string& filename) :
            _ofs (NULL), _ifs (NULL), _fstr (NULL), _iostr (NULL), _isopen (false)
        {
            open (filename);
        }

        INIArchive (std::ostream& ofs) :
            _ofs (NULL), _ifs (NULL), _fstr (NULL), _iostr (NULL), _isopen (false)
        {
            open (ofs);
        }

        //! destructor
        virtual ~INIArchive () { close (); }

        //! @brief close this archive for streaming
        void close ();

        //! @copydoc Archive::isOpen
        bool isOpen () { return _isopen; };

        //! @copydoc Archive::flush
        void flush ();

      protected:
        //! @brief Maximum number of characters in one line.
        static const int MAX_LINE_WIDTH = 1000;

        //////////////////// SCOPE
        //! @copydoc OutputArchive::doWriteEnterScope
        void doWriteEnterScope (const std::string& id);

        //! @copydoc OutputArchive::doWriteLeaveScope
        void doWriteLeaveScope (const std::string& id);

        //! @copydoc InputArchive::doReadEnterScope
        void doReadEnterScope (const std::string& id);

        //! @copydoc InputArchive::doReadLeaveScope
        void doReadLeaveScope (const std::string& id);

        void doOpenArchive (const std::string& filename);
        void doOpenArchive (std::iostream& stream);
        void doOpenOutput (std::ostream& ofs);
        void doOpenInput (std::istream& ifs);

        ///////////////////////// WRITING
        virtual void doWrite (bool val, const std::string& id)
        {
            if (val)
                doWrite ((int) 1, id);
            else
                doWrite ((int) 0, id);
        }
        void doWrite (int8_t val, const std::string& id);
        void doWrite (uint8_t val, const std::string& id);

        void doWrite (int16_t val, const std::string& id) { writeValue (val, id); };
        void doWrite (uint16_t val, const std::string& id) { writeValue (val, id); };
        void doWrite (int32_t val, const std::string& id) { writeValue (val, id); };
        void doWrite (uint32_t val, const std::string& id) { writeValue (val, id); };
        void doWrite (int64_t val, const std::string& id) { writeValue (val, id); };
        void doWrite (uint64_t val, const std::string& id) { writeValue (val, id); };
        void doWrite (float val, const std::string& id) { writeValue (val, id); };
        void doWrite (double val, const std::string& id) { writeValue (val, id); };
        void doWrite (const std::string& val, const std::string& id) { writeValue (val, id); };

        void doWrite (const std::vector< bool >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< int8_t >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< uint8_t >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< int16_t >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< uint16_t >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< int32_t >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< uint32_t >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< int64_t >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< uint64_t >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< float >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< double >& val, const std::string& id)
        {
            writeValue (val, id);
        };
        void doWrite (const std::vector< std::string >& val, const std::string& id)
        {
            writeValue (val, id);
        };

        //! @copydoc OutputArchive::doWrite(const Eigen::MatrixXd&, const std::string&)
        void doWrite (const Eigen::MatrixXd& val, const std::string& id) { writeMatrix (val, id); }

        //! @copydoc OutputArchive::doWrite(const Eigen::VectorXd&, const std::string&)
        void doWrite (const Eigen::VectorXd& val, const std::string& id) { writeMatrix (val, id); }

        // template<class T>
        // void doWrite(const T& data, const std::string& id){ OutputArchive::write<T>(data,id); }

        //! @copydoc OutputArchive::doWrite(const std::vector<bool>&,const std::string&)
        template< class T > void writeValue (const std::vector< T >& val, const std::string& id)
        {
            (*_ofs) << id << "=";
            for (const T rval : val) {
                (*_ofs) << rval << " ";
            }
            (*_ofs) << "\n";
        }

        //! @copydoc OutputArchive::doWrite(bool,const std::string&)
        template< class T > void writeValue (const T& val, const std::string& id)
        {
            (*_ofs) << id << "=" << val << "\n";
        }

        /**
         * @brief Write a generic Eigen matrix to output.
         * @param val [in] the matrix to output.
         * @param id [in] identifier for the matrix.
         */
        template< class Derived >
        void writeMatrix (const Eigen::DenseCoeffsBase< Derived, Eigen::ReadOnlyAccessors >& val,
                          const std::string& id)
        {
            typedef
                typename Eigen::DenseCoeffsBase< Derived, Eigen::ReadOnlyAccessors >::Index Index;
#if __cplusplus >= 201103L
            const int digitsType = std::numeric_limits< double >::max_digits10;
#else
            const int digitsType = static_cast< int > (
                std::floor (std::numeric_limits< double >::digits * std::log10 (2) + 2));
#endif
            const int digits = static_cast< int > (_ofs->precision ()) < digitsType
                                   ? static_cast< int > (_ofs->precision ())
                                   : digitsType;
            const int spacePerVal =
                digits + 1 + 1 + 1 +
                5;    // including a space, sign, decimal seperator and exponential (e.g e+123)
            const int maxVal = MAX_LINE_WIDTH / spacePerVal;
            if (maxVal == 0)
                RW_THROW ("Please increase the MAX_LINE_WIDTH or decrease the streams precision to "
                          "write matrix.");
            (*_ofs) << id << "=[" << val.rows () << "x" << val.cols () << "]\n";
            int printed = 0;
            for (Index i = 0; i < val.rows (); i++) {
                for (Index j = 0; j < val.cols (); j++) {
                    if (printed == 0)
                        (*_ofs) << val (i, j);
                    else
                        (*_ofs) << " " << val (i, j);
                    printed++;
                    if (printed == maxVal || j == val.cols () - 1) {
                        printed = 0;
                        (*_ofs) << "\n";
                    }
                }
            }
            RW_ASSERT (printed == 0);
        }

        // template<class T>
        // void doWrite(const T& object, const std::string& id){
        //	((OutputArchive*)this)->write<T>(object, id);
        //}
        ///////////////// READING

        /**
         * @brief Split a line in a name and a value.
         * @return a pair of strings. First is name, second is the value.
         */
        std::pair< std::string, std::string > getNameValue ()
        {
            std::string line (_line);
            for (size_t i = 0; i < line.size (); i++) {
                if (line[i] == '=') {
                    char nname[100], nval[100];
                    // split is at i
                    std::string name = line.substr (0, i);
                    std::string val  = line.substr (i + 1, line.size () - 1);

                    sscanf (name.c_str (), "%s", nname);
                    sscanf (val.c_str (), "%s", nval);

                    return std::make_pair (nname, val);
                }
            }
            RW_THROW ("Not valid ini property! From line: " << line);
            return std::make_pair ("", "");
        }

        virtual void doRead (bool& val, const std::string& id);
        virtual void doRead (int8_t& val, const std::string& id);
        virtual void doRead (uint8_t& val, const std::string& id);
        virtual void doRead (int16_t& val, const std::string& id)
        {
            readValue< int16_t > (val, id);
        }
        virtual void doRead (uint16_t& val, const std::string& id)
        {
            readValue< uint16_t > (val, id);
        }
        virtual void doRead (int32_t& val, const std::string& id)
        {
            readValue< int32_t > (val, id);
        }
        virtual void doRead (uint32_t& val, const std::string& id)
        {
            readValue< uint32_t > (val, id);
        }
        virtual void doRead (int64_t& val, const std::string& id)
        {
            readValue< int64_t > (val, id);
        }
        virtual void doRead (uint64_t& val, const std::string& id)
        {
            readValue< uint64_t > (val, id);
        }
        virtual void doRead (float& val, const std::string& id) { readValue< float > (val, id); }
        virtual void doRead (double& val, const std::string& id) { readValue< double > (val, id); }
        virtual void doRead (std::string& val, const std::string& id);

        virtual void doRead (std::vector< bool >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< int8_t >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< uint8_t >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< int16_t >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< uint16_t >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< int32_t >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< uint32_t >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< int64_t >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< uint64_t >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< float >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< double >& val, const std::string& id)
        {
            readValue (val, id);
        }
        virtual void doRead (std::vector< std::string >& val, const std::string& id);

        //! @copydoc InputArchive::doRead(Eigen::MatrixXd&, const std::string&)
        virtual void doRead (Eigen::MatrixXd& val, const std::string& id) { readMatrix (val, id); }

        //! @copydoc InputArchive::doRead(Eigen::VectorXd&, const std::string&)
        virtual void doRead (Eigen::VectorXd& val, const std::string& id) { readMatrix (val, id); }

        // template<class T>
        // void doRead(T& object, const std::string& id){
        //    ((InputArchive*)this)->read<T>(object, id);
        //}

        //! @copydoc InputArchive::doRead(std::vector<bool>&,const std::string&)
        template< class T > void readValue (std::vector< T >& val, const std::string& id)
        {
            getLine ();
            std::pair< std::string, std::string > valname = getNameValue ();
            if (id != valname.first)
                RW_WARN ("mismatched ids: " << id << " ---- " << valname.first);
            // read from array
            std::vector< std::string > result;
            val.clear ();
            boost::split (result, valname.second, boost::is_any_of ("\t "));
            for (std::string& rval : result) {
                if (rval.empty ())
                    continue;
                // std::cout << rval << " : ";
                val.push_back (boost::lexical_cast< T > (rval));
            }
        }

        //! @copydoc InputArchive::doRead(bool&,const std::string&)
        template< class T > void readValue (T& val, const std::string& id)
        {
            getLine ();

            std::pair< std::string, std::string > valname = getNameValue ();
            if (id != valname.first)
                RW_WARN ("mismatched ids: " << id << " ---- " << valname.first);
            // std::cout << "Reading: " << id << "= " << valname.second << "\n";
            val = boost::lexical_cast< T > (valname.second);
        }

        /**
         * @brief Read a generic Eigen matrix.
         * @param val [out] the result.
         * @param id [in] identifier for the matrix - gives a warning if it does not match the id in
         * the file.
         */
        template< class Derived >
        void readMatrix (Eigen::PlainObjectBase< Derived >& val, const std::string& id)
        {
            typedef typename Eigen::PlainObjectBase< Derived >::Index Index;
            getLine ();
            std::pair< std::string, std::string > valname = getNameValue ();
            if (id != valname.first)
                RW_WARN ("mismatched ids: " << id << " ---- " << valname.first);
            // read from array
            std::vector< std::string > dims;
            boost::split (dims, valname.second, boost::is_any_of ("[x]"));
            const int M = boost::lexical_cast< int > (dims[1]);
            const int N = boost::lexical_cast< int > (dims[2]);
            val.resize (M, N);
            Index i = 0;
            Index j = 0;
            for (int cur = 0; cur < M * N;) {
                getLine ();
                std::vector< std::string > values;
                boost::split (values, _line, boost::is_any_of (" \n"));
                for (std::string& rval : values) {
                    if (rval.empty ())
                        continue;
                    val (i, j) = boost::lexical_cast< double > (rval);
                    j++;
                    cur++;
                    if (j == N) {
                        i++;
                        j = 0;
                    }
                }
            }
        }

        /**
         * @brief Read one line from input.
         *
         * Line length is maximum MAX_LINE_WIDTH.
         *
         * @return true if success.
         */
        bool getLine ();

      private:
        std::string getScope ()
        {
            if (_scope.size () == 0)
                return "";
            std::stringstream sstr;
            for (size_t i = 0; i < _scope.size () - 1; i++)
                sstr << _scope[i] << ".";
            sstr << _scope.back ();
            return sstr.str ();
        }

      private:
        std::ostream* _ofs;
        std::istream* _ifs;
        std::fstream* _fstr;
        std::iostream* _iostr;
        char _line[MAX_LINE_WIDTH];
        bool _isopen;
        std::vector< std::string > _scope;
    };

}}    // namespace rw::common

#endif
