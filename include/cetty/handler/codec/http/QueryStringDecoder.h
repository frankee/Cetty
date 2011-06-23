#if !defined(CETTY_HANDLER_CODEC_HTTP_QUERYSTRINGDECODER_H)
#define CETTY_HANDLER_CODEC_HTTP_QUERYSTRINGDECODER_H

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

#include <map>
#include <vector>
#include <string>

#include "cetty/util/URI.h"

namespace cetty { namespace handler { namespace codec { namespace http { 

using namespace cetty::util;

/**
 * Splits an HTTP query string into a path string and key-value parameter pairs.
 * This decoder is for one time use only.  Create a new instance for each URI:
 * <pre>
 * {@link QueryStringDecoder} decoder = new {@link QueryStringDecoder}("/hello?recipient=world");
 * assert decoder.getPath().equals("/hello");
 * assert decoder.getParameters().get("recipient").equals("world");
 * </pre>
 *
 * 
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="http://tsunanet.net/">Benoit Sigoure</a>
 * @version $Rev: 2302 $, $Date: 2010-06-14 20:07:44 +0900 (Mon, 14 Jun 2010) $
 *
 * @see QueryStringEncoder
 *
 * @apiviz.stereotype utility
 * @apiviz.has        org.jboss.netty.handler.codec.http.HttpRequest oneway - - decodes
 */

class QueryStringDecoder {
public:
    typedef std::map<std::string, std::vector<std::string> > ParametersType;

public:
    /**
     * Creates a new decoder that decodes the specified URI. The decoder will
     * assume that the query string is encoded in UTF-8.
     */
    QueryStringDecoder(const std::string& uri) : uri(uri) {
    }

    /**
     * Creates a new decoder that decodes the specified URI. The decoder will
     * assume that the query string is encoded in UTF-8.
     */
    QueryStringDecoder(const URI& uri) : uri(uri.toString()) {
    }

    /**
     * Returns the decoded path string of the URI.
     */
    const std::string& getPath() const {
        if (path.empty()) {
            std::string::size_type pathEndPos = uri.find('?');
            if (pathEndPos == std::string::npos) {
                path = uri;
            }
            else {
                path = uri.substr(0, pathEndPos);
            }
        }
        return path;
    }

    /**
     * Returns the decoded key-value parameter pairs of the URI.
     */
    const ParametersType& getParameters() const {
        if (params.empty()) {
            size_t pathLength = getPath().length();
            if (uri.length() != pathLength) {
                decodeParams(uri.substr(pathLength + 1), params);
            }
        }
        return params;
    }

private:
    void decodeParams(const std::string& s, ParametersType& params) const {
        std::string name;
        std::string::size_type pos = 0; // Beginning of the unprocessed region
        std::string::size_type i;       // End of the unprocessed region
        char c = 0;  // Current character
        for (i = 0; i < s.length(); ++i) {
            c = s[i];
            if (c == '=' && name.empty()) {
                if (pos != i) {
                    decodeComponent(s.substr(pos, i - pos), name);
                }
                pos = i + 1;
            }
            else if (c == '&') {
                if (name.empty() && pos != i) {
                    // We haven't seen an `=' so far but moved forward.
                    // Must be a param of the form '&a&' so add it with
                    // an empty value.
                    addParam(params, decodeComponent(s.substr(pos, i - pos)), "");
                }
                else if (!name.empty()) {
                    addParam(params, name, decodeComponent(s.substr(pos, i - pos)));
                    name.clear();
                }
                pos = i + 1;
            }
        }

        if (pos != i) {  // Are there characters we haven't dealt with?
            if (name.empty()) {     // Yes and we haven't seen any `='.
                addParam(params, decodeComponent(s.substr(pos, i - pos)), "");
            }
            else {                // Yes and this must be the last value.
                addParam(params, name, decodeComponent(s.substr(pos, i - pos)));
            }
        }
        else if (!name.empty()) {  // Have we seen a name without value?
            addParam(params, name, "");
        }
    }

    std::string decodeComponent(const std::string& s) const {
        std::string str;
        decodeComponent(s, str);
        return str;
    }

    void decodeComponent(const std::string& s, std::string& decoded) const {
        URI::decode(s, decoded);
    }

    void addParam(ParametersType& params, const std::string& name, const std::string& value) const {
        std::vector<std::string>& values = params[name];
        values.push_back(value);
    }

private:
    std::string uri;
    mutable std::string path;
    mutable std::map<std::string, std::vector<std::string> > params;
};


}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_QUERYSTRINGDECODER_H)
