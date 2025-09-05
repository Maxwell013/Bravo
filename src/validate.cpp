#include <bravo/bravo.hpp>

#include <filesystem>

using namespace brv;

void config::validate(const ConfigContext *cfg, const CmdContext *cctx) {
    for (const Validation &validation : VALIDATION_MAP) {
        validation.call(cfg);
        BRV_CONDITIONAL(cctx->verbose, "Validating in project '", cfg->project_name, "': ", validation.name);
    }
}

void config::validateProjectName(const ConfigContext *cfg) {
    BRV_ASSERT(!cfg->project_name.empty(), "Project must have a non-empty name.");
}

void config::validateProjectType(const ConfigContext *cfg) {
    BRV_ASSERT(VALID_PROJECT_TYPES.contains(cfg->project_type), "Unkown project type specified.");
}

void config::validateEntry(const ConfigContext *cfg) {
    if (cfg->project_type == BRV_PROJECT_TYPE_EXEC)
        BRV_ASSERT(cfg->entry.has_value(), "Project of type 'exec' must specify 'entry' key.");
}

void config::validateDeps(const ConfigContext *cfg) {
    for (const fs::path &dep : cfg->deps)
        BRV_ASSERT(file::isdir(fs::absolute(dep)), "Dependecy paths must be valid and contain a 'bravo.json' config file.");
}
