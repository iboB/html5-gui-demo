// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

#include <string>

class DirUtil
{
public:
    // get the full path to the current executable
    static std::string getCurrentExecutablePath();

    // starts looking from the current directory upwards until it discovers a valid subdirectory described by assetDir
    // for example
    // getAssetPath("/home/someuser/projects/xxx/build/bin", "assets"); will return /home/someuser/projects/xxx/assets if this directory exists
    static std::string getAssetPath(std::string baseDir, const std::string& assetDir);
};
