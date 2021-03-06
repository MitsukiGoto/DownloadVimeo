//
//  ParsedArg.hpp
//  vimeo_download
//
//  Created by Mitsuki Goto on 2021/01/26.
//  Copyright © 2021 MitsukiGoto. All rights reserved.
//

#ifndef ParsedArg_hpp
#define ParsedArg_hpp

#include <iostream>

class ParsedArg {
public:
    ParsedArg(std::string outputName, std::string url, bool isVerbose, int progress_limit) : url(url), outputName(outputName), isVerbose(isVerbose), progress_limit(progress_limit) {}
    std::string url;
    std::string outputName;
    bool isVerbose;
    int progress_limit;
};

#endif /* ParsedArg_hpp */
