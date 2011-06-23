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

#include "cetty/buffer/Array.h"
#include <string.h>

namespace cetty { namespace buffer {

Array Array::clone(const Array& array) {
    return ConstArray::clone(array);
}

void Array::copy(const Array& dst, int dstIndex, const ConstArray& src, int srcIndex, int length) {
    const char* srcBuf = src.data();
    char* dstBuf = dst.data();

    int srcLength = src.length();
    int srcLastIndex = srcIndex + length;

    if (dstIndex < 0 || srcIndex < 0 || length < 0 || srcLastIndex > srcLength
        || dstIndex + length > dst.bufSize) {
            throw RangeException("");
    }

    if (NULL == srcBuf || NULL == dstBuf) {
        throw InvalidArgumentException("");
    }

    if (srcBuf == dst.buf) {
        memmove(dstBuf + dstIndex, srcBuf + srcIndex, length);
    }
    else {
        memcpy(dstBuf + dstIndex, srcBuf + srcIndex, length);
    }
}

cetty::buffer::Array ConstArray::clone(const ConstArray& array) {
    int dataLength = array.bufSize;
    const char* arrayData = array.buf;

    if (dataLength < 0) {
        throw RangeException("");
    }

    if (NULL == arrayData || 0 == dataLength) {
        return Array();
    }

    char* newData = new char[dataLength];
    memcpy(newData, arrayData, dataLength);
    return Array(newData, dataLength);
}

}}
