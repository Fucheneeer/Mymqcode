////////////////////////////////////////////////////////////////////////////////
//
// Uri.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description:
//
////////////////////////////////////////////////////////////////////////////////


#ifndef URI_H_
#define URI_H_

#include <string>
#include "StringUtility.h"
#include "QMAPITypes.h"

namespace lightmq { namespace qmapi {

class URI {
public:
    static constexpr auto SCHEME = "tcp://";
    static constexpr auto COLON = ":";
    static constexpr auto COMMA = ",";

public:
    virtual ~URI() = default;

    URI(std::string separator = COMMA)
        : m_brokerURLs{std::string()}, m_separator{separator} {
    }

    URI(const std::string& brokerURLs, std::string separator)
        : m_brokerURLs{brokerURLs}, m_separator{separator} {
    }

    /**
     * Get a list of broker (hosts and ports) from the given URLs
     *
     * @param brokers
     *      a vector of brokers in ip address and port
     *
     * @return true if there is at least one broker address extracted from URLs
     *
     */
    bool getBrokers(std::vector<BrokerIp>& brokers) {
        std::vector<std::string> urls;
        dep::StringUtility::split(m_brokerURLs, m_separator, urls, -1);
        for (std::string& url : urls) {
            std::string brokerHost;
            uint16_t brokerPort;
            if (dep::StringUtility::startWith(url, SCHEME) == false) {
                continue;
            }
            // remove the scheme prefix
            dep::StringUtility::replacePattern(url, SCHEME, "");
            // it should be brokerHost:brokerPort after removing the prefix
            std::vector<std::string> tokens;
            dep::StringUtility::split(url, COLON, tokens, -1);
            if (tokens.size() != 2) {
                continue;
            }
            brokerHost = tokens[0];
            if (StringUtil::isNumeric(tokens[1], StringUtil::NUMTYPE::UNSIGNED) == false) {
                continue;
            }
            brokerPort = stol(tokens[1]);
            brokers.push_back(BrokerIp{brokerHost, brokerPort});
        }
        return brokers.size() > 0;
    }

    /**
     * Get an URL string from the given broker address and port
     *
     * @param brokerHost
     *      host of the broker
     * @param brokerPort
     *      port of the broker
     *
     * @return the URL string
     *
     */
    static std::string getUrl(const std::string& brokerHost, uint16_t brokerPort) {
        return SCHEME + brokerHost + COLON + std::to_string(brokerPort);
    }

    /**
     * Concatenate broker URL string with the separator given in the constructor
     *
     * @param brokerURL
     *      URL string of a broker
     *
     * @return concatenated broker URL strings
     *
     */
    URI& concat(const std::string& brokerURL) {
        if (m_brokerURLs.empty()) {
            m_brokerURLs = brokerURL;
        } else {
            m_brokerURLs += (m_separator + brokerURL);
        }
        return *this;
    }


private:
    std::string m_brokerURLs;
    std::string m_separator;
};

}} // namespace lightmq { namespace qmapi {

#endif /* URI_H_ */
