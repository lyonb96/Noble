#pragma once

#include <json/json_fwd.hpp>

namespace Noble
{
	using json = nlohmann::json;

	namespace Config
	{
		
		/**
		 * Loads the config from a file
		 */
		void LoadConfig();

		/**
		 * Saves the config to a file
		 */
		void SaveConfig();

		/**
		 * Returns a JSON object that holds all config data
		 */
		json& GetConfigData();

		/**
		 * Returns a JSON object representing a sub-level of the configuration
		 */
		json& GetSubConfig(const char* sub);
	}
}