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

#include <boost/crc.hpp>
#include "cetty/util/StringUtil.h"

namespace cetty { namespace util { 

const std::string StringUtil::NEWLINE("\r\n");

int StringUtil::hashCode(const std::string& str) {
    boost::crc_32_type crc32;
    crc32.process_bytes((void const*)(str.data()), str.size());
    return crc32.checksum();
}

std::string StringUtil::stripControlCharacters(const std::string& value) {
    if (value.empty()) {
        return value;
    }

    bool hasControlChars = false;
    for (int i = (int)(value.length()) - 1; i >= 0; --i) {
        if (Character::isISOControl(value.at(i))) {
            hasControlChars = true;
            break;
        }
    }

    if (!hasControlChars) {
        return value;
    }

    std::string buf;
    buf.reserve(value.length());

    size_t i = 0;

    // Skip initial control characters (i.e. left trim)
    for (; i < value.length(); ++i) {
        if (!Character::isISOControl(value.at(i))) {
            break;
        }
    }

    // Copy non control characters and substitute control characters with
    // a space.  The last control characters are trimmed.
    bool suppressingControlChars = false;
    for (; i < value.length(); ++i) {
        if (Character::isISOControl(value.at(i))) {
            suppressingControlChars = true;
            continue;
        }
        else {
            if (suppressingControlChars) {
                suppressingControlChars = false;
                buf += ' ';
            }
            buf += value.at(i);
        }
    }

    return buf;
}

}}