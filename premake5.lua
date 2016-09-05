-- premake5.lua

--test
--[[
print("------->",path.getabsolute("DomiEngine/network"))
print("------->",os.matchdirs("DomiEngine/*"))

for k,v in pairs(os.matchdirs("common/*")) do
	print(v,path.getdirectory(v),path.getname(v),os.getcwd(v))
end
--]]


-- solution
solution("DomiServer")
	configurations { "Debug", "Release" }
	location "build"

	--消除vs警告
	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS", "_WINSOCK_DEPRECATED_NO_WARNINGS", "WIN32" }
	
	configuration { "linux", "gmake" }
		linkoptions  { "-lrt"}
		--buildoptions { "-std=c++11"}		

-- domiserver
project "DomiServer"
	kind "ConsoleApp"
	language "C++"
	targetdir "Output/%{cfg.buildcfg}"
	targetname "domiserver"

	-- 包含目录列表
	includedirs {
		"3rd/include",
	}
	-- 库目录列表
	libdirs {"3rd/lib"}

	files { "**.cpp", "**.h", "**.c"}
	if os.get() == "windows" then	-- windows
		local vpathsTable = {}
		
		-- common
		for k,v in pairs(os.matchdirs("common/*")) do
			vpathsTable[v] = {v.."*/*"}
		end
		
		-- network
		vpathsTable["network"] = {"network/*.*"}
		vpathsTable["logic"] = {"logic/*.h","logic/*.cpp"}
		
		vpaths(vpathsTable)
	end

	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
