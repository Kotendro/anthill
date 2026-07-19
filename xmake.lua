add_rules("mode.debug", "mode.release")

add_requires("raylib")
add_requires("sol2")
add_rules("plugin.compile_commands.autoupdate")

target("anthill")
    set_kind("binary")
    set_languages("c++20")
    add_includedirs("src")
    add_files("src/*.cpp", "src/**.cpp")
    add_packages("raylib", "sol2")

    after_build(function (target)
        os.cp("scripts", target:targetdir())
    end)

