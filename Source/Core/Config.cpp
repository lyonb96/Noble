#include "Config.h"

#include <json/json.hpp>

#include "FileIO.h"
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

			File configFile("JSON Test.txt");
			if (configFile)
			{
				configFile.ReadIntoJson(g_ConfigData);
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
			File configFile("JSON Test.txt", true, true);

			configFile << g_ConfigData.dump(4);
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