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

class Vimeo {
public:
    Vimeo(const std::string&, const std::string&, std::unique_ptr<JSON>, bool verbose);
    Vimeo() = delete;
    Vimeo& download();
    void merge();
private:
    void downloadVideo();
    void downloadAudio();
    std::unique_ptr<JSON> json;
    std::string url;
    std::string base_url;
    std::string tmp_dir;
    std::string output_name;
    bool verbose;
};

#endif /* VimeoParse_hpp */
