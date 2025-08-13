#include <bravo/bravo.hpp>

using namespace brv;

ConfigContext *brv::processConfigFile(const CmdContext *cctx, const fs::path &root) {
    ConfigContext *cfg = new ConfigContext();

    if (cctx->cmd.skip_config)
        return cfg;

    cfg->root = root;

    config::load(cfg);

    config::validate(cfg, cctx);

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

    BRV_ASSERT(file::isfile(file), "Project must contain a 'bravo.json' config file.");

    jltt::Parser *parser = new jltt::Parser(file);

    BRV_ASSERT(parser->state() == jltt::STATE_OPEN, "Failed to open config file.");

    parser->start();

    BRV_ASSERT(parser->state() == jltt::STATE_SUCCESS, "Failed to parse config file.");
    BRV_ASSERT(parser->root()->type == jltt::JType::OBJECT, "Invalid config file structure.");

    jltt::JValue *json = parser->root();

    delete parser;
    return json;
}

std::string config::getString(jltt::JValue *json, const jltt::JString &key) {
    jltt::JValue *val = json->at(key);

    BRV_ASSERT(val != nullptr, "Missing value '", key, "'." );
    BRV_ASSERT(val->is<jltt::JString>(), "Value '", key, "' must be of type 'string'" );

    return *val->as<jltt::JString>();
}

std::optional<std::string> config::getOptString(jltt::JValue *json, const jltt::JString &key) {
    jltt::JValue *val = json->at(key);

    if (val == nullptr) return {};
    BRV_ASSERT(val->is<jltt::JString>(), "Value '", key, "' must be of type 'string'" );

    return *val->as<jltt::JString>();
}

std::vector<fs::path> config::getPathVec(jltt::JValue *json, const jltt::JString &key) {
    jltt::JValue *val = json->at(key);

    if (val == nullptr) return {};
    BRV_ASSERT(val->is<jltt::JArray>(), "Value '", key, "' must be of type 'array'" );

    std::vector<fs::path> paths;
    for (jltt::JValue *element: *val->as<jltt::JArray>()) {
        BRV_ASSERT(element->is<jltt::JString>(), "Values of '", key, "' array must be of type 'string'" );

        paths.push_back(*element->as<jltt::JString>());
    }

    return paths;
}
