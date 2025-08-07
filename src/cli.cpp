#include <bravo/bravo.hpp>

#include <vector>

using namespace brv;

CmdContext *brv::processCliArgs(int argc, char **argv) {
    CmdContext *cctx = new CmdContext();

    if (argc < 2) {
        cctx->cmd = CMD_MAP[BRV_CMD_HELP_STR];
        return cctx;
    }

    const std::string cmd = cli::fuzzyMatch(argv[1], VALID_CMD_STRS);
    cctx->cmd = CMD_MAP[cmd];

    for (int i = 2; i < argc; i++) {
        const std::string opt = cli::fuzzyMatch(argv[i], VALID_OPT_STRS[cmd]);
        cli::setOpt(cctx, opt);
    }

    return cctx;
}

std::string cli::fuzzyMatch(const std::string &input, const std::vector<std::string> &valid) {
    std::vector<std::string> matches;
    for (const std::string& valid : valid)
        if (valid.find(input) == 0)
            matches.emplace_back(valid);

    switch (matches.size()) {
    case 0:
        BRV_THROW("Unkown command!");
    case 1:
        return matches[0];
    default:
        BRV_THROW("Ambiguous command!");
    }
    BRV_THROW("Unreachable!");
    return ""; // for compiler warning
}

void cli::setOpt(CmdContext *cctx, const std::string &opt) {
    switch (OPT_MAP[opt]) {
    case BRV_OPT_VERBOSE_ID:
        cctx->verbose = true;
        return;
    case BRV_OPT_DEPS_ID:
        cctx->rebuild = true;
        return;
    }
}
