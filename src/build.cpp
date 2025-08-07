#include <bravo/bravo.hpp>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <thread>

using namespace brv;

void build::compile(const CmdContext *cctx) {

    BRV_CONDITIONAL(cctx->verbose, "Preparing compilation:");

    std::vector<std::string> cmds;
    for (const ProjectContext *pctx : cctx->build_protocol) {
        BRV_CONDITIONAL(cctx->verbose, "Enumerating source files for '", pctx->config->project_name, "':");

        std::ostringstream common;
        common << "clang++ -std=c++20 -Wall -Wextra -Werror -pedantic-errors"; // tmp
        for (const fs::path &dir : pctx->build->include_dirs)
            common << " -I" << dir;

        for (const fs::path &file : pctx->build->src_files) {
            const fs::path rel = fs::relative(file, pctx->build->src_dir);
            fs::path dst = rel;
            dst.replace_extension(BRV_FILE_EXT_OBJ);
            dst = pctx->build->obj_dir / dst;

            fs::create_directories(dst.parent_path());
            pctx->build->obj_files.emplace_back(dst);

            if (!cctx->rebuild && !rebuild(file, dst)) {
                BRV_CONDITIONAL(cctx->verbose, "Skiping source file ", rel);
                continue;
            }
            BRV_CONDITIONAL(cctx->verbose, "Adding source file ", rel);

            std::ostringstream cmd;
            cmd << common.str();
            cmd << " -c " << file;
            cmd << " -o " << dst;

            cmds.emplace_back(cmd.str());
        }
    }

    std::vector<std::thread> workers;
    std::atomic<size_t> index{0};

    const int thread_count = threadCount(cmds);
    BRV_CONDITIONAL(cctx->verbose, "Starting compilation with ", thread_count, " thread(s):");

    for (int i = 0; i < thread_count; ++i)
        workers.emplace_back(std::thread(worker, i, cctx->verbose, std::cref(cmds), std::ref(index)));

    for (std::thread &worker : workers)
        worker.join();

    BRV_CONDITIONAL(cctx->verbose, "All workers done; compiled ", cmds.size(), " file(s)!");
}

void build::worker(const int id, const bool verbose, const std::vector<std::string> &cmds, std::atomic<size_t> &index) {
    BRV_CONDITIONAL(verbose, "Dispached worker thread with id : ", id);
    while (true) {
        size_t i = index.fetch_add(1);
        if (i >= cmds.size()) break;

        BRV_CONDITIONAL(verbose, "Worker [", id, "] picked up task (", i+1, "/", cmds.size(), ")");

        int exit_code = std::system(cmds[i].c_str());
        BRV_ASSERT(exit_code == EXIT_SUCCESS, "Worker [", id, "] Failed to compile build command.");
    }
    BRV_CONDITIONAL(verbose, "Worker [", id, "] done!");
}

void build::link(const CmdContext *cctx) {

    BRV_CONDITIONAL(cctx->verbose, "Starting linking protocol:");

    std::vector<fs::path> archs = {};
    int proj = 0;
    for (const ProjectContext *pctx : cctx->build_protocol) {

        BRV_CONDITIONAL(cctx->verbose, "Linking project '", pctx->config->project_name, "' (", ++proj, "/", cctx->build_protocol.size(), ")");

        std::string cmd;

        if (pctx->config->project_type == BRV_PROJECT_TYPE_EXEC) {
            cmd = linkExec(pctx, archs);
        }
        else {
            cmd = linkStatic(pctx, archs);
        }

        fs::create_directories(pctx->build->bin_dir);

        BRV_ASSERT(std::system(cmd.c_str()) == EXIT_SUCCESS, "Failed to link project '", pctx->config->project_name, "'.");
    }

    BRV_CONDITIONAL(cctx->verbose, "Linking done; ", cctx->build_protocol.size(), " projects linked!");
}

std::string build::linkExec(const ProjectContext *pctx, const std::vector<fs::path> &archs) {
    std::ostringstream cmd;

    fs::path dst = pctx->build->bin_dir;
    dst /= (pctx->config->project_name + BRV_FILE_EXT_EXE);

    cmd << "clang++ -std=c++20 -Wall -Wextra -Werror -pedantic-errors"; // tmp
    cmd << " -o " << dst;

    for (const fs::path &arch : archs)
        cmd << " " << arch;

    for (const fs::path &obj : pctx->build->obj_files)
        cmd << " " << obj;

    return cmd.str();
}

std::string build::linkStatic(const ProjectContext *pctx, std::vector<fs::path> &archs) {
    std::ostringstream cmd;

    fs::path dst = pctx->build->bin_dir;
    dst /= (pctx->config->project_name + BRV_FILE_EXT_ARCHIVE); // TODO : add config field for bin filename

    cmd << "ar rcs " << dst; // tmp

    archs.emplace_back(dst);

    for (const fs::path &obj : pctx->build->obj_files)
        cmd << " " << obj;

    return cmd.str();
}

bool build::rebuild(const fs::path &src, const fs::path &obj) {
    if (!file::isfile(obj)) return true;
    return fs::last_write_time(src) > fs::last_write_time(obj);
}

int build::threadCount(const std::vector<std::string> &cmds) {
    return std::min(std::thread::hardware_concurrency(), (unsigned int)cmds.size());
}
