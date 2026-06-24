-- xmake.lua for DuiLib (SDL3 backend)

-- set the project information
set_project("DuiLib")
set_version("1.0.0")

set_xmakever("2.7.9")

-- C++14 standard
set_languages("cxx14")

-- configuration option: unicode (Windows only)
option("unicode")
    set_default(false)
    set_showmenu(true)
    set_description("Enable Unicode character set (Windows)")
option_end()

-- add global defines for SDL backend
add_defines("DUILIB_SDL")

--------------------------------------------------------------------------------
-- Target: DuiLib
target("DuiLib")
    local target_file_name = "DuiLib"

    -- Platform-specific settings
    if is_plat("linux") then
        add_includedirs("/usr/local/include/SDL3")
        add_includedirs("/usr/local/include/SDL3_ttf")

        -- Link SDL3, SDL_ttf
        add_links("SDL3", "SDL3_ttf")
        -- System libs
        add_links("pthread", "dl")

        -- Install headers (same as original)
        add_installfiles("compat.h", {prefixdir = "include/DuiLib"})
        add_installfiles("DuiLib.h", {prefixdir = "include/DuiLib"})
        add_installfiles("UIlib.h", {prefixdir = "include/DuiLib"})
        add_installfiles("Control/*.h", {prefixdir = "include/DuiLib/Control"})
        add_installfiles("Core/*.h", {prefixdir = "include/DuiLib/Core"})
        add_installfiles("Layout/*.h", {prefixdir = "include/DuiLib/Layout"})
        add_installfiles("Render/*.h", {prefixdir = "include/DuiLib/Render"})
        add_installfiles("Utils/*.h", {prefixdir = "include/DuiLib/Utils"})

        after_uninstall(function (target)
            os.rm(target:installdir() .. "/include/DuiLib/")
        end)

    elseif is_plat("macosx") then
        add_includedirs("/usr/local/include/SDL3")
        add_includedirs("/usr/local/include/SDL3_ttf")

        add_links("SDL3", "SDL3_ttf")
        -- Needed system frameworks
        add_frameworks("Cocoa", "OpenGL", "IOKit", "CoreVideo", "CoreFoundation")
        -- CoreText and CoreGraphics for text shaping (harfbuzz/CoreText)
        add_frameworks("CoreText", "CoreGraphics")

        -- macOS deployment target (set to 11.0)
        add_cxflags("-mmacosx-version-min=11.0")
        add_ldflags("-mmacosx-version-min=11.0")

        -- Install headers
        add_installfiles("compat.h", {prefixdir = "include/DuiLib"})
        add_installfiles("DuiLib.h", {prefixdir = "include/DuiLib"})
        add_installfiles("UIlib.h", {prefixdir = "include/DuiLib"})
        add_installfiles("Control/*.h", {prefixdir = "include/DuiLib/Control"})
        add_installfiles("Core/*.h", {prefixdir = "include/DuiLib/Core"})
        add_installfiles("Layout/*.h", {prefixdir = "include/DuiLib/Layout"})
        add_installfiles("Render/*.h", {prefixdir = "include/DuiLib/Render"})
        add_installfiles("Utils/*.h", {prefixdir = "include/DuiLib/Utils"})

        after_uninstall(function (target)
            os.rm(target:installdir() .. "/include/DuiLib/")
        end)

    elseif is_plat("windows") then
        -- Windows-specific macros
        add_defines("WIN32", "_WIN32", "WINDOWS")

        -- Unicode option
        if has_config("unicode") then
            add_defines("UNICODE", "_UNICODE")
        end

        if is_kind("static") then
            add_defines("UILIB_EXPORTS", "UILIB_STATIC")
        else
            add_defines("UILIB_EXPORTS")
        end

        -- SDL3 include path (assume 3rd party directory is at ../3rd/SDL/include)
        add_includedirs("$(projectdir)/../3rd/SDL/include")
        add_includedirs("$(projectdir)/../3rd/SDL_ttf/include")

        -- Link SDL3 static libs (adjust names/paths as needed)
        add_linkdirs("$(projectdir)/../3rd/SDL/lib")
        add_linkdirs("$(projectdir)/../3rd/SDL_ttf/lib")
        add_links("SDL3", "SDL3_ttf")

        -- DLL/lib filename rule (original logic preserved)
        if is_arch("x64") then
            if has_config("unicode") then
                if is_kind("static") then
                    target_file_name = target_file_name .. "_64us"
                else
                    target_file_name = target_file_name .. "_64u"
                end
            else
                if is_kind("static") then
                    target_file_name = target_file_name .. "_64s"
                else
                    target_file_name = target_file_name .. "_64"
                end
            end
        else
            if has_config("unicode") then
                if is_kind("static") then
                    target_file_name = target_file_name .. "_us"
                else
                    target_file_name = target_file_name .. "_u"
                end
            else
                if is_kind("static") then
                    target_file_name = target_file_name .. "_s"
                else
                    target_file_name = target_file_name
                end
            end
        end

        -- Copy output files after build
        if is_kind("shared") then
            after_build(function (target)
                local destfile = target:scriptdir() .. "/../bin/" .. target:filename()
                os.cp(target:targetfile(), destfile)

                local destfile_lib = target:scriptdir() .. "/Lib/" .. target_file_name .. ".lib"
                local srcfile = target:targetdir() .. "/" .. target_file_name .. ".lib"
                os.cp(srcfile, destfile_lib)
            end)
        else
            after_build(function (target)
                local destfile = target:scriptdir() .. "/Lib/" .. target:filename()
                os.cp(target:targetfile(), destfile)
            end)
        end
    end

    -- Common settings
    set_kind("$(kind)")
    if is_mode("debug") then
        set_optimize("none")
        add_defines("DEBUG", "_DEBUG")
        set_symbols("debug")
    elseif is_mode("release") then
        set_optimize("fastest")
        set_strip("all")
        add_defines("NDEBUG")
    end

    set_targetdir("$(buildir)/$(plat)/$(arch)/$(mode)")
    set_basename(target_file_name)

    -- Source files
    add_files("**.cpp")
    remove_files("Utils/unzip.cpp", "UIDataExchange.cpp")

    -- Include directories
    add_includedirs(".")

    -- Precompiled header
    set_pcxxheader("./StdAfx.h")