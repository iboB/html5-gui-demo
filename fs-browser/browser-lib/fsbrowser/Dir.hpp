// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

#include <vector>
#include <string>

namespace fsbrowser
{

class Dir
{
public:
    Dir(const std::string& basePath);
    ~Dir();

    struct DirectoryContents
    {
        std::vector<std::string> files;
        std::vector<std::string> dirs;
    };

    // path will be interpreted as relative to base path
    DirectoryContents getDirectoryContents(const char* path) const;

private:
    std::string m_basePath;
};

}
