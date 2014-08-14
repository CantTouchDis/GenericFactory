solution "ElTrazadoDeRayos"

language "C++"
location("./" .. _ACTION)  -- Where to put the project files.
flags {"StaticRuntime"}

-- Add the c++ 11 standard if we use gmake.
if (_ACTION == "gmake") then
  buildoptions {"-std=c++0x -std=gnu++0x"}
end

-- Define which OS we use.
if os.get() == "windows" then
  defines {"WINDOWS"}
elseif os.get() == "linux" then 
  defines {"LINUX"}
elseif os.get() == "macosx" then
  defines {"MACOSX"}
end

-- Configurations.
configurations {"Release", "Debug", "Profile"}

-- Platforms
platforms { "native", "universal" }
-- platforms {"x32", "x64"}

configuration "Release"
  defines {"NDEBUG"}
  flags {"Optimize"}
  targetdir "../bin/release"

configuration { "gmake",  "release" }

configuration "Debug"
  defines {"DEBUG"}
  flags {"Symbols"}
  targetdir "../bin/debug"
  
configuration { "gmake",  "Debug" }

configuration "Profile"
  defines {"PROF"}
  flags {"Symbols", "Optimize"}
  targetdir "../bin/prof"
  
configuration { "gmake",  "Profile" }
  buildoptions { "-pg" }
  linkoptions { "-pg" }

-- Enable openmp for visual studio project files.
configuration { "vs*",  "release" }

-- Projects.
project "GenericFactory"
  files {"../src/**"}
  kind "ConsoleApp"
  if os.get() == "windows" then
    postbuildcommands { "py ..\\cpplintHelper.py --root=src ..\\..\\src" }
  elseif os.get() == "linux" then 
    postbuildcommands { "-python2 ../cpplintHelper.py --root=src ../../src > /dev/null" }
  end

  -- This is nice to have so VS always uses the same uuids in its project files.
  -- Generated via http://www.uuidgenerator.net
  uuid("646c20b8-af6e-4110-8044-fc1ccf05b5d3")

