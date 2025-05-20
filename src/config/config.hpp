#pragma once

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <fcntl.h>
#include <json/json.h>

#include "../input/input.hpp"
#include "../common.hpp"

struct ConfigData {
    std::vector<KeyData> keys;
};

namespace Config {
    void initialize();
    void create_file(std::string name, std::string content);
    bool save();
    bool file_exists(std::string name);
}

inline ConfigData config;
