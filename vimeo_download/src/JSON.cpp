//
//  JSON.cpp
//  vimeo_download
//
//  Created by MitsukiGoto on 2020/05/16.
//  Copyright © 2020 MitsukiGoto. All rights reserved.
//

#include "JSON.hpp"

JSON::JSON(std::string json) {
    std::string err = picojson::parse(this->v, json);
    if(!err.empty()) {
        std::cout << "ERROR: " << err << std::endl;
        std::exit(1);
    }
    std::cout << "JSON Parsed" << std::endl;
}
