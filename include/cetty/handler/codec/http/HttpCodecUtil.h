#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCODECUTIL_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPCODECUTIL_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <string>

namespace cetty { namespace handler { namespace codec { namespace http { 

class HttpMessage;

/**
 * 
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

class HttpCodecUtil {
public:
    //space ' '
    static const char SP = 32;

    //tab ' '
    static const char HT = 9;

    /**
     * Carriage return
     */
    static const char CR = 13;

    /**
     * Equals '='
     */
    static const char EQUALS = 61;

    /**
     * Line feed character
     */
    static const char LF = 10;

    /**
     * carriage return line feed
     */
    static const char CRLF[2];

    /**
    * Colon ':'
    */
    static const char COLON = 58;

    /**
    * Semicolon ';'
    */
    static const char SEMICOLON = 59;

     /**
    * comma ','
    */
    static const char COMMA = 44;

    static const char DOUBLE_QUOTE = '"';

public:
    static void validateHeaderName(const std::string& name);
    static void validateHeaderValue(const std::string& value);
    static bool isTransferEncodingChunked(const HttpMessage& m);

private:
    HttpCodecUtil() {}
};

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCODECUTIL_H)
