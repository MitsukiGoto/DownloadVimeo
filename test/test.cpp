#include <iostream>
#include "../vimeo_download/include/Vimeo.hpp"
#include <sstream>
#include <vector>
#include <string>

std::vector<std::string> split(std::string str, std::string split_str)
{
    if (split_str == "")
        return {str};
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

int main()
{
    auto vec = split("https://135vod-adaptive.akamaized.net/exp=1619304805~acl=%2F2399b639-f634-4619-8617-b8104ebc4476%2F%2A~hmac=463dcebe095526a361192018b9f765d57aec0b33257a23422d6f85e0a242e200/2399b639-f634-4619-8617-b8104ebc4476/sep/video/93604f68,0981b75b,f3ece05a/audio/3a7faaae,5b71e844/master.json?query_string_ranges=1&base64_init=1","/");
    for(auto v:vec) {
        std::cout << v << std::endl;
    }
    return 0;
}