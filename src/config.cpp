#include <bravo/bravo.hpp>

using namespace brv;

ConfigContext *brv::processConfigFile(const CmdContext *cctx, const fs::path &root) {
    ConfigContext *cfg = new ConfigContext();

    if (cctx->cmd.skip_config)
        return cfg;

    cfg->root = root;

    config::load(cfg);

    config::validate(cfg);

    return cfg;
}

void config::load(ConfigContext *cfg) {
    jltt::JValue *json;

    json = read(cfg->root);

    cfg->project_name = getString(json, BRV_KEY_PROJECT_NAME);
    cfg->project_type = getString(json, BRV_KEY_PROJECT_TYPE);
    cfg->entry = getOptString(json, BRV_KEY_ENTRY);
    cfg->deps = getPathVec(json, BRV_KEY_DEPS);

    delete json;
}

jltt::JValue *config::read(const fs::path &root) {
    fs::path file = root;
    file /= BRV_FILE_NAME_CONFIG;

    if (!file::isfile(file)) {
        BRV_THROW("Project must contain a 'bravo.json' config file.");
    }

    jltt::Parser *parser = new jltt::Parser(file);

    if (parser->state() != jltt::STATE_OPEN) {
        BRV_THROW("Failed to open config file.");
    }

    parser->start();

    if (parser->state() != jltt::STATE_SUCCESS) {
        BRV_THROW("Failed to parse config file.");
    }

    if (parser->root()->type != jltt::JType::OBJECT) {
        BRV_THROW("Invalid config file structure.");
    }

    jltt::JValue *json = parser->root();

    delete parser;

    return json;
}

std::string config::getString(jltt::JValue *json, const jltt::JString &key) {
    jltt::JValue *val = json->at(key);

    if (val == nullptr) {
        BRV_THROW("Missing value '", key, "'.");
    }

    if (!val->is<jltt::JString>()) {
        BRV_THROW("Value '", key, "' must be of type 'string'");
    }

    return *val->as<jltt::JString>();
}

std::optional<std::string> config::getOptString(jltt::JValue *json, const jltt::JString &key) {
    jltt::JValue *val = json->at(key);

    if (val == nullptr) {
        return {};
    }

    if (!val->is<jltt::JString>()) {
        BRV_THROW("Value '", key, "' must be of type 'string'");
    }

    return *val->as<jltt::JString>();
}

std::vector<fs::path> config::getPathVec(jltt::JValue *json, const jltt::JString &key) {
    jltt::JValue *val = json->at(key);
    if (val == nullptr) {
        return {};
    }

    if (!val->is<jltt::JArray>()) {
        BRV_THROW("Value '", key, "' must be of type 'array'");
    }

    std::vector<fs::path> paths;

    for (jltt::JValue *element: *val->as<jltt::JArray>()) {
        if (!element->is<jltt::JString>()) {
            BRV_THROW("Values of '", key, "' array must be of type 'string'");
        }

        paths.push_back(*element->as<jltt::JString>());
    }

    return paths;
}
