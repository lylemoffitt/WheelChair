--------------------------------------------------------------------------
-- A premake configuration script  
--------------------------------------------------------------------------


-- A solution contains projects, and defines the available configurations
solution "WheelChair"
	configurations {
		"Debug","Release" -- Arbitrary, but must match those below.
	}
	-- The directories to search for #include files.
	includedirs { 
		-- "/usr/include/c++/4.2.1/**",
		-- "/usr/local/include/gcc/**",
		-- "/usr/local/include/**", -- "**" denotes recursion
		-- "/usr/include/**",
		-- "./WheelChair/WheelChair/*",
	} 
	-- The directories to search for dynamic/static libraries to link against.
	libdirs {
		-- "/usr/local/lib/**",
		 -- "/usr/lib/**"
	}
	-- Args passed directly to the compiler without translation.
	buildoptions {
		"--std=c++1y",
		-- "-nostdinc++",
		--"-fextern-tls-init",
		-- "-integrated-as",
		--"-fexceptions",
	} 
	-- Passes args directly to the linker without translation.
	linkoptions { 
	
	}
	-- Specifies a list of libraries and projects to link against.
	links {
		"stdc++", -- ".*" need not be included
		-- "c++",
		"pthread"
	}
	-- A project defines one build target, i.e. the name of the executable
	project "wheelchair"
		local build_path="../wheelchair.build/"
		-- Shell commands to be executed before building
		prebuildcommands {
			("if [[ ! -e "..build_path.." ]]; then mkdir "..build_path.."; fi "),
		}
		-- The location to put the compiled executable
		targetdir ("../..")
		-- The directories to search for #include files.
		includedirs { 
			"./*",
		} 
		kind (	-- ONE of the following
		"ConsoleApp"  -- A console, or command-line, executable.
		-- "WindowedApp" -- An app that runs in a desktop window. Only for Windows and OS X.
		-- "SharedLib"	 -- A shared library, or DLL.
		-- "StaticLib"	 -- A static library.
		)
		language "C++"		-- "C", "C++", or "C#"
		files { "**.h", "**.cpp", "**.tcc" }
		--
		configuration "Release"
			local release_build_dir=(build_path.."Release")
			-- Shell commands to be executed before building
			prebuildcommands {
				("if [[ ! -e "..release_build_dir.." ]]; then mkdir "..release_build_dir.."; fi "),
			}
			-- The location of the build
			location (release_build_dir)
			-- Build vars to be defined
			defines { "NDEBUG" }
			flags { 
				-- "EnableSSE"			   ,
				-- "EnableSSE2"			   ,-- Use the SSE instruction sets for floating point math.
				-- "ExtraWarnings"		   ,-- Sets the compiler's maximum warning level.
				-- "FatalWarnings"		   ,-- Treat warnings as errors.
				-- "FloatFast"			   ,-- Enable floating point optimizations at the expense of accuracy.
				-- "FloatStrict"		   ,-- Improve floating point consistency at the expense of performance.
				-- "Managed"			   ,-- Enable Managed C++ (.NET).
				-- "MFC"				   ,-- Enable support for Microsoft Foundation Classes.
				-- "NativeWChar"		   ,
				-- "NoNativeWChar"		   ,-- Enable or disable support for the wchar data type. If no flag is specified, the toolset default will be used.
				-- "No64BitChecks"		   ,-- Disable 64-bit portability warnings.
				-- "NoEditAndContinue"	   ,-- Disable support for Visual Studio's Edit-and-Continue feature.
				-- "NoExceptions"		   ,-- Disable C++ exception support.
				-- "NoFramePointer"		   ,-- Disable the generation of stack frame pointers.
				-- "NoIncrementalLink"	   ,-- Disable support for Visual Studio's incremental linking feature.
				-- "NoImportLib"		   ,-- Prevent the generation of an import library for a Windows DLL.
				-- "NoManifest"			   ,-- Prevent the generation of a manifest for Windows executables and shared libraries.
				-- "NoMinimalRebuild"	   ,-- Disable Visual Studio's minimal rebuild feature.
				-- "NoPCH"				   ,-- Disable precompiled header support. If not specified, the toolset default behavior will be used. Also see pchheader and pchsource.
				-- "NoRTTI"				   ,-- Disable C++ runtime type information.
				"Optimize"				,-- Perform a balanced set of optimizations.
				-- "OptimizeSize"		   ,-- Optimize for the smallest file size.
				-- "OptimizeSpeed"		   ,-- Optimize for the best performance.
				-- "SEH"				   ,-- Enable structured exception handling.
				-- "StaticRuntime"		   ,-- Perform a static link against the standard runtime libraries.
				-- "Symbols"			   ,-- Generate debugging information.
				-- "Unicode"			   ,-- Enable Unicode strings. If not specified, the default toolset behavior is used.
				-- "Unsafe"				   ,-- Enable the use of unsafe code in .NET applications.
				-- "WinMain"			   ,-- Use WinMain() as the program entry point for Windows applications, rather than the default main().
			}
			--
		configuration "Debug"
			local debug_build_dir=(build_path.."Debug")
			-- Shell commands to be executed before building
			prebuildcommands { 
				("if [[ ! -e "..debug_build_dir.." ]]; then mkdir "..debug_build_dir.."; fi "),
			}
			-- The location of the build
			location (debug_build_dir)
			-- Build vars to be defined
			defines { "DEBUG" }
			flags { 
				-- "EnableSSE"			   ,
				-- "EnableSSE2"			   ,-- Use the SSE instruction sets for floating point math.
				-- "ExtraWarnings"		   ,-- Sets the compiler's maximum warning level.
				-- "FatalWarnings"		   ,-- Treat warnings as errors.
				-- "FloatFast"			   ,-- Enable floating point optimizations at the expense of accuracy.
				-- "FloatStrict"		   ,-- Improve floating point consistency at the expense of performance.
				-- "Managed"			   ,-- Enable Managed C++ (.NET).
				-- "MFC"				   ,-- Enable support for Microsoft Foundation Classes.
				-- "NativeWChar"		   ,
				-- "NoNativeWChar"		   ,-- Enable or disable support for the wchar data type. If no flag is specified, the toolset default will be used.
				-- "No64BitChecks"		   ,-- Disable 64-bit portability warnings.
				-- "NoEditAndContinue"	   ,-- Disable support for Visual Studio's Edit-and-Continue feature.
				-- "NoExceptions"		   ,-- Disable C++ exception support.
				-- "NoFramePointer"		   ,-- Disable the generation of stack frame pointers.
				-- "NoIncrementalLink"	   ,-- Disable support for Visual Studio's incremental linking feature.
				-- "NoImportLib"		   ,-- Prevent the generation of an import library for a Windows DLL.
				-- "NoManifest"			   ,-- Prevent the generation of a manifest for Windows executables and shared libraries.
				-- "NoMinimalRebuild"	   ,-- Disable Visual Studio's minimal rebuild feature.
				-- "NoPCH"				   ,-- Disable precompiled header support. If not specified, the toolset default behavior will be used. Also see pchheader and pchsource.
				-- "NoRTTI"				   ,-- Disable C++ runtime type information.
				-- "Optimize"			   ,-- Perform a balanced set of optimizations.
				-- "OptimizeSize"		   ,-- Optimize for the smallest file size.
				-- "OptimizeSpeed"		   ,-- Optimize for the best performance.
				-- "SEH"				   ,-- Enable structured exception handling.
				-- "StaticRuntime"		   ,-- Perform a static link against the standard runtime libraries.
				"Symbols"				,-- Generate debugging information.
				-- "Unicode"			   ,-- Enable Unicode strings. If not specified, the default toolset behavior is used.
				-- "Unsafe"				   ,-- Enable the use of unsafe code in .NET applications.
				-- "WinMain"			   ,-- Use WinMain() as the program entry point for Windows applications, rather than the default main().
			}
			-- Appended and sent to the compiler directly.
			buildoptions {
				-- "-fwritable-strings",
				-- "-ferror-limit=1",
				-- "-pedantic",
				-- "-v",
				"-g3",
				"-ggdb", 
				"-O0",
				-- "-Weverything",
			}
			-- Appends more arguments for the linker when congfigured for "Debug".
			linkoptions {
				-- "-t"	-- Logs each file (object, archive, or dylib) the linker loads.
						-- Useful for debugging problems with search paths where the wrong library is loaded.
			}
			
		configuration "clean"
			-- Shell commands to be executed before building
			prebuildcommands { 
				("rm -rf " .. build_path),
			}

