/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#if !defined(CETTY_UTIL_INTEGER_H)
#define CETTY_UTIL_INTEGER_H

#include <string>
#include <limits.h>
#include <stdio.h>

#include <boost/assert.hpp>
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>

#include "cetty/util/Exception.h"

namespace cetty { namespace util {

class Integer {
public:
    static const int MAX_VALUE = INT_MAX;
    static const int MIN_VALUE = INT_MIN;

    Integer() : value(MAX_VALUE) {}
    Integer(int value) : value(value) {}
    Integer(const Integer& integer) : value(integer.value) {}
    Integer(const std::string& s) : value(MAX_VALUE) { if (!s.empty()) value = atoi(s.c_str()); }

    Integer& operator =(const Integer& integer) { this->value = integer.value; return *this; }
    bool operator ==(const Integer& integer) const { return this->value == integer.value; }
    bool operator !=(const Integer& integer) const { return this->value != integer.value; }
    bool operator < (const Integer& integer) const { return this->value <  integer.value; }

    Integer& operator +=(const Integer& integer) {
        this->value += integer.value; return *this;
    }
    Integer& operator +=(int value) { this->value += value; return *this; }

    char byteValue() { return char(value & 0xf); }
    int  intValue() { return value; }

    std::string toString() {
        return toString(value);
    }

    static Integer decode(const std::string& nm);

    // Returns a Integer instance representing the specified int value.
    static Integer valueOf(int i) { return Integer(i); }
    static Integer valueOf(const std::string& s) { return Integer(s); }
    static Integer valueOf(const std::string& s, int radix);

    static int parse(const std::string& s) { return parse(s.c_str()); }
    static int parse(const std::string& s, int radix) {
        return parse(s.c_str(), radix);
    }
    static int parse(const char* s) { BOOST_ASSERT(s); return atoi(s); }
    static int parse(const char* s, int radix) {
        if (radix == 16) {
            unsigned int value;
            if (tryParseHex(s, value)) {
                return value;
            }
            else {
                throw SyntaxException("Not a valid integer", s);
            }
        }

        throw SyntaxException("Not a supported integer", s);
    }

    static bool tryParse(const std::string& s, int& value) {
        char temp;
        return sscanf(s.c_str(), "%d%c", &value, &temp) == 1;
    }

    static bool tryParseHex(const std::string& s, unsigned int& value) {
        return tryParseHex(s.c_str(), value);
    }

    static bool tryParseHex(const char* s, unsigned int& value) {
        BOOST_ASSERT(s);
        char temp;
        return sscanf(s, "%x%c", &value, &temp) == 1;
    }

    static std::string toBinaryString(int i);
    static std::string toHexString(int i) {
        std::string buf;
        appendHex(buf, i);
        return buf;
    }

    static std::string toOctalString(int i);

    static std::string toString(int i) {
        std::string buf;
        append(buf, i);
        return buf;
    }
    static std::string toString(int i, int width, bool zeroPadded = false);
    static std::string toBinaryString(int i, int width, bool zeroPadded = false);
    static std::string toOctalString(int i, int width, bool zeroPadded = false);
    static std::string toHexString(int i, int width, bool zeroPadded = false) {
        std::string buf;
        append(buf, width, zeroPadded);
        return buf;
    }

    static void append(std::string& str, int i) {
        char buf[64];
        sprintf(buf, "%d", i);
        str += buf;
    }

    static void append(std::string& str, int value, int width, bool zeroPadded = false) {
        BOOST_ASSERT(width > 0 && width < 64);
        char buffer[64];

        if (zeroPadded) {
            sprintf(buffer, "%0*d", width, value);
        }
        else {
            sprintf(buffer, "%*d", width, value);
        }

        str.append(buffer);
    }

    static void appendBinary(std::string& str, int value);
    static void appendHex(std::string& str, int value) {
        char buffer[64];
        sprintf(buffer, "%X", value);
        str.append(buffer);
    }

    static void appendHex(std::string& str, int value, int width, bool zeroPadded) {
        BOOST_ASSERT(width > 0 && width < 64);
        char buffer[64];

        if (zeroPadded) {
            sprintf(buffer, "%0*X", width, value);
        }
        else {
            sprintf(buffer, "%*X", width, value);
        }

        str.append(buffer);
    }

    static void appendOctal(std::string& str, int value, int width, bool zeroPadded);

private:
    int value;
};

}}

#endif //#if !defined(CETTY_UTIL_INTEGER_H)

