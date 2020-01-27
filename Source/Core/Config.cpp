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
				File configFile("JSON Test.txt", FileMode::FILE_READ);
				Size configSize = configFile.GetFileSize();
				Size bugSize = CheckFileSize("JSON Test.txt");

				char* data = static_cast<char*>(Memory::Malloc(configSize + 1, NOBLE_DEFAULT_ALIGN));
				Size bytesRead = configFile.Read(data, configSize + 1);
				data[bytesRead] = '\0';

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
			File configFile("JSON Test.txt", FileMode::FILE_WRITE_REPLACE, true);

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