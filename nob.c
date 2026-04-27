#define NOB_IMPLEMENTATION
#include "nob.h"

#ifndef CC
#  define CC "cc"
#endif

#define RAYLIB_SRC "raylib-6.0/src"
#define BUILD_DIR ".build"

#define CFLAGS "-Wall", "-Wextra", "-pedantic", "-std=c11", "-ggdb"
#define CLIBS "-L"BUILD_DIR, "-l:libraylib.a", "-lm", "-lX11"

static Cmd cmd = {0};

bool build_raylib(void) {
    const char *units[] = {
        "rcore",
        "rtextures",
        "rtext",
        "rglfw",
        "rmodels",
        "rshapes",
        "raudio",
    };

    Nob_Procs procs = {0};
    for (size_t i = 0; i < ARRAY_LEN(units); ++i) {
        const char *src = temp_sprintf("%s/%s.c", RAYLIB_SRC, units[i]);
        const char *obj = temp_sprintf("%s/%s.o", BUILD_DIR, units[i]);

        if (!needs_rebuild1(obj, src)) {
            continue;
        }

        cmd_append(&cmd, CC);
        cmd_append(&cmd, "-I"RAYLIB_SRC);
        cmd_append(&cmd, "-I"RAYLIB_SRC"/external/glfw/include");
        cmd_append(&cmd, "-DPLATFORM_DESKTOP_GLFW", "-D_GLFW_X11");
        cmd_append(&cmd, "-c", src, "-o", obj);

        if (!cmd_run(&cmd, .async = &procs)) return false;
    }
    if (!nob_procs_wait(procs)) return false;

    cmd_append(&cmd, "ar", "rcs", BUILD_DIR"/libraylib.a");
    for (size_t i = 0; i < ARRAY_LEN(units); ++i) {
        const char *obj = temp_sprintf("%s/%s.o", BUILD_DIR, units[i]);
        cmd_append(&cmd, obj);
    }
    if (!cmd_run(&cmd)) return false;

    return true;
}

int main(int argc, char **argv) {
    GO_REBUILD_URSELF_PLUS(argc, argv, "nob.h");

    if (!mkdir_if_not_exists(BUILD_DIR)) {
        return 1;
    }

    if (!build_raylib()) return 1;

    cmd_append(&cmd, CC, CFLAGS, "-o", "gesq", "src/main.c", CLIBS);
    if (!cmd_run(&cmd)) return 1;

    return 0;
}
