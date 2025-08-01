#include <bravo/bravo.hpp>

using namespace brv;

void config::validate(const ConfigContext *cfg) {
    for (const Validation &validation : VALIDATION_MAP) {
        validation.call(cfg); // TODO : Lima log
    }
}
void config::validateProjectName(const ConfigContext *cfg) {
    if (cfg->project_name.empty()) {
        BRV_THROW("Project must have a non-empty name.");
    }
}

void config::validateProjectType(const ConfigContext *cfg) {
    for (const std::string& type : VALID_PROJECT_TYPES)
        if (type == cfg->project_type)
            return;
    BRV_THROW("Unkown project type specified.");
}

void config::validateEntry(const ConfigContext *cfg) {
    if (cfg->project_type == BRV_PROJECT_TYPE_EXEC)
        if (!cfg->entry.has_value()) {
            BRV_THROW("Project of type 'exec' must specify 'entry' key.");
        }
}

void config::validateDeps(const ConfigContext *cfg) {
    for (const fs::path &dep : cfg->deps)
        if (!file::isdir(fs::absolute(dep))) {
            BRV_THROW("Dependecy paths must be valid and contain a bravo.json config file.");
        }
}
