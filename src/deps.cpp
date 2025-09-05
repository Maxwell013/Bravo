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
    fs::path root = cfg->root;

    bctx->bin_dir = root / BRV_DIR_BIN;
    bctx->include_dir = root / BRV_DIR_INCLUDE;
    bctx->obj_dir = root / BRV_DIR_OBJ;
    bctx->src_dir = root / BRV_DIR_SRC;
    bctx->test_dir = root / BRV_DIR_TEST;

    BRV_ASSERT(file::isdir(bctx->src_dir), "Project must contain a 'src' directory.");
    BRV_ASSERT(file::isdir(bctx->include_dir), "Project must contain a 'include' directory.");

    file::recurse(bctx->src_dir, bctx->src_files, BRV_FILE_EXT_CPP);
    file::swap(bctx->src_files, bctx->obj_files, bctx->src_dir, bctx->obj_dir, BRV_FILE_EXT_OBJ);

    if (file::isdir(bctx->test_dir / BRV_DIR_SRC)) {
        file::recurse(bctx->test_dir / BRV_DIR_SRC, bctx->test_src_files, BRV_FILE_EXT_CPP);
        file::swap(
            bctx->test_src_files,
            bctx->test_obj_files,
            bctx->test_dir / BRV_DIR_SRC,
            bctx->test_dir / BRV_DIR_OBJ,
            BRV_FILE_EXT_OBJ
        );
        file::swap(
            bctx->test_src_files,
            bctx->test_exe_files,
            bctx->test_dir / BRV_DIR_SRC,
            bctx->test_dir / BRV_DIR_BIN,
            BRV_FILE_EXT_EXE
        );
    }


    std::string ext = cfg->project_type == BRV_PROJECT_TYPE_EXEC ? BRV_FILE_EXT_EXE : BRV_FILE_EXT_ARCHIVE;

    bctx->end_dst = bctx->bin_dir / (cfg->build_name + ext);
    bctx->include_dirs.emplace_back(bctx->include_dir);
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
