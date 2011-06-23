#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTCOOKIE_H)
#define CETTY_HANDLER_CODEC_HTTP_DEFAULTCOOKIE_H

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

#include "cetty/handler/codec/http/Cookie.h"

namespace cetty { namespace handler { namespace codec { namespace http { 

/**
 * The default {@link Cookie} implementation.
 *
 * 
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

class DefaultCookie : public Cookie {
public:
    private static final Set<std::string> RESERVED_NAMES = new TreeSet<std::string>(CaseIgnoringComparator.INSTANCE);

    static {
        RESERVED_NAMES.add("Domain");
        RESERVED_NAMES.add("Path");
        RESERVED_NAMES.add("Comment");
        RESERVED_NAMES.add("CommentURL");
        RESERVED_NAMES.add("Discard");
        RESERVED_NAMES.add("Port");
        RESERVED_NAMES.add("Max-Age");
        RESERVED_NAMES.add("Expires");
        RESERVED_NAMES.add("Version");
        RESERVED_NAMES.add("Secure");
        RESERVED_NAMES.add("HTTPOnly");
    }



    /**
     * Creates a new cookie with the specified name and value.
     */
    DefaultCookie(std::string name, std::string value) {
        if (name == null) {
            throw new NullPointerException("name");
        }
        name = name.trim();
        if (name.length() == 0) {
            throw new InvalidArgumentException("empty name");
        }

        for (int i = 0; i < name.length(); i ++) {
            char c = name.charAt(i);
            if (c > 127) {
                throw new InvalidArgumentException(
                        "name contains non-ascii character: " + name);
            }

            // Check prohibited characters.
            switch (c) {
            case '\t': case '\n': case 0x0b: case '\f': case '\r':
            case ' ':  case ',':  case ';':  case '=':
                throw new InvalidArgumentException(
                        "name contains one of the following prohibited characters: " +
                        "=,; \\t\\r\\n\\v\\f: " + name);
            }
        }

        if (RESERVED_NAMES.contains(name)) {
            throw new InvalidArgumentException("reserved name: " + name);
        }

        this.name = name;
        setValue(value);
    }

    const std::string& getName() const {
        return name;
    }

    const std::string& getValue() const {
        return value;
    }

    void setValue(const std::string& value) {
        this->value = value;
    }

    const std::string& getDomain() const {
        return domain;
    }

    void setDomain(const std::string& domain) {
        this.domain = validateValue("domain", domain);
    }

    const std::string& getPath() const {
        return path;
    }

    void setPath(const std::string& path) {
        this.path = validateValue("path", path);
    }

    const std::string& getComment() const {
        return comment;
    }

    void setComment(const std::string& comment) {
        this.comment = validateValue("comment", comment);
    }

    const std::string& getCommentUrl() const {
        return commentUrl;
    }

    void setCommentUrl(const std::string& commentUrl) {
        this.commentUrl = validateValue("commentUrl", commentUrl);
    }

    bool isDiscard() const {
        return discard;
    }

    void setDiscard(bool discard) const {
        this.discard = discard;
    }

    const std::vector<int>& getPorts() const {
        return ports;
    }

    void setPorts(int... ports) {
        if (ports == null) {
            throw new NullPointerException("ports");
        }

        int[] portsCopy = ports.clone();
        if (portsCopy.length() == 0) {
            unmodifiablePorts = this.ports = Collections.emptySet();
        } else {
            Set<Integer> newPorts = new TreeSet<Integer>();
            for (int p: portsCopy) {
                if (p <= 0 || p > 65535) {
                    throw new InvalidArgumentException("port out of range: " + p);
                }
                newPorts.add(Integer.valueOf(p));
            }
            this.ports = newPorts;
            unmodifiablePorts = null;
        }
    }

    void setPorts(Iterable<Integer> ports) {
        Set<Integer> newPorts = new TreeSet<Integer>();
        for (int p: ports) {
            if (p <= 0 || p > 65535) {
                throw new InvalidArgumentException("port out of range: " + p);
            }
            newPorts.add(Integer.valueOf(p));
        }
        if (newPorts.isEmpty()) {
            unmodifiablePorts = this.ports = Collections.emptySet();
        } else {
            this.ports = newPorts;
            unmodifiablePorts = null;
        }
    }

    int getMaxAge() const {
        return maxAge;
    }

    void setMaxAge(int maxAge) {
        if (maxAge < -1) {
            throw new InvalidArgumentException(
                    "maxAge must be either -1, 0, or a positive integer: " +
                    maxAge);
        }
        this->maxAge = maxAge;
    }

    int getVersion() const {
        return version;
    }

    void setVersion(int version) {
        this->version = version;
    }

    bool isSecure() const {
        return secure;
    }

    void setSecure(bool secure) {
        this->secure = secure;
    }

    bool isHttpOnly() const {
        return httpOnly;
    }

    void setHttpOnly(bool httpOnly) {
        this->httpOnly = httpOnly;
    }

    int hashCode() {
        return getName().hashCode();
    }

    bool equals(const Cookie& that) const {
        if (!getName().equalsIgnoreCase(that.getName())) {
            return false;
        }

        if (getPath() == null && that.getPath() != null) {
            return false;
        } else if (that.getPath() == null) {
            return false;
        }
        if (!getPath().equals(that.getPath())) {
            return false;
        }

        if (getDomain() == null && that.getDomain() != null) {
            return false;
        } else if (that.getDomain() == null) {
            return false;
        }
        if (!getDomain().equalsIgnoreCase(that.getDomain())) {
            return false;
        }

        return true;
    }

    int compareTo(const Cookie& c) const {
        int v;
        v = getName().compareToIgnoreCase(c.getName());
        if (v != 0) {
            return v;
        }

        if (getPath() == null && c.getPath() != null) {
            return -1;
        } else if (c.getPath() == null) {
            return 1;
        }
        v = getPath().compareTo(c.getPath());
        if (v != 0) {
            return v;
        }

        if (getDomain() == null && c.getDomain() != null) {
            return -1;
        } else if (c.getDomain() == null) {
            return 1;
        }
        v = getDomain().compareToIgnoreCase(c.getDomain());
        return v;
    }

    std::string toString() const {
        StringBuilder buf = new StringBuilder();
        buf.append(getName());
        buf.append('=');
        buf.append(getValue());
        if (getDomain() != null) {
            buf.append(", domain=");
            buf.append(getDomain());
        }
        if (getPath() != null) {
            buf.append(", path=");
            buf.append(getPath());
        }
        if (getComment() != null) {
            buf.append(", comment=");
            buf.append(getComment());
        }
        if (getMaxAge() >= 0) {
            buf.append(", maxAge=");
            buf.append(getMaxAge());
            buf.append('s');
        }
        if (isSecure()) {
            buf.append(", secure");
        }
        if (isHttpOnly()) {
            buf.append(", HTTPOnly");
        }
        return buf.toString();
    }

private:
    void validateValue(const std::string& name, const std::string& value, std::string& output) const {
        output = boost::trim_copy(value);
        for (size_t i = 0; i < output.length(); ++i) {
            char c = output.at(i);
            switch (c) {
            case '\r': case '\n': case '\f': case 0x0b: case ';':
                throw InvalidArgumentException(
                        name + " contains one of the following prohibited characters: " +
                        ";\\r\\n\\f\\v (" + value + ')');
            }
        }
        return value;
    }

private:
    bool secure;
    bool httpOnly;

    std::string name;
    std::string value;
    std::string domain;
    std::string path;
    std::string comment;
    std::string commentUrl;
    bool discard;
    Set<Integer> ports = Collections.emptySet();
    Set<Integer> unmodifiablePorts = ports;
    int maxAge = -1;
    int version;
};

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_DEFAULTCOOKIE_H)
