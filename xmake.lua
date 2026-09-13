add_rules("mode.debug", "mode.release")

set_languages("c++26")

add_requires("cpp-httplib")
add_requires("entt")
add_requires("libdatachannel")
add_requires("jwt-cpp")
add_requires("nlohmann_json")

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

target("server")
    set_kind("binary")
    add_files("src/**.cpp")
    add_includedirs("src")

    add_packages("entt", "libdatachannel", "cpp-httplib", "jwt-cpp", "nlohmann_json")
    add_defines("WIN32_LEAN_AND_MEAN", "NOMINMAX")