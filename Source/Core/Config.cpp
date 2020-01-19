#include "Config.h"

#include <json/json.hpp>

#include "FileSystem.h"
#include "Logger.h"

namespace Noble
{
	// Config files are in JSON
	using json = nlohmann::json;

	namespace Config
	{
		json g_ConfigData;

		void LoadConfig()
		{
			NE_LOG_INFO("Loading configuration from file");

			bool configExists = CheckExists("JSON Test.txt");
			if (configExists)
			{
				Size configSize = CheckFileSize("JSON Test.txt");
				File configFile/*("JSON Test.txt")*/;
				configFile.OpenFile("JSON Test.txt", false);

				char* data = static_cast<char*>(Memory::Malloc(configSize + 1, NOBLE_DEFAULT_ALIGN));
				configFile.Read((U8*)data, configSize);
				data[configSize] = '\0';

				g_ConfigData = json::parse(data);
				
				Memory::Free(data);
			}
			else
			{
				NE_LOG_INFO("Config File not found. Setting defaults.");
				// LoadDefaultConfig();
			}
		}

		void SaveConfig()
		{
			NE_LOG_INFO("Saving config to file");
			File configFile/*("JSON Test.txt", true, true)*/;
			configFile.OpenFile("JSON Test.txt", false, FileMode::READWRITE);

			std::string str = g_ConfigData.dump(4);
			configFile.Write(str.c_str(), str.length());
		}

		json& GetConfigData()
		{
			return g_ConfigData;
		}

		json& GetSubConfig(const char* sub)
		{
			return g_ConfigData[sub];
		}
	}
}