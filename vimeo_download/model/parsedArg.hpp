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
    ParsedArg(std::string outputName, std::string url, bool isVerbose) : outputName(outputName), url(url), isVerbose(isVerbose){}
    std::string url;
    std::string outputName;
    bool isVerbose;
};

#endif /* ParsedArg_hpp */
