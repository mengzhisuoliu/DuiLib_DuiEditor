-- set the project information
set_project("TestDuiLib")
set_version("1.0.0")

set_xmakever("2.7.9")
set_languages("cxx14")

option("unicode")
option_end()

-- 确保使用 SDL 后端
add_defines("DUILIB_SDL")

target("TestDuiLib")
    local target_file_name = "TestDuiLib"

    -- 根据平台配置
    if is_plat("linux") then
        -- DuiLib 头文件路径（相对当前脚本：../../DuiLib）
        add_includedirs("$(scriptdir)/../../DuiLib")
        -- DuiLib 库输出目录
        add_linkdirs("$(scriptdir)/../../DuiLib/Lib")
        add_links("DuiLib")

        -- SDL 库路径和链接（通常已安装到系统，或指定自定义路径）
        add_linkdirs("/usr/local/lib")
        add_links("SDL3", "SDL3_ttf")
		add_links("X11")
        add_links("pthread", "dl")

        -- 运行期库搜索路径（相对可执行文件目录）
        add_ldflags("-Wl,-rpath=./ -no-pie")
        add_rpathdirs("/usr/local/lib")

        -- 编译后拷贝可执行文件到当前目录
        after_build(function (target)
            local destfile = target:scriptdir() .. "/" .. target:filename()
            os.cp(target:targetfile(), destfile)
        end)

    elseif is_plat("macosx") then
        -- DuiLib 头文件路径
        add_includedirs("$(scriptdir)/../../DuiLib")
        add_linkdirs("$(scriptdir)/../../DuiLib/Lib")
        add_links("DuiLib")

        -- SDL 库
        add_linkdirs("/usr/local/lib", "/opt/homebrew/lib")
        add_links("SDL3", "SDL3_ttf")

        -- 必要的系统框架
        add_frameworks("Cocoa", "OpenGL", "IOKit", "CoreVideo", "CoreFoundation",
                       "CoreText", "CoreGraphics")

        -- 兼容 macOS 11.0 及以上
        add_cxflags("-mmacosx-version-min=11.0")
        add_ldflags("-mmacosx-version-min=11.0")

        -- 拷贝到当前目录
        after_build(function (target)
            local destfile = target:scriptdir() .. "/" .. target:filename()
            os.cp(target:targetfile(), destfile)
        end)

    elseif is_plat("windows") then
        -- Unicode 支持
        if has_config("unicode") then
            add_defines("UNICODE", "_UNICODE")
        end

        -- DuiLib 头文件
        add_includedirs("$(scriptdir)/../../DuiLib")
        add_linkdirs("$(scriptdir)/../../DuiLib/Lib")

        -- SDL 头文件（假设 3rd 目录与 DuiLib 平级）
        add_includedirs("$(scriptdir)/../../3rd/SDL/include")
        add_includedirs("$(scriptdir)/../../3rd/SDL_ttf/include")

        -- SDL 库路径
        add_linkdirs("$(scriptdir)/../../3rd/SDL/lib",
                     "$(scriptdir)/../../3rd/SDL_ttf/lib")

        -- 链接 DuiLib（Windows 下库名可能带后缀，如 DuiLib_64u.lib）
        -- 这里假设你已通过 DuiLib 的 xmake 规则生成正确文件名
        if is_mode("debug") then
            add_links("DuiLib_64ud")   -- 示例，根据实际命名调整
        else
            add_links("DuiLib_64u")
        end
        add_links("SDL3", "SDL3_ttf")

        -- 编译后拷贝到 bin 目录
        after_build(function (target)
            local destdir = target:scriptdir() .. "/../bin/"
            os.mkdir(destdir)
            os.cp(target:targetfile(), destdir)
        end)
    end

    -- 通用设置
    set_kind("binary")

    if is_mode("debug") then
        set_optimize("none")
        add_defines("DEBUG", "_DEBUG")
        set_symbols("debug")
    elseif is_mode("release") then
        set_optimize("fastest")
        set_strip("all")
        add_defines("NDEBUG")
    end

    set_targetdir("$(scriptdir)/../../bin")
    set_basename(target_file_name)

    -- 源文件：当前目录所有 cpp
    add_files("*.cpp")
