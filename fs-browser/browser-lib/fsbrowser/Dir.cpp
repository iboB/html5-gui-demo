// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include "Dir.hpp"

#if defined(_WIN32)
#   include <cstdlib>
#   include "win_dirent.h"
#else
#   include <dirent.h>
#endif

#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

namespace fsbrowser
{

namespace
{
void normalizePath(std::string& path)
{
    for (auto& c : path)
        if (c == '\\')
            c = '/';
}
}

Dir::Dir(const std::string& basePath)
    : m_basePath(basePath)
{}

Dir::~Dir() = default;

Dir::DirectoryContents Dir::getDirectoryContents(const char* path) const
{
    DirectoryContents ret;

    auto dir = m_basePath + path;
    auto dp = opendir(dir.c_str());

    dirent* entry;
    while ((entry = readdir(dp)) != nullptr)
    {
        if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) continue; // don't care for this
        auto fullPath = dir + "/" + entry->d_name;
        struct stat info;
        if (stat(fullPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR))
        {
            ret.dirs.emplace_back(entry->d_name);
        }
        else
        {
            ret.files.emplace_back(entry->d_name);
        }
    }

    closedir(dp);

    return ret;
}

}
