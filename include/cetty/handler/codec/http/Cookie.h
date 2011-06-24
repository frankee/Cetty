#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIE_H)
#define CETTY_HANDLER_CODEC_HTTP_COOKIE_H

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
 * An HTTP <a href="http://en.wikipedia.org/wiki/HTTP_cookie">Cookie</a>.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class Cookie /*extends Comparable<Cookie> */{
public:
    virtual ~Cookie() {}

    /**
     * Returns the name of this cookie.
     */
    virtual const std::string& getName() const = 0;

    /**
     * Returns the value of this cookie.
     */
    virtual const std::string& getValue() const = 0;

    /**
     * Sets the value of this cookie.
     */
    virtual void setValue(const std::string& value) = 0;

    /**
     * Returns the domain of this cookie.
     */
    virtual const std::string& getDomain() const = 0;

    /**
     * Sets the domain of this cookie.
     */
    virtual void setDomain(const std::string& domain) = 0;

    /**
     * Returns the path of this cookie.
     */
    virtual const std::string& getPath() const = 0;

    /**
     * Sets the path of this cookie.
     */
    virtual void setPath(const std::string& path) = 0;

    /**
     * Returns the comment of this cookie.
     */
    virtual const std::string& getComment() const = 0;

    /**
     * Sets the comment of this cookie.
     */
    virtual void setComment(const std::string& comment) = 0;

    /**
     * Returns the max age of this cookie in seconds.
     */
    virtual int getMaxAge() const = 0;

    /**
     * Sets the max age of this cookie in seconds.  If <tt>0</tt> is specified,
     * this cookie will be removed by browser because it will be expired
     * immediately.  If <tt>-1</tt> is specified, this cookie will be removed
     * when a user terminates browser.
     */
    virtual void setMaxAge(int maxAge) = 0;

    /**
     * Returns the version of this cookie.
     */
    virtual int getVersion() const = 0;

    /**
     * Sets the version of this cookie.
     */
    virtual void setVersion(int version) = 0;

    /**
     * Returns the secure flag of this cookie.
     */
    virtual bool isSecure() const = 0;

    /**
     * Sets the secure flag of this cookie.
     */
    virtual void setSecure(bool secure) = 0;

    /**
     * Returns if this cookie cannot be accessed through client side script.
     * This flag works only if the browser supports it.  For more information,
     * see <a href="http://www.owasp.org/index.php/HTTPOnly">here</a>.
     */
    virtual bool isHttpOnly() const = 0;

    /**
     * Sets if this cookie cannot be accessed through client side script.
     * This flag works only if the browser supports it.  For more information,
     * see <a href="http://www.owasp.org/index.php/HTTPOnly">here</a>.
     */
    virtual void setHttpOnly(bool httpOnly) = 0;

    /**
     * Returns the comment URL of this cookie.
     */
    const std::string& getCommentUrl() const = 0;

    /**
     * Sets the comment URL of this cookie.
     */
    virtual void setCommentUrl(const std::string& commentUrl) = 0;

    /**
     * Returns the discard flag of this cookie.
     */
    virtual bool isDiscard() const = 0;

    /**
     * Sets the discard flag of this cookie.
     */
    virtual void setDiscard(bool discard) = 0;

    /**
     * Returns the ports of this cookie.
     */
    virtual std::vector<int> getPorts() const = 0;

    /**
     * Sets the ports of this cookie.
     */
    virtual void setPorts(const std::vector<int>& ports) = 0;
};


}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIE_H)
