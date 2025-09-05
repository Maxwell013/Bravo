#include <bravo/bravo.hpp>

#include <vector>

using namespace brv;

CmdContext *brv::processCliArgs(int argc, char **argv) {
    CmdContext *cctx = new CmdContext();

    if (argc < 2) {
        cctx->cmd = CMD_MAP.at(BRV_CMD_HELP_STR);
        return cctx;
    }

    const std::string cmd = cli::fuzzyMatch(argv[1], CMD_VECTOR);
    cctx->cmd = CMD_MAP.at(cmd);

    for (int i = 2; i < argc; i++)
        cli::parseArg(argv[i], cmd, cctx);

    if (cctx->verbose) {
        BRV_INFO("Verbose logging enabled!");
        BRV_CONDITIONAL(cctx->rebuild, "Recursive dependency rebuild enabled!");
        BRV_CONDITIONAL(cctx->no_build, "Build skip enabled!");
        for (const std::string &arg : cctx->non_opt_args)
            BRV_INFO("Non-option argument parsed : '", arg, "'!");
    }

    // TODO : proper validation
    BRV_ASSERT(!(cctx->no_build && cctx->rebuild), "Cannot enable both flags : 'deps' and 'no-build'.");

    return cctx;
}

void cli::parseArg(const std::string &input, const std::string &cmd, CmdContext *cctx) {
    if (input.size() < 2)
        BRV_THROW("Invalid argument : '", input, "' : too short!");

    if (!input.starts_with('-')) {
        cctx->non_opt_args.emplace_back(input);
        if (cctx->non_opt_args.size() > cctx->cmd.non_opt_argc_max)
            BRV_THROW("Too many arguments specifed!");
        return;
    }

    if (input.starts_with("--")) {
        unsigned int opt_id = OPT_LONG_MAP.at(
            fuzzyMatch(
                input.substr(2),
                OPT_LONG_VECTOR)
        );
        setOpt(cctx, cmd, opt_id);
        return;
    }

    for (const char &ch : input.substr(1)) {
        if (!OPT_SHORT_SET.contains(ch))
            BRV_THROW("Unkown shorthand argument : '", ch, "'!");
        setOpt(cctx, cmd, OPT_SHORT_MAP.at(ch));
    }
}

std::string cli::fuzzyMatch(const std::string &input, const std::vector<std::string> &valid) {
    std::vector<std::string> matches;
    for (const std::string& valid : valid)
        if (valid.starts_with(input))
            matches.emplace_back(valid);

    switch (matches.size()) {
    case 0:
        BRV_THROW("Unkown command or argument!");
    case 1:
        return matches[0];
    }
    BRV_THROW("Ambiguous command or argument!");
    return ""; // Silence compiler
}

void cli::setOpt(CmdContext *cctx, const std::string &cmd, unsigned int opt_id) {
    if (!VALID_OPT_IDS.at(cmd).contains(opt_id))
        BRV_THROW("Command '", cmd, "' does not support specified arguments!");

    switch (opt_id) {
    case BRV_OPT_VERBOSE_ID:
        cctx->verbose = true;
        return;
    case BRV_OPT_DEPS_ID:
        cctx->rebuild = true;
        return;
    case BRV_OPT_NO_BUILD_ID:
        cctx->no_build = true;
        return;
    }
}
