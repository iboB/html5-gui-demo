// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

#include <string>
#include <memory>

namespace fsbrowser
{

class Dir;

class FSBrowser
{
public:
    FSBrowser();
    ~FSBrowser();

    // process a request according to the format in fs-browser.html
    // returns the response
    std::string onRequest(const std::string& request);

    // just a proxy to Dir::getDirectoryContents which converts the output to 
    // a JSON string corresponding to the format accepted by fs-browser.html
    std::string dir(const std::string& path) const;

private:
    std::unique_ptr<Dir> m_dir;
};

}
