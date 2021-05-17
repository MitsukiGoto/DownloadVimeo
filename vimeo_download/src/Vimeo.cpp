//
//  VimeoParse.cpp
//  vimeo_download
//
//  Created by MitsukiGoto on 2020/05/16.
//  Copyright © 2020 MitsukiGoto. All rights reserved.
//

#include "Vimeo.hpp"
#include "Requests.hpp"
#include "../include/cppcodec/base64_rfc4648.hpp"
#include "../include/Utils.hpp"
#include "../include/Colors.hpp"
#include <regex>
#include <tuple>
#include <filesystem>
#include <chrono>
#include <future>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>

Vimeo::Vimeo(const std::string &output_name, const std::string &url, std::unique_ptr<JSON> json, bool isVerbose, int progress_limit) : url(url), output_name(output_name), isVerbose(isVerbose), progress_limit(progress_limit)
{
    this->json = std::move(json);
    this->base_url = Utils::getBaseUrl(url, this->json->v.get<picojson::object>()["base_url"].to_str());
#if defined(__MACH__) || defined(__linux)
    this->home_dir = std::string(std::getenv("HOME"));
#endif
#ifdef _WIN64
    this->home_dir = Utils::get_enviroment("USERPROFILE");
#endif
    if (this->isVerbose)
    {
        std::cout << base_url << std::endl;
    }
    auto paths = createDirectory();
    this->tmp_dir = std::get<0>(paths);
    this->save_dir = std::get<1>(paths);
    if (this->output_name.find(".mp4") == std::string::npos)
    {
        if (this->isVerbose)
        {
            std::cout << "cannot find .mp4 with name" << std::endl;
        }
        this->output_name += ".mp4";
    }
    std::cout << "Output file name will be: " << this->output_name << std::endl;
}

template <typename T, typename U>
auto Vimeo::command(T tmp, U name)
{
    auto command = "ffmpeg -i " + tmp + "a.mp3 " + "-i " + tmp + "v.mp4" + " -acodec" + " copy" + " -vcodec" + " copy " + this->home_dir
#if defined(__MACH__) || defined(__linux)
                   + "/Desktop/Vimeo/" + name;
#endif
#ifdef _WIN64
    +"\\Desktop\\Vimeo/" + name;
#endif
    return command;
}

void Vimeo::merge()
{
#if defined(__MACH__) || defined(__linux) || defined(_WIN64)
    auto command_ = command(this->tmp_dir, this->output_name);
    if (!this->isVerbose)
    {
#if defined(__MACH__) || defined(__linux) || defined(_WIN64)
        command_ += " -loglevel quiet";
#endif
    }
    else
    {
        std::cout << command_ << std::endl;
    }
    std::system(command_.c_str());
    std::cout << Utils::Colors::MAGENTA
              << "Merging Audio and Video has successflly done" << std::endl;
#if defined(__MACH__) || defined(__linux)
    std::cout << "The Video was saved to: " << this->home_dir << "/Desktop/Vimeo/" << this->output_name << std::endl;
#endif
#ifdef _WIN64
    std::cout << "The Video was saved to: " << this->home_dir << "\\Desktop\\Vimeo\\" << this->output_name << std::endl;
#endif
    std::filesystem::remove_all(this->tmp_dir);
    std::cout << "Cleaning Process Started" << std::endl;
    std::cout << Utils::Colors::RED
              << "Temp Folder has removed: " << this->tmp_dir << std::endl;
    std::cout << "Cleaning Process End"
              << Utils::Colors::RESET << std::endl;
#endif
}

Vimeo &Vimeo::download()
{
#if defined(__MACH__) || defined(__linux) || defined(_WIN64)
    // #ifdef DEBUGING
    auto process1 = std::thread([this]
                                { this->downloadVideo(); });
    auto process2 = std::thread([this]
                                { this->downloadAudio(); });
    process1.join();
    process2.join();
#else
    this->downloadVideo();
    this->downloadAudio();
#endif
    std::cout << "\n"
              << Utils::Colors::CYAN
              << "Downloading Video&Audio has successflly done"
              << Utils::Colors::RESET << std::endl;
    return *this;
}

std::tuple<std::string, std::string> Vimeo::createDirectory()
{
    auto now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << now_c;
#if defined(__MACH__) || defined(__linux)
    std::string tmp = std::filesystem::temp_directory_path().string() + "/dlvimeo/" + ss.str() + "/";
    std::string saved = this->home_dir + "/Desktop/Vimeo";
#endif
#ifdef _WIN64
    std::string tmp = std::filesystem::temp_directory_path().string() + "\\dlvimeo\\" + ss.str() + "\\";
    std::string saved = this->home_dir + "\\Desktop\\Vimeo";
#endif
    std::filesystem::create_directories(tmp);
    std::filesystem::create_directories(saved);
    std::tuple<std::string, std::string> paths = {tmp, saved};
    return paths;
}

void Vimeo::downloadVideo()
{
    picojson::object highest_video = getHighestQualityVideoPartOfJson();
    std::string video_base_url = this->base_url + highest_video.at("base_url").to_str();
    if (this->isVerbose)
    {
        std::cout << "video_base_url: " << video_base_url << std::endl;
    }
    downloadSegmentAndMerge(highest_video, video_base_url, "video");
    if (this->isVerbose)
    {
        std::cout << Utils::Colors::CYAN
                  << "Downloading Video has successflly done"
                  << Utils::Colors::RESET << std::endl;
        std::cout << "Only Video at: " << this->tmp_dir + "v.mp4" << std::endl;
    }
}

void Vimeo::downloadAudio()
{
    auto audio = json->v.get<picojson::object>()["audio"].get<picojson::array>()[0].get<picojson::object>();
    std::string audio_base_url = this->base_url + audio.at("base_url").to_str();
    if (this->isVerbose)
    {
        std::cout << audio.at("base_url") << std::endl;
        std::cout << "audio_base_url: " << audio_base_url << std::endl;
    }
    downloadSegmentAndMerge(audio, audio_base_url, "audio");
    if (this->isVerbose)
    {
        std::cout << Utils::Colors::GREEN
                  << "Downloading Audio has successflly done"
                  << Utils::Colors::RESET << std::endl;
        std::cout << "Only Audio at: " << this->tmp_dir + "v.mp4" << std::endl;
    }
}

picojson::object Vimeo::getHighestQualityVideoPartOfJson()
{
    auto video = json->v.get<picojson::object>()["video"].get<picojson::array>();
    int i = 0;
    // The highest quality video has most largest "height" value
    std::vector<std::tuple<int, int>> heights;
    for (auto &content : video)
    {
        std::string height = content.get<picojson::object>()["height"].to_str();
        std::tuple<int, int> t = {std::stoi(height), i};
        heights.push_back(t);
        i++;
    }
    std::sort(std::begin(heights), std::end(heights), std::greater<std::tuple<int, int>>());
    return video[std::get<1>(heights[0])].get<picojson::object>();
}

void Vimeo::downloadSegmentAndMerge(picojson::object &obj, std::string base_url, std::string mode)
{
    std::string tmpFileDir;
    if (Utils::isVideo(mode))
    {
        tmpFileDir = this->tmp_dir + "v.mp4";
    }
    else if (Utils::isAudio(mode))
    {
        tmpFileDir = this->tmp_dir + "a.mp3";
    }
    else
    {
        std::cerr << "Unknown mode argument were given.";
        std::exit(1);
    }
    decodeInitSegmentAndMerge(obj, tmpFileDir);
    auto array = obj.at("segments").get<picojson::array>();
    int array_size = array.size();
    bool overLimit = false;
    if (array_size >= progress_limit)
    {
        overLimit = true;
        if (isVerbose)
        {
            std::cout << "element size is larger than limit" << std::endl;
        }
    }
    for (int index = 0; auto &segment : array)
    {
        if (Utils::isVideo(mode) && overLimit == false)
        {
            std::cout << "\r"
                      << "Progress: "
                      << "["
                      << Utils::Colors::RED
                      << std::string(index, '.') << std::string(array_size - index - 1, ' ') << Utils::Colors::RESET
                      << "]" << std::flush;
        }
        else if (Utils::isVideo(mode) && overLimit == true)
        {
            std::cout << "\r"
                      << "Progress: "
                      << index << " / " << array_size << std::flush;
        }
        std::string segment_url = base_url + segment.get<picojson::object>()["url"].to_str();
        Requests::get(segment_url, tmpFileDir);
        if(Utils::isVideo(mode) && index == array_size -1) {
            std::cout << "\r" << std::string(20, ' ') << "\r"
                      << "Progress: "
                      << "Done!" << std::flush;
        }
        index++;
    }
}

void Vimeo::decodeInitSegmentAndMerge(const picojson::object &obj, std::string tmpFileDir)
{
    std::ofstream ofs(tmpFileDir, std::ios::out | std::ios::binary);
    std::stringstream s;
    auto decoded = Utils::decode(obj.at("init_segment").to_str());
    std::move(decoded.begin(), decoded.end(), std::ostream_iterator<unsigned char>(s));
    ofs << s.rdbuf();
    ofs.close();
}
