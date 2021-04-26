#include <iostream>
#include "../vimeo_download/include/Vimeo.hpp"
#include <sstream>
#include <vector>
#include <string>
#include <regex>

template <typename T>
void print(T t)
{
    std::cout << t << std::endl;
}

std::vector<std::string> splitBySlash(std::string str)
{
    const std::string split_str = "/";
    std::vector<std::string> result;
    std::string tstr = str + split_str;
    long l = tstr.length(), sl = split_str.length();
    std::string::size_type pos = 0, prev = 0;

    for (; pos < l && (pos = tstr.find(split_str, pos)) != std::string::npos; prev = (pos += sl))
    {
        result.emplace_back(tstr, prev, pos - prev);
    }
    return result;
}

auto vecToString(const std::vector<std::string> &vec)
{
    std::string str;
    for (const auto &elm : vec)
    {
        if (str != "")
        {
            str = str + "/" + elm;
        } else {
            str = elm;
        }
    }
    return str+="/";
}

auto getBaseUrl(std::string base_url, std::string relative)
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

int main()
{
    print(getBaseUrl("https://135vod-adaptive.akamaized.net/exp=1619426953~acl=%2F2399b639-f634-4619-8617-b8104ebc4476%2F%2A~hmac=246cada37c386de2082e26a3778af986cf3cbf3c47de166c0a417e6b665ccd91/2399b639-f634-4619-8617-b8104ebc4476/sep/video/93604f68,0981b75b,f3ece05a/audio/5b71e844,3a7faaae/master.json?query_string_ranges=1&base64_init=1", "../../../"));
    return 0;
}