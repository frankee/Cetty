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

#include "cetty/handler/codec/http/DefaultHttpRequest.h"
#include "cetty/util/StringUtil.h"
#include "cetty/util/internal/ConversionUtil.h"

namespace cetty { namespace handler { namespace codec { namespace http { 

using namespace cetty::util;
using namespace cetty::util::internal;

std::string DefaultHttpRequest::toString() const {
    std::string buf;
    buf.reserve(2048);

    buf.append("HttpRequest");
    buf.append("(chunked: ");
    buf.append(ConversionUtil::toString(isChunked()));
    buf.append(")");
    buf.append(StringUtil::NEWLINE);
    buf.append(getMethod().toString());
    buf.append(" ");
    buf.append(getUri());
    buf.append(" ");
    buf.append(getProtocolVersion().getText());
    buf.append(StringUtil::NEWLINE);
    message.appendHeaders(buf);

    // Remove the last newline.
    buf[buf.length() - StringUtil::NEWLINE.length()] = '\0';
    return buf;
}

}}}}