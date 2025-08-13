#include <bravo/bravo.hpp>

using namespace brv;

void cmd::run(const CmdContext *cctx) {
    cmd::build(cctx); // TODO : skip linking if possible

    const ProjectContext *pctx = cctx->build_protocol.back();

    BRV_ASSERT(pctx->config->project_type == BRV_PROJECT_TYPE_EXEC, "Cannot run a project of type '", pctx->config->project_type, "'");

    const fs::path rel = fs::relative(pctx->build->end_dst, pctx->build->bin_dir);
    int exit_code = std::system(rel.c_str());
    BRV_CONDITIONAL(cctx->verbose, "Program exited with code : ", exit_code);
}
