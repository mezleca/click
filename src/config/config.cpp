#include "config.hpp"

std::string default_config_content =
R"({
    "keys": [],
    "randomized": false
}
)";

bool Config::file_exists(std::string name) {

    if (FILE* file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }

    return false;
}

void Config::create_file(std::string name, std::string content = "") {
    std::cout << "creating default config file" << "\n";
    std::ofstream file(name);
    file << content;
    file.close();
}

void Config::initialize() {
    
    std::string file_name = "config.json";
    KeyData default_key;

    if (!file_exists(file_name)) {          
        create_file(file_name, default_config_content);
        config.keys.push_back(default_key);
    }

    std::ifstream config_file(file_name);

    // check if the config file exists, if not create a placeholder one
    if (!config_file.is_open()) {

        printf("creating new config\n");

        std::ofstream new_config("./config.json");
        new_config << default_config_content;
        config.keys.push_back(default_key);
        new_config.close();
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    std::string errs;

    bool ok = Json::parseFromStream(builder, config_file, &root, &errs);

    if (!ok) {
        printf("failed to parse config file\nmake sure the json file is valid\n");
        return;
    }

    const Json::Value& keys = root["keys"];

    // make sure is a valid array
    if (!keys.isArray()) {
        printf("failed to get keys array\nusing the default one\n");
        config.keys.push_back(default_key);
        return;
    }

    // add the keys to the config struct
    for (unsigned int i = 0; i < keys.size(); i++) {
        
        const Json::Value key = keys[i];

        if (!key.isMember("trigger") || !key.isMember("target") || !key.isMember("cps")) {
            printf("invalid schema for key %i\n", i);
            continue;
        }

        int trigger_keycode = key["trigger"].asInt();
        int target_keycode  = key["target"].asInt();
        int cps_count       = key["cps"].asInt();
        bool randomized      = key["randomized"].asBool();

        if (trigger_keycode < KeyList::NOT_SET || target_keycode > KeyList::MAX_VALUE) {
            printf("Invalid keycode for key %i\n", i);
            continue;
        }
        
        printf("[%i] trigger: %i | target: %i | cps: %i\n", i, trigger_keycode, target_keycode, cps_count);

        config.keys.push_back({
            trigger: (KeyList)trigger_keycode,
            target: (KeyList)target_keycode,
            cps: cps_count,
            randomized: randomized
        });
    }

    printf("intialized config\n");
}

bool Config::save() {

    Json::Value root;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "\t";

    Json::Value keys(Json::arrayValue);

    for (size_t i = 0; i < config.keys.size(); i++) {

        KeyData current_key = config.keys.at(i);
        Json::Value key(Json::objectValue);

        key["trigger"] = current_key.trigger;
        key["target"] = current_key.target;
        key["cps"] = current_key.cps;
        key["randomized"] = current_key.randomized;

        keys.append(key);
    }

    root["keys"] = keys;

    std::string config_content = Json::writeString(wbuilder, root);
    create_file("config.json", config_content);

    return true;
}
