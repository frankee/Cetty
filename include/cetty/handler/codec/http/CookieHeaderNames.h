#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIEHEADERNAMES_H)
#define CETTY_HANDLER_CODEC_HTTP_COOKIEHEADERNAMES_H

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

namespace cetty { namespace handler { namespace codec { namespace http { 

/**
 * 
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */
class CookieHeaderNames {
public:
    static const std::string PATH = "Path";

    static const std::string EXPIRES = "Expires";

    static const std::string MAX_AGE = "Max-Age";

    static const std::string DOMAIN = "Domain";

    static const std::string SECURE = "Secure";

    static const std::string HTTPONLY = "HTTPOnly";

    static const std::string COMMENT = "Comment";

    static const std::string COMMENTURL = "CommentURL";

    static const std::string DISCARD = "Discard";

    static const std::string PORT = "Port";

    static const std::string VERSION = "Version";

private:
    CookieHeaderNames() {
        // Unused.
    }
};

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIEHEADERNAMES_H)
