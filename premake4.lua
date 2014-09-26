--------------------------------------------------------------------------
-- A premake configuration script for the MicroMouse 2014 project. 
--------------------------------------------------------------------------

-- Adds new macros definitions.
defines {
    "CC=clang++" -- 
}

-- A solution contains projects, and defines the available configurations
solution "MicroMouse2014"
    configurations {
        "Debug", "Release" -- Arbitrary, but must match those below.
    }
    -- The directories to search for #include files.
    includedirs { 
        "/usr/include/**", "/usr/local/include/**" -- "**" denotes recursion
    } 
    -- The directories to search for dynamic/static libraries to link against.
    libdirs {
        "/usr/lib/**", "/usr/local/lib/**" 
    }
    -- Args passed directly to the compiler without translation.
    buildoptions {
        "--std=c++11"
    } 
    -- Passes args directly to the linker without translation.
    linkoptions { 
        "-search_paths_first"   -- From the "ld" man page
    
    }
    -- Specifies a list of libraries and projects to link against.
    links {
        -- "libstdc++", -- ".*" need not be included 
        "libc++"
    }

	-- A project defines one build target, i.e. the name of the executable
	project "micromouse"
		kind (  -- ONE of the following
        "ConsoleApp"  -- A console, or command-line, executable.
        -- "WindowedApp" -- An app that runs in a desktop window. Only for Windows and OS X.
        -- "SharedLib"   -- A shared library, or DLL.
        -- "StaticLib"   -- A static library.
        )
		language "C++"      -- "C", "C++", or "C#"
		files { "*.h", "*.cpp", "*.tcc" }

		configuration "Debug"
			defines { "DEBUG" }
			flags { 
                "Symbols"         -- Generate debugging information.
                -- "ExtraWarnings"   ,-- Sets the compiler's maximum warning level.
                -- "FatalWarnings"   ,-- Treat warnings as errors.
            
            }
            -- Appended and sent to the compiler directly.
            buildoptions {
                "-pedantic",
            }
            -- Appends more arguments for the linker when congfigured for "Debug".
            linkoptions {
                "-t"    -- Logs each file (object, archive, or dylib) the linker loads.  
                        -- Useful for debugging problems with search paths where the wrong library is loaded.
            }
            

		configuration "Release"
			defines { "NDEBUG" }
			flags { 
                "Optimize"        -- Perform a balanced set of optimizations.
                -- "OptimizeSize"    ,-- Optimize for the smallest file size.
                -- "OptimizeSpeed"   ,-- Optimize for the best performance.
                -- "EnableSSE"       ,-- Use the SSE instruction sets for floating point math.(Much faster float ops. are freq. repeated)
                -- "EnableSSE2"      ,-- Use the SSE2 instruction sets for floating point math.
                -- "FloatFast"       ,-- Enable floating point optimizations at the expense of accuracy.
                -- "FloatStrict"     ,-- Improve floating point consistency at the expense of performance.
                -- "StaticRuntime"   ,-- Perform a static link against the standard runtime libraries.
                -- "Unicode"         ,-- Enable Unicode strings. If not specified, the default toolset behavior is used.
                -- "NoExceptions"    ,-- Disable C++ exception support.
                -- "No64BitChecks"   ,-- Disable 64-bit portability warnings.
            }

