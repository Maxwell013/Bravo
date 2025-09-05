#include <bravo/bravo.hpp>

using namespace brv;

void cmd::run(const CmdContext *cctx) {
    cmd::build(cctx); // TODO : skip linking if possible

    const ProjectContext *pctx = cctx->active_project;

    BRV_ASSERT(pctx->config->project_type == BRV_PROJECT_TYPE_EXEC, "Cannot run a project of type '", pctx->config->project_type, "'");

    const fs::path rel = fs::relative(pctx->build->end_dst, pctx->config->root);

    std::string args = pctx->config->run_args.has_value() ? pctx->config->run_args.value() : "";

    int exit_code = std::system((rel.string() + " " + args).c_str());
    BRV_CONDITIONAL(cctx->verbose, "Program exited with code : ", exit_code);
}
