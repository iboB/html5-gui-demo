// HTML 5 GUI Demo
// Copyright (c) 2019 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

#include <demo/App.hpp>

class DemoCefApp : public demo::App
{
    virtual demo::GUI* createGUI() override;
};