// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include "FSBrowser.hpp"
#include "Dir.hpp"
#include <helper/DirUtil.hpp>

#include <cassert>
#include <sstream>

namespace fsbrowser
{

FSBrowser::FSBrowser()
{
    auto exePath = DirUtil::getCurrentExecutablePath();
    auto repoPath = DirUtil::getAssetPath(exePath, "html");

    if (!repoPath.empty()) {
        assert(repoPath.size() >= 5);
        // remove /html
        repoPath = repoPath.substr(0, repoPath.size() - 5);
    }

    m_dir.reset(new Dir(repoPath));
}

FSBrowser::~FSBrowser() = default;

std::string FSBrowser::onRequest(const std::string& request)
{
    const char* err = R"json({"type":"Error","msg":"Unknown request"})json";
    std::string r = request;
    auto f = r.find(':');
    if (f == std::string::npos) return err;
    auto rtype = r.substr(0, f);
    if (rtype != "contents") return err;
    auto rarg = r.substr(f + 1);
    return dir(rarg);
}

std::string FSBrowser::dir(const std::string& path) const
{
    auto contents = m_dir->getDirectoryContents(path.c_str());

    std::ostringstream json;
    json << "{";
    json << R"json("type":"Contents","path":")json";
    json << path;
    json << R"json(","dirs":[)json";
    for (size_t i = 0; i < contents.dirs.size(); ++i) {
        if (i != 0) json << ',';
        json << '"' << contents.dirs[i] << '"';
    }
    json << R"json(],"files":[)json";
    for (size_t i = 0; i < contents.files.size(); ++i) {
        if (i != 0) json << ',';
        json << '"' << contents.files[i] << '"';
    }
    json << R"json(])json";
    json << "}";

    return json.str();
}


}
