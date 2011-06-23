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

#include "cetty/buffer/ChannelBufferIndexFinder.h"
#include "cetty/buffer/ChannelBuffer.h"

namespace cetty { namespace buffer {

/**
 * Index finder which locates a <tt>NUL (0x00)</tt> byte.
 */
class ChannelBufferIndexNullFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        return buffer.getByte(guessedIndex) == 0;
    }
};

/**
 * Index finder which locates a non-<tt>NUL (0x00)</tt> byte.
 */
class ChannelBufferIndexNotNulFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        return buffer.getByte(guessedIndex) != 0;
    }
};

/**
 * Index finder which locates a <tt>CR ('\r')</tt> byte.
 */
class ChannelBufferIndexCrFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        return buffer.getByte(guessedIndex) == '\r';
    }
};

/**
 * Index finder which locates a non-<tt>CR ('\r')</tt> byte.
 */
class ChannelBufferIndexNotCrFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        return buffer.getByte(guessedIndex) != '\r';
    }
};

/**
 * Index finder which locates a <tt>LF ('\n')</tt> byte.
 */
class ChannelBufferIndexLfFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        return buffer.getByte(guessedIndex) == '\n';
    }
};

/**
 * Index finder which locates a non-<tt>LF ('\n')</tt> byte.
 */
class ChannelBufferIndexNotLfFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        return buffer.getByte(guessedIndex) != '\n';
    }
};

/**
 * Index finder which locates a <tt>CR ('\r')</tt> or <tt>LF ('\n')</tt>.
 */
class ChannelBufferIndexCrLfFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        boost::int8_t b = buffer.getByte(guessedIndex);
        return b == '\r' || b == '\n';
    }
};

/**
 * Index finder which locates a byte which is neither a <tt>CR ('\r')</tt>
 * nor a <tt>LF ('\n')</tt>.
 */
class ChannelBufferIndexNotCrLfFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        boost::int8_t b = buffer.getByte(guessedIndex);
        return b != '\r' && b != '\n';
    }
};

/**
 * Index finder which locates a linear whitespace
 * (<tt>' '</tt> and <tt>'\t'</tt>).
 */
class ChannelBufferIndexLinearWhitespaceFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        boost::int8_t b = buffer.getByte(guessedIndex);
        return b == ' ' || b == '\t';
    }
};

/**
 * Index finder which locates a byte which is not a linear whitespace
 * (neither <tt>' '</tt> nor <tt>'\t'</tt>).
 */
class ChannelBufferIndexNotLinearWhitespaceFinder : public ChannelBufferIndexFinder {
public:
    bool find(const ChannelBuffer& buffer, int guessedIndex) const {
        boost::int8_t b = buffer.getByte(guessedIndex);
        return b != ' ' && b != '\t';
    }
};

static ChannelBufferIndexNullFinder nullFinder;
static ChannelBufferIndexNotNulFinder notNullFinder;
static ChannelBufferIndexCrFinder crFinder;
static ChannelBufferIndexNotCrFinder notCrFinder;
static ChannelBufferIndexLfFinder lfFinder;
static ChannelBufferIndexNotLfFinder notLfFinder;
static ChannelBufferIndexCrLfFinder crlfFinder;
static ChannelBufferIndexNotCrLfFinder notCrlfFinder;
static ChannelBufferIndexLinearWhitespaceFinder linearWhitespaceFinder;
static ChannelBufferIndexNotLinearWhitespaceFinder notLinearWhitespaceFinder;

const ChannelBufferIndexFinder& ChannelBufferIndexFinder::NUL = nullFinder;
const ChannelBufferIndexFinder& ChannelBufferIndexFinder::NOT_NUL = notNullFinder;
const ChannelBufferIndexFinder& ChannelBufferIndexFinder::CR = crFinder;
const ChannelBufferIndexFinder& ChannelBufferIndexFinder::NOT_CR = notCrFinder;
const ChannelBufferIndexFinder& ChannelBufferIndexFinder::LF = lfFinder;
const ChannelBufferIndexFinder& ChannelBufferIndexFinder::NOT_LF = notLfFinder;
const ChannelBufferIndexFinder& ChannelBufferIndexFinder::CRLF = crlfFinder;
const ChannelBufferIndexFinder& ChannelBufferIndexFinder::NOT_CRLF = notCrlfFinder;
const ChannelBufferIndexFinder& ChannelBufferIndexFinder::LINEAR_WHITESPACE = linearWhitespaceFinder;
const ChannelBufferIndexFinder& ChannelBufferIndexFinder::NOT_LINEAR_WHITESPACE = notLinearWhitespaceFinder;

}}
