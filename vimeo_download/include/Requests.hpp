//
//  Requests.hpp
//  vimeo_download
//
//  Created by MitsukiGoto on 2020/05/16.
//  Copyright © 2020 MitsukiGoto. All rights reserved.
//

#ifndef Requests_hpp
#define Requests_hpp

#include <iostream>
#include <string>
#include <memory>
#include "JSON.hpp"

namespace Requests {
    std::unique_ptr<JSON> get(const std::string&);
    void get(std::string&, const std::string&);
}

#endif /* Requests_hpp */