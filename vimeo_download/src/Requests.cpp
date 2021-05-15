//
//  Requests.cpp
//  vimeo_download
//
//  Created by MitsukiGoto on 2020/05/16.
//  Copyright © 2020 MitsukiGoto. All rights reserved.
//

#include "Requests.hpp"
#include <curl/curl.h>
#include <fstream>
#include <memory>

void callCurl(CURL *curl, const std::string &url)
{
    std::string response;
    long status_code = 0;
    CURLcode ret;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    ret = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    if (status_code != 200)
    {
        std::cout << "ERROR: HTTP Status Code is " << status_code << std::endl;
        curl = nullptr;
        std::exit(1);
    }
    curl_easy_cleanup(curl);
    curl = nullptr;
    return;
}

std::unique_ptr<JSON> Requests::get(const std::string &url)
{
    CURL *curl;
    std::string response;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +([](char *ptr, std::size_t size, std::size_t nmemb, std::string *stream) {
                         int dataLength = static_cast<int>(size * nmemb);
                         stream->append(ptr, dataLength);
                         return dataLength;
                     }));
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    callCurl(curl, url);
    auto json = std::make_unique<JSON>(response);
    return json;
}

void Requests::get(std::string &url, const std::string &path)
{
    std::ofstream output(path, std::ios::binary | std::ios::app);
    CURL *curl;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +([](char *ptr, std::size_t size, std::size_t nmemb, void *user_data) {
                         std::ofstream *out = static_cast<std::ofstream *>(user_data);
                         size_t nbytes = size * nmemb;
                         out->write(ptr, nbytes);
                         return nbytes;
                     }));
    curl_easy_setopt(curl, CURLOPT_FILE, &output);
    callCurl(curl, url);
}
