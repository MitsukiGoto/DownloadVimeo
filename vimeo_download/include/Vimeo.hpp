//
//  VimeoParse.hpp
//  vimeo_download
//
//  Created by MitsukiGoto on 2020/05/16.
//  Copyright © 2020 MitsukiGoto. All rights reserved.
//

#ifndef Vimeo_hpp
#define Vimeo_hpp

#include <iostream>
#include <memory>
#include <string>
#include "JSON.hpp"

class Vimeo
{
public:
    Vimeo(const std::string &, const std::string &, std::unique_ptr<JSON>, bool verbose, int progress_limit);
    Vimeo() = delete;
    Vimeo &download();
    void merge();

private:
    void downloadVideo();
    void downloadAudio();
    void downloadSegmentAndMerge(picojson::object& obj, std::string base_url, std::string mode);
    void decodeInitSegmentAndMerge(const picojson::object& obj, std::string tmpFileDir);
    std::tuple<std::string, std::string> createDirectory();
    picojson::object getHighestQualityVideoPartOfJson();
    std::unique_ptr<JSON> json;
    std::string url;
    std::string base_url;
    std::string tmp_dir;
    std::string save_dir;
    std::string home_dir;
    std::string output_name;
    bool isVerbose;
    int progress_limit;
    template <typename T, typename U>
    auto command(T tmp, U name);
};

#endif /* VimeoParse_hpp */