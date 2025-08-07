#include <bravo/bravo.hpp>

using namespace brv;

void brv::resolveProtocol(CmdContext *cctx) {
    std::set<fs::path> visited{}, visiting{};

    for (ProjectContext *pctx : cctx->projects)
        graph::build(visited, visiting, cctx, pctx);
}

void graph::build(std::set<fs::path> &visited, std::set<fs::path> &visiting, CmdContext *cctx, ProjectContext *pctx) {
    fs::path root = pctx->config->root;

    BRV_ASSERT(!visiting.contains(root), "Circular dependecy involving '", pctx->config->project_name, "'.");
    if (visited.contains(root)) return;

    visiting.insert(root);

    for (const fs::path &path : pctx->config->deps)
        for (ProjectContext *dpctx : cctx->projects)
            if (dpctx->config->root == path) {
                graph::build(visited, visiting, cctx, dpctx);
                pctx->build->include_dirs.push_back(dpctx->build->include_dir);
            }

    visiting.erase(root);
    visited.insert(root);
    cctx->build_protocol.emplace_back(pctx);
}
