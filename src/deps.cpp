#include <bravo/bravo.hpp>

using namespace brv;

BuildContext *brv::processDeps(const ConfigContext *cfg, CmdContext *cctx) {
    BuildContext *bctx = new BuildContext();

    if (cctx->cmd.skip_config) {
        return bctx;
    }

    deps::scanProject(bctx, cfg);

    deps::scanDeps(cfg, cctx);

    return bctx;
}

void deps::scanProject(BuildContext *bctx, const ConfigContext *cfg) {
    fs::path src = cfg->root;
    src /= BRV_DIR_SRC;
    bctx->src_dir = src;
    BRV_ASSERT(file::isdir(src), "Project must contain a 'src' directory.");

    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(src)) {
        if (!entry.is_regular_file()) continue;

        fs::path ext = entry.path().extension();
        if (ext == BRV_FILE_EXT_CPP)
            bctx->src_files.emplace_back(fs::absolute(entry.path()));
    }

    fs::path bin = cfg->root;
    bin /= BRV_DIR_BIN;
    bctx->bin_dir = bin;
    bin /= cfg->project_name + (cfg->project_type == BRV_PROJECT_TYPE_EXEC ? BRV_FILE_EXT_EXE : BRV_FILE_EXT_ARCHIVE);
    bctx->end_dst = bin;

    fs::path obj = cfg->root;
    obj /= BRV_DIR_OBJ;
    bctx->obj_dir = obj;

    fs::path include = cfg->root;
    include /= BRV_DIR_INCLUDE;
    BRV_ASSERT(file::isdir(include), "Project must contain a 'include' directory.");

    bctx->include_dir = include;
    bctx->include_dirs.emplace_back(include);
}

void deps::scanDeps(const ConfigContext *cfg, CmdContext *cctx) {
    for (const fs::path &dep : cfg->deps) {

        bool skip = false;
        for (const ProjectContext *pctx : cctx->projects)
            if (dep == pctx->config->root) {
                skip = true;
                break;
            }
        if (skip) continue;

        ProjectContext *pctx = new ProjectContext();

        cctx->projects.emplace_back(pctx);

        pctx->config = processConfigFile(cctx, dep);

        pctx->build = processDeps(pctx->config, cctx);
    }
}
