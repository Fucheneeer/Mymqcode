////////////////////////////////////////////////////////////////////////////////
//
// StringUtil.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: String Util
//
////////////////////////////////////////////////////////////////////////////////

#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_

#include <limits>
#include <vector>
#include <tuple>
#include <stdio.h>
#include "StringUtility.h"

namespace lightmq { namespace qmapi {

    class StringUtil {
    public:
        enum class NUMTYPE {
            SIGNED,
            UNSIGNED
        };

        inline static bool isNumeric(const std::string& input, NUMTYPE type = NUMTYPE::SIGNED) {
            uint32_t startPos = 0;
            if (input.length() >= 2 && input.at(0) == '-') {
                if (type == NUMTYPE::UNSIGNED) {
                    return false;
                }
                startPos = 1;
            }
            for (uint32_t i = startPos; i < input.length(); i++) {
                if (isdigit(input.at(i)) != true) {
                    return false;
                }
            }
            return true;
        }
        inline static bool toSize(const std::string& input, uint64_t& value) { 
			if (dep::StringUtility::endsWith(input, "k") || dep::StringUtility::endsWith(input, "K")) {
                if (isNumeric(std::string(input.c_str(), input.length() - 1), value)) {
                    value *= 1024;
                    return true;
                }
                else {
                    return false;
                }
			}
			else if (dep::StringUtility::endsWith(input, "m") || dep::StringUtility::endsWith(input, "M")) {
				if (isNumeric(std::string(input.c_str(), input.length() - 1), value)) {
					value *= 1024 * 1024;
					return true;
				}
				else {
					return false;
				}
			}
			else if (dep::StringUtility::endsWith(input, "g") || dep::StringUtility::endsWith(input, "G")) {
				if (isNumeric(std::string(input.c_str(), input.length() - 1), value)) {
					value *= 1024 * 1024 * 1024;
					return true;
				}
				else {
					return false;
				}
			}
			else {
                return isNumeric(input, value);
			}
            
        }

        inline static bool isNumeric(const std::string& input, uint16_t& output)
        {
            if (isNumeric(input, NUMTYPE::UNSIGNED) == false)
            {
                return false;
            }
            try
            {
                output = static_cast<uint16_t>(stou(input));
            }
            catch (const std::exception& e)
            {
                return false;
            }
            return true;
        }

        inline static bool isNumeric(const std::string& input, uint32_t& output) {
            if (isNumeric(input, NUMTYPE::UNSIGNED) == false) {
                return false;
            }
            try {
                output = stou(input);
            }
            catch (const std::exception& e) {
                return false;
            }
            return true;
        }

        inline static bool isNumeric(const std::string& input, int32_t& output) {
            if (isNumeric(input, NUMTYPE::SIGNED) == false) {
                return false;
            }
            try {
                output = stoi(input);
            }
            catch (const std::exception& e) {
                return false;
            }
            return true;
        }

        inline static bool isNumeric(const std::string& input, uint64_t& output) {
            if (isNumeric(input, NUMTYPE::UNSIGNED) == false) {
                return false;
            }
            try {
                output = stoul(input);
            }
            catch (const std::exception& e) {
                return false;
            }
            return true;
        }

        inline static bool isNumeric(const std::string& input, int64_t& output) {
            if (isNumeric(input, NUMTYPE::SIGNED) == false) {
                return false;
            }
            try {
                output = stol(input);
            }
            catch (const std::exception& e) {
                return false;
            }
            return true;
        }

        inline static uint64_t split(const std::string& s, const std::string& delim, std::vector<std::string>& v) {
            auto i = 0;
            auto len = delim.size();
            auto pos = s.find(delim);
            while (pos != std::string::npos) {
                v.push_back(s.substr(i, pos - i));
                i = pos + len;
                pos = s.find(delim, i);
            }
            if (s.substr(i).size() != 0) {
                v.push_back(s.substr(i));
            }

            return v.size();
        }

        inline static std::tuple<std::string, std::string> split(const std::string& s, std::string delim = "=") {
            std::vector<std::string> tokens;
            uint32_t size = split(s, delim, tokens);

            if (size == 2) {
                return std::make_tuple(tokens.at(0), tokens.at(1));
            }
            else {
                return std::make_tuple("", "");
            }
        }

        inline static std::string getDelimiter(const std::string& msgDelimiter) {
            // take a delimiter in a format of x,y,z,... x/y/z are decimals
            if (isdigit(msgDelimiter.at(0))) {
                std::vector<std::string> tokens;
                split(msgDelimiter, ",", tokens);
                std::string tempDelimiter(tokens.size(), 0x0);
                for (uint32_t i = 0; i < tempDelimiter.size(); i++) {
                    tempDelimiter.replace(i, 1, 1, (char) (stoi(tokens.at(i))));
                }
                return tempDelimiter;
            } else {
            // OR  no need to process if it is just a string delimiter
                return msgDelimiter;
            }
        }

        inline static std::string toDecimalCSV(const std::string& str) {
            char buffer[33];
            std::string result;
            for (const char c : str) {
                sprintf(buffer, "%d,", c);
                result += buffer;
            }

            // remove the last comma ','
            return result.substr(0, result.length()-1);
        }

        inline static uint32_t stou(std::string const& str) {
            uint64_t result = std::stoul(str);
            if (result > std::numeric_limits<uint32_t>::max()) {
                throw std::out_of_range("out_of_range in stou");
            }
            return result;
        }
    };

}} // namespace lightmq { namespace qmapi {


#endif /* STRINGUTIL_H_ */
