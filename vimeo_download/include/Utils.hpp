#include <iostream>
#include <string>
#include <vector>
#include "cppcodec/base64_rfc4648.hpp"

namespace Utils
{
    inline auto decode(std::string encodedStr)
    {
        return cppcodec::base64_rfc4648::decode(encodedStr);
    }

    inline std::vector<std::string> splitBySlash(std::string str)
    {
        const std::string split_str = "/";
        std::vector<std::string> result;
        std::string tstr = str + split_str;
        unsigned long l = tstr.length(), sl = split_str.length();
        std::string::size_type pos = 0, prev = 0;

        for (; pos < l && (pos = tstr.find(split_str, pos)) != std::string::npos; prev = (pos += sl))
        {
            result.emplace_back(tstr, prev, pos - prev);
        }
        return result;
    }

    inline auto vecToString(const std::vector<std::string> &vec)
    {
        std::string str;
        for (const auto &elm : vec)
        {
            if (str != "")
            {
                str = str + "/" + elm;
            }
            else
            {
                str = elm;
            }
        }
        return str += "/";
    }

    inline auto getBaseUrl(std::string base_url, std::string relative)
    {
        auto splitted = splitBySlash(base_url);
        splitted.erase(splitted.end() - 1);
        for (auto &content : splitBySlash(relative))
        {
            if (content == "..")
            {
                splitted.erase(splitted.end() - 1);
            }
        }
        return vecToString(splitted);
    }
#ifdef _WIN64
    std::string get_enviroment(std::string name)
    {
        size_t buf;
        char buffer[128];
        if (getenv_s(&buf, buffer, 128, name.c_str()))
            throw std::runtime_error("read error");
        if (buf == 0)
            throw std::runtime_error("not such environment variable");
        return std::string(buffer);
    }
#endif
    bool isVideo(std::string mode)
    {
        if (mode == "video")
            return true;
        return false;
    }
    bool isAudio(std::string mode)
    {
        if (mode == "audio")
            return true;
        return false;
    }
}