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

std::unique_ptr<JSON> Requests::get(const std::string& url) {
    std::string response;
    long status_code = 0;
    CURL* curl;
    CURLcode ret;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +([](char* ptr, std::size_t size, std::size_t nmemb, std::string* stream){
        int dataLength = static_cast<int>(size*nmemb);
        stream->append(ptr, dataLength);
        return dataLength;
    }));
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    ret = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    if(status_code == 200) {
        curl_easy_cleanup(curl);
        curl = nullptr;
        auto json = std::make_unique<JSON>(response);
        return json;
    }
    std::cout << "ERROR: HTTP Status Code is " << status_code <<  std::endl;
    curl = nullptr;
    std::exit(1);
}

void Requests::get(std::string& url, const std::string& path) {
    long status_code = 0;
    std::ofstream output(path, std::ios::binary|std::ios::app);
    CURL* curl;
    CURLcode ret;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +([](char* ptr, std::size_t size, std::size_t nmemb, void* user_data){
        std::ofstream *out = static_cast<std::ofstream *>(user_data);
        size_t nbytes = size * nmemb;
        out->write(ptr, nbytes);
        return nbytes;
    }));
    curl_easy_setopt(curl, CURLOPT_FILE, &output);
    ret = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    if(status_code == 200) {
        curl_easy_cleanup(curl);
        curl = nullptr;
        return;
    }
    std::cout << "ERROR: HTTP Status Code is " << status_code <<  std::endl;
    curl = nullptr;
    std::exit(1);
}
