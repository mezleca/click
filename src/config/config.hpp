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
    bool save();
}

extern ConfigData config;
