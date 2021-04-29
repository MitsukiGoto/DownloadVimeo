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

Vimeo::Vimeo(const std::string &output_name, const std::string &url, std::unique_ptr<JSON> json, bool isVerbose) : url(url), output_name(output_name), isVerbose(isVerbose)
{
    this->json = std::move(json);
    this->base_url = Utils::getBaseUrl(url, this->json->v.get<picojson::object>()["base_url"].to_str());
    this->home_dir = std::string(std::getenv("HOME"));
    std::cout << base_url << std::endl;
    auto paths = createDirectory();
    this->tmp_dir = std::get<0>(paths);
    this->save_dir = std::get<1>(paths);
    std::cout << output_name << std::endl;
    if (this->output_name.find(".mp4") == std::string::npos)
    {
        if (this->isVerbose)
        {
            std::cout << "cannot find .mp4 with name" << std::endl;
        }
        this->output_name += ".mp4";
    }
}

template <typename T, typename U>
auto Vimeo::command(T tmp, U name)
{
    auto command = "ffmpeg -i " + tmp + "a.mp3 " + "-i " + tmp + "v.mp4" + " -acodec" + " copy" + " -vcodec" + " copy " + this->home_dir + "/Desktop/Vimeo/" + name;
    return command;
}

void Vimeo::merge()
{
#if defined(__MACH__) || defined(__linux)
    auto command_ = command(this->tmp_dir, this->output_name);
    if (!this->isVerbose)
    {
        command_ += " >/dev/null 2>&1";
    }
    else
    {
        std::cout << command_ << std::endl;
    }
    std::system(command_.c_str());
    std::cout << "\u001b[35m"
              << "Merging Audio and Video has successflly done" << std::endl;
    std::cout << "The Video was saved to:" << this->home_dir << "/Desktop/Vimeo/" << this->output_name << std::endl;
    std::filesystem::remove_all(this->tmp_dir);
    std::cout << "Cleaning Process Started" << std::endl;
    std::cout << "\u001b[31m"
              << "Temp Folder has removed: " << this->tmp_dir << std::endl;
    std::cout << "Cleaning Process End"
              << "\u001b[0m" << std::endl;
#endif
#ifdef __WIN64
    std::cout << "Sorry, Merging Audio and Video doesn't support on Windows now" << std::endl;
#endif
}

Vimeo &Vimeo::download()
{
#if defined(__MACH__) || defined(__linux)
    // #ifdef DEBUGING
    auto process1 = std::thread([this] { this->downloadVideo(); });
    auto process2 = std::thread([this] { this->downloadAudio(); });
    process1.join();
    process2.join();
#else
    this->downloadVideo();
    this->downloadAudio();
#endif
    return *this;
}

std::tuple<std::string, std::string> Vimeo::createDirectory() {
    auto now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << now_c;
    std::string tmp = std::filesystem::temp_directory_path().string() + "/dlvimeo/" + ss.str() + "/";
    std::filesystem::create_directories(tmp);
    std::string saved = this->home_dir + "/Desktop/Vimeo";
    std::filesystem::create_directories(saved);
    std::tuple<std::string, std::string> paths = {tmp, saved};
    return paths;
}

void Vimeo::downloadVideo()
{
    auto video = json->v.get<picojson::object>()["video"].get<picojson::array>();
    int i = 0;
    // The most high quality video has most largest "height" value
    std::vector<std::tuple<int, int>> heights;
    for (auto &content : video)
    {
        std::string height = content.get<picojson::object>()["height"].to_str();
        std::tuple<int, int> t = {std::stoi(height), i};
        heights.push_back(t);
        i++;
    }
    std::sort(std::begin(heights), std::end(heights), std::greater<std::tuple<int, int>>());
    picojson::object highest_video = video[std::get<1>(heights[0])].get<picojson::object>();
    std::string video_base_url = this->base_url + highest_video.at("base_url").to_str();
    if (this->isVerbose)
    {
        std::cout << "video_base_url: " << video_base_url << std::endl;
    }
    auto decoded = Utils::decode(highest_video.at("init_segment").to_str());
    std::ofstream ofs(this->tmp_dir + "v.mp4", std::ios::out | std::ios::binary);
    std::stringstream s;
    std::move(decoded.begin(), decoded.end(), std::ostream_iterator<unsigned char>(s));
    ofs << s.rdbuf();
    ofs.close();
    for (auto &segment : highest_video.at("segments").get<picojson::array>())
    {
        std::string segment_url = video_base_url + segment.get<picojson::object>()["url"].to_str();
        Requests::get(segment_url, this->tmp_dir + "v.mp4");
    }
    std::cout << "\u001b[36m"
              << "Downloading Video has successflly done"
              << "\u001b[0m" << std::endl;
    if (this->isVerbose)
    {
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
    auto decoded = Utils::decode(audio.at("init_segment").to_str());
    std::ofstream ofs(this->tmp_dir + "a.mp3", std::ios::out | std::ios::binary);
    std::stringstream s;
    std::move(decoded.begin(), decoded.end(), std::ostream_iterator<unsigned char>(s));
    ofs << s.rdbuf();
    ofs.close();
    for (auto &segment : audio.at("segments").get<picojson::array>())
    {
        std::string segment_url = audio_base_url + segment.get<picojson::object>()["url"].to_str();
        Requests::get(segment_url, this->tmp_dir + "a.mp3");
    }
    std::cout << "\u001b[32m"
              << "Downloading Audio has successflly done"
              << "\u001b[0m" << std::endl;
    if (this->isVerbose)
    {
        std::cout << "Only Audio at: " << this->tmp_dir + "v.mp4" << std::endl;
    }
}

// void Vimeo::downloadSegmentAndMerge(picojson::object obj, std::string mode)
// {
//     for (auto &segment : obj.at("segments").get<picojson::array>())
//     {
//         std::string segment_url = video_base_url + segment.get<picojson::object>()["url"].to_str();
//         Requests::get(segment_url, this->tmp_dir + tmpFileName);
//     }
// }
