//
//  main.cpp
//  vimeo_download
//
//  Created by MitsukiGoto on 2020/05/12.
//  Copyright © 2020 MitsukiGoto. All rights reserved.
//

#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>
#include <boost/program_options.hpp>
#include <tuple>
#include "include/Vimeo.hpp"
#include "include/model/parsedArg.hpp"
#include "Requests.hpp"

namespace
{
    auto parseArgument(int argc, const char **argv)
    {
        std::tuple<std::string, std::string, bool, bool> options;
        boost::program_options::options_description description("Download Vimeo Video even though it is set Private \n"
                                                                "Usage: vimeo_download [-u URL] [-o Output]");
        description.add_options()("url,u", boost::program_options::value<std::string>(), "Set Target URL")("output,o", boost::program_options::value<std::string>(), "Set output name")("continue,c", "Other Download")("help,h", "Help")("verbose", "Turn on Verbose Mode")("version", "Show Version");

        boost::program_options::variables_map vm;
        store(boost::program_options::parse_command_line(argc, argv, description), vm);
        notify(vm);

        if (vm.count("help"))
        {
            std::cout << description << std::endl;
            std::exit(0);
        }
        if (vm.count("version"))
        {
            std::cout << "Download Vimeo (v2.0)" << std::endl;
            std::exit(0);
        }
        if (!vm.count("url") || !vm.count("output"))
        {
            std::cout << "\e[36m"
                      << "\n";
            std::cerr << "Required URL or OUTPUT Option" << std::endl;
            std::cout << "\e[m"
                      << "\n";
            std::exit(1);
        }
        std::get<0>(options) = vm["output"].as<std::string>();
        std::get<1>(options) = vm["url"].as<std::string>();
        if (vm.count("verbose"))
        {
            std::get<2>(options) = true;
        }
        else
        {
            std::get<2>(options) = false;
        }
        auto parsedArg = std::make_unique<ParsedArg>(std::get<0>(options), std::get<1>(options), std::get<2>(options));
        return parsedArg;
    }
} // namespace

int main(int argc, const char *argv[])
{
    //     Expected: output, url
    auto arg = parseArgument(argc, argv);
    if (arg->isVerbose)
        std::cout << "Verbose Mode on" << std::endl;
    auto vimeo = std::make_unique<Vimeo>(arg->outputName, arg->url, Requests::get(arg->url), arg->isVerbose);
    vimeo->download().merge();
#ifdef __APPLE__
    std::system("osascript -e 'display notification \"Finish\"'");
#endif
    return 0;
}
