#include <bravo/bravo.hpp>

#include <sstream>
#include <thread>

using namespace brv;

void build::compile(const CmdContext *cctx) {

    std::vector<std::string> cmds;

    for (const ProjectContext *pctx : cctx->build_protocol) {

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

            std::ostringstream cmd;
            cmd << common.str();
            cmd << " -c " << file;
            cmd << " -o " << dst;

            cmds.emplace_back(cmd.str());
        }
    }

    std::vector<std::thread> workers;
    std::atomic<size_t> index{0};

    const int thread_count = std::thread::hardware_concurrency();

    for (int i = 0; i < thread_count; ++i) {
        BRV_TRACE("[Worker] Dispached compiler thread (", i+1, "/", thread_count, ")!");
        workers.emplace_back(std::thread(worker, std::cref(cmds), std::ref(index)));
    }

    for (std::thread &worker : workers) {
        worker.join();
        BRV_TRACE("[Worker] Joined compiler thread!");
    }
}

void build::worker(const std::vector<std::string> &cmds, std::atomic<size_t> &index) {
    while (true) {
        size_t i = index++;
        if (i >= cmds.size()) return;

        int exit_code = std::system(cmds[i].c_str());
        if (exit_code != EXIT_SUCCESS) {
            BRV_THROW("Failed to compile build command");
        }
    }
}

void build::link(const CmdContext *cctx) {

    std::vector<fs::path> archs = {};

    int proj = 1;

    for (const ProjectContext *pctx : cctx->build_protocol) {

        std::string cmd;

        if (pctx->config->project_type == BRV_PROJECT_TYPE_EXEC) {
            cmd = linkExec(pctx, archs);
            BRV_TRACE("[Linker] Building executable (", proj++, "/", cctx->build_protocol.size(), ")");
        }
        else if (pctx->config->project_type == BRV_PROJECT_TYPE_STATIC) {
            cmd = linkStatic(pctx, archs);
            BRV_TRACE("[Linker] Building archive (", proj++, "/", cctx->build_protocol.size(), ")");
        } else {
            BRV_THROW("Unreachable!");
        }

        fs::create_directories(pctx->build->bin_dir);

        if (std::system(cmd.c_str()) != EXIT_SUCCESS) {
            BRV_THROW("Failed to link!");
        }
    }
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
