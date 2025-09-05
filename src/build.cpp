#include <bravo/bravo.hpp>

#include <filesystem>
#include <thread>
#include <vector>

using namespace brv;

void build::compile(const CmdContext *cctx) {

    BRV_CONDITIONAL(cctx->verbose, "Preparing compilation:");

    std::string base = "clang++ -std=c++20 -Wall -Wextra -Werror -pedantic-errors"; // tmp

    std::vector<std::string> cmds;
    for (const ProjectContext *pctx : cctx->build_protocol) {

        BRV_CONDITIONAL(cctx->verbose, "Enumerating source files for '", pctx->config->project_name, "':");

        std::ostringstream common;
        common << base;
        for (const fs::path &dir : pctx->build->include_dirs)
            common << " -I" << dir;

        const unsigned int size = pctx->build->src_files.size();

        for (unsigned int i = 0; i < size; ++i) {
            const fs::path src = pctx->build->src_files.at(i);
            const fs::path dst = pctx->build->obj_files.at(i);

            if (cctx->rebuild || rebuild(src, dst)) {
                BRV_CONDITIONAL(cctx->verbose, "Adding : ", src.filename());
                cmds.emplace_back(makeCompileCommand(common.str(), src, dst));
                continue;
            }
            BRV_CONDITIONAL(cctx->verbose, "Skipping : ", src.filename());
        }
    }

    const BuildContext *bctx = cctx->active_project->build;

    if (!bctx->test_src_files.empty()) {
        BRV_CONDITIONAL(cctx->verbose, "Enumerating test files for '", cctx->active_project->config->project_name, "':");

        std::ostringstream common;
        common << base;
        for (const fs::path &dir : bctx->include_dirs)
            common << " -I" << dir;

        const unsigned int size = bctx->test_src_files.size();

        for (unsigned int i = 0; i < size; ++i) {
            const fs::path src = bctx->test_src_files.at(i);
            const fs::path dst = bctx->test_obj_files.at(i);
            if (cctx->rebuild || rebuild(src, dst)) {
                BRV_CONDITIONAL(cctx->verbose, "Adding : ", src.filename());
                cmds.emplace_back(makeCompileCommand(common.str(), src, dst));
                continue;
            }
            BRV_CONDITIONAL(cctx->verbose, "Skipping : ", src.filename());
        }
    }

    std::vector<std::thread> workers{};
    std::atomic<size_t> index = 0;

    const unsigned int thread_count = threadCount(cmds);
    BRV_CONDITIONAL(cctx->verbose, "Starting compilation with ", thread_count, " thread(s):");

    for (unsigned int i = 0; i < thread_count; ++i)
        workers.emplace_back(std::thread(worker, i, cctx->verbose, std::cref(cmds), std::ref(index)));

    for (std::thread &worker : workers)
        worker.join();

    BRV_CONDITIONAL(cctx->verbose, "All workers done; compiled ", cmds.size(), " file(s)!");
}

void build::worker(unsigned int id, bool verbose, const std::vector<std::string> &cmds, std::atomic<size_t> &index) {
    BRV_CONDITIONAL(verbose, "Dispached worker thread with id : ", id);
    while (true) {
        size_t i = index.fetch_add(1);
        if (i >= cmds.size()) break;

        int exit_code = std::system(cmds[i].c_str());
        BRV_ASSERT(exit_code == EXIT_SUCCESS, "Worker [", id, "] Failed to compile build command.");

        BRV_CONDITIONAL(verbose, "Worker [", id, "] ended task (", i+1, "/", cmds.size(), ")");
    }
    BRV_CONDITIONAL(verbose, "Worker [", id, "] done!");
}

void build::link(const CmdContext *cctx) {

    BRV_CONDITIONAL(cctx->verbose, "Starting linking protocol:");

    std::vector<fs::path> archs{};
    int proj = 0;
    for (const ProjectContext *pctx : cctx->build_protocol) {

        BRV_CONDITIONAL(cctx->verbose, "Linking project '", pctx->config->project_name, "' (", ++proj, "/", cctx->build_protocol.size(), ")");

        LinkProcess process = LINK_PROCESS_MAP.at(pctx->config->project_type);

        const std::string cmd = process(pctx->build->obj_files, archs, pctx->build->end_dst);

        fs::create_directories(pctx->build->bin_dir);

        BRV_ASSERT(std::system(cmd.c_str()) == EXIT_SUCCESS, "Failed to link project '", pctx->config->project_name, "'.");
    }

    BRV_CONDITIONAL(cctx->verbose, "Starting test linking:");

    const BuildContext *bctx = cctx->active_project->build;
    const ConfigContext *cfg = cctx->active_project->config;

    std::vector<fs::path> objs{};

    if (cfg->project_type == BRV_PROJECT_TYPE_EXEC) {
        for (const fs::path &obj : bctx->obj_files)
            if (fs::relative(obj, bctx->obj_dir).replace_extension(BRV_FILE_EXT_CPP).string() != cfg->entry.value())
                objs.emplace_back(obj);
    }

    for (const fs::path &arch : archs)
        objs.emplace_back(arch);

    for (const fs::path &test : bctx->test_obj_files) {

        BRV_CONDITIONAL(cctx->verbose, "Linking test ", test.filename(), ".");

        fs::path dst = bctx->test_dir / BRV_DIR_BIN / fs::relative(
            test,
            bctx->test_dir / BRV_DIR_OBJ
        ).replace_extension(BRV_FILE_EXT_EXE);
        const std::string cmd = linkExec({ test }, objs, dst);
        fs::create_directories(dst.parent_path());
        BRV_ASSERT(std::system(cmd.c_str()) == EXIT_SUCCESS, "Failed to link test ", test.filename(), ".");
    }

    BRV_CONDITIONAL(cctx->verbose, "Linking done; ", cctx->build_protocol.size(), " projects linked!");
}

std::string build::linkExec(const std::vector<fs::path> &objs, std::vector<fs::path> &archs, const fs::path &dst) {
    std::ostringstream cmd;

    cmd << "clang++ -std=c++20 -Wall -Wextra -Werror -pedantic-errors"; // tmp
    cmd << " -o " << dst;

    for (const fs::path &arch : archs)
        cmd << " " << arch;

    for (const fs::path &obj : objs)
        cmd << " " << obj;

    return cmd.str();
}

std::string build::linkStatic(const std::vector<fs::path> &objs, std::vector<fs::path> &archs, const fs::path &dst) {
    std::ostringstream cmd;

    cmd << "ar rcs " << dst;

    archs.emplace_back(dst);

    for (const fs::path &obj : objs)
        cmd << " " << obj;

    return cmd.str();
}

std::string build::makeCompileCommand(const std::string &common, const fs::path &src, const fs::path &dst) {
    fs::create_directories(dst.parent_path());
    std::ostringstream cmd;
    cmd << common;
    cmd << " -c " << src;
    cmd << " -o " << dst;
    return cmd.str();
}

bool build::rebuild(const fs::path &src, const fs::path &obj) {
    if (!file::isfile(obj)) return true;
    return fs::last_write_time(src) > fs::last_write_time(obj);
}

int build::threadCount(const std::vector<std::string> &cmds) {
    return std::min(std::thread::hardware_concurrency(), (unsigned int)cmds.size());
}
