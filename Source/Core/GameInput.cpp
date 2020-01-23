#include "GameInput.h"

#include <json/json.hpp>

#include "Logger.h"
#include "Config.h"

namespace Noble
{
	using json = nlohmann::json;

	namespace Input
	{
		namespace
		{
			ActionBinding g_ActionBindings[64];
			U32 g_ActionCount = 0;
			AxisBinding g_AxisBindings[32];
			U32 g_AxisCount = 0;
		}

		ActionBinding::ActionBinding()
			: m_Identifier(), m_IdentifierHash(0)
		{
			// Set all actions to unassigned on creation
			m_Triggers[0] = m_Triggers[1] = m_Triggers[2] = Action::ACTION_UNASSIGNED;
		}

		ActionBinding::ActionBinding(const char* id)
			: m_Identifier(id), m_IdentifierHash(HASH(id))
		{
			// Set all actions to unassigned on creation
			m_Triggers[0] = m_Triggers[1] = m_Triggers[2] = Action::ACTION_UNASSIGNED;
		}

		const U32 ActionBinding::GetIdentifierHash() const
		{
			return m_IdentifierHash;
		}

		const char* ActionBinding::GetIdentifier() const
		{
			return m_Identifier;
		}

		const Action ActionBinding::GetTrigger(U32 index) const
		{
#ifdef _DEBUG
			if (index < 0 || index > 2)
			{
				NE_LOG_ERROR("Index (%u) out of bounds for action binding: %s", index, m_Identifier);
				return Action::ACTION_UNKNOWN;
			}
#endif

			return m_Triggers[index];
		}

		void ActionBinding::SetTrigger(U32 index, Action act)
		{
#ifdef _DEBUG
			if (index < 0 || index > 2)
			{
				NE_LOG_ERROR("Index (%u) out of bounds for action binding: %s", index, m_Identifier);
				return;
			}
#endif

			m_Triggers[index] = act;
		}

		bool ActionBinding::IsPressed() const
		{
			for (U32 i = 0; i < 3; i++)
			{
				if (Input::IsPressed(m_Triggers[i]))
				{
					return true;
				}
			}

			return false;
		}

		bool ActionBinding::IsJustPressed() const
		{
			for (U32 i = 0; i < 3; i++)
			{
				if (Input::IsJustPressed(m_Triggers[i]))
				{
					return true;
				}
			}

			return false;
		}

		bool ActionBinding::IsReleased() const
		{
			for (U32 i = 0; i < 3; i++)
			{
				if (Input::IsReleased(m_Triggers[i]))
				{
					return true;
				}
			}

			return false;
		}

		bool ActionBinding::IsJustReleased() const
		{
			for (U32 i = 0; i < 3; i++)
			{
				if (Input::IsJustReleased(m_Triggers[i]))
				{
					return true;
				}
			}

			return false;
		}

		AxisBinding::AxisBinding()
			: m_Identifier(), m_IdentifierHash(0)
		{
			for (U32 i = 0; i < 6; i++)
			{
				m_Triggers[i].IsAction = false;
				m_Triggers[i].AxisIn = Axis::AXIS_UNASSIGNED;
				m_Triggers[i].Scale = 0.0F;
			}
		}

		AxisBinding::AxisBinding(const char* id)
			: m_Identifier(id), m_IdentifierHash(HASH(id))
		{
			for (U32 i = 0; i < 6; i++)
			{
				m_Triggers[i].IsAction = false;
				m_Triggers[i].AxisIn = Axis::AXIS_UNASSIGNED;
				m_Triggers[i].Scale = 0.0F;
			}
		}

		const U32 AxisBinding::GetIdentifierHash() const
		{
			return m_IdentifierHash;
		}

		const char* AxisBinding::GetIdentifier() const
		{
			return m_Identifier;
		}

		float AxisBinding::GetState() const
		{
			F32 outVal = 0.0F;

			for (U32 i = 0; i < 6; i++)
			{
				F32 outValAbs = glm::abs(outVal);
				if (m_Triggers[i].IsAction)
				{
					// Handle axis as action
					if (IsPressed(m_Triggers[i].ActionIn))
					{
						if (glm::abs(m_Triggers[i].Value) > outValAbs)
						{
							outVal = m_Triggers[i].Value;
						}
					}
				}
				else
				{
					F32 state = GetAxisState(m_Triggers[i].AxisIn) * m_Triggers[i].Scale;
					if (glm::abs(state) > outValAbs)
					{
						outVal = state;
					}
				}
			}

			return outVal;
		}

		void AxisBinding::SetTrigger(U32 index, Axis axis, F32 scale)
		{
#ifdef _DEBUG
			if (index < 0 || index > 2)
			{
				NE_LOG_ERROR("Index (%u) out of bounds for action binding: %s", index, m_Identifier);
				return;
			}
#endif

			m_Triggers[index].IsAction = false;
			m_Triggers[index].AxisIn = axis;
			m_Triggers[index].Scale = scale;
		}

		void AxisBinding::SetTrigger(U32 index, Action action, F32 value)
		{
#ifdef _DEBUG
			if (index < 0 || index > 2)
			{
				NE_LOG_ERROR("Index (%u) out of bounds for action binding: %s", index, m_Identifier);
				return;
			}
#endif

			m_Triggers[index].IsAction = true;
			m_Triggers[index].ActionIn = action;
			m_Triggers[index].Value = value;
		}

		U16 AxisBinding::GetTrigger(U32 index) const
		{
			if (m_Triggers[index].IsAction)
			{
				return m_Triggers[index].ActionIn;
			}
			else
			{
				return m_Triggers[index].AxisIn;
			}
		}

		F32 AxisBinding::GetTriggerValue(U32 index) const
		{
			if (m_Triggers[index].IsAction)
			{
				return m_Triggers[index].Value;
			}
			else
			{
				return m_Triggers[index].Scale;
			}
		}

		bool AxisBinding::IsTriggerAction(U32 index) const
		{
			return m_Triggers[index].IsAction;
		}

		U32 GetActionIndex(const U32 id)
		{
			for (U32 i = 0; i < g_ActionCount; i++)
			{
				if (g_ActionBindings[i].GetIdentifierHash() == id)
				{
					return i;
				}
			}
			return g_ActionCount + 1;
		}

		U32 GetAxisIndex(const U32 id)
		{
			for (U32 i = 0; i < g_AxisCount; i++)
			{
				if (g_AxisBindings[i].GetIdentifierHash() == id)
				{
					return i;
				}
			}
			return g_AxisCount + 1;
		}

		void RegisterActionBinding(const char* id, Action defaultAction)
		{
			g_ActionBindings[g_ActionCount] = ActionBinding(id);
			g_ActionBindings[g_ActionCount++].SetTrigger(0, defaultAction);
		}

		void RegisterAxisBinding(const char* id, Axis defaultAxis, F32 scale)
		{
			g_AxisBindings[g_AxisCount] = AxisBinding(id);
			g_AxisBindings[g_AxisCount++].SetTrigger(0, defaultAxis, scale);
		}

		void RegisterAxisBinding(const char* id, Action defaultAction, F32 value)
		{
			g_AxisBindings[g_AxisCount] = AxisBinding(id);
			g_AxisBindings[g_AxisCount++].SetTrigger(0, defaultAction, value);
		}

		void SetActionBinding(const U32 id, const U32 index, Action action)
		{
			const U32 aIndex = GetActionIndex(id);
			if (aIndex > g_ActionCount)
			{
				NE_LOG_WARNING("No matching Action ID!");
				return;
			}

			g_ActionBindings[aIndex].SetTrigger(index, action);
		}

		void SetAxisBinding(const U32 id, const U32 index, Axis axis, float scale)
		{
			const U32 aIndex = GetAxisIndex(id);
			if (aIndex > g_AxisCount)
			{
				NE_LOG_WARNING("No matching Axis ID!");
				return;
			}

			g_AxisBindings[aIndex].SetTrigger(index, axis, scale);
		}

		void SetAxisBinding(const U32 id, const U32 index, Action action, float value)
		{
			const U32 aIndex = GetAxisIndex(id);
			if (aIndex > g_AxisCount)
			{
				NE_LOG_WARNING("No matching Axis ID! Index: %u. Count: %u", index, g_AxisCount);
				return;
			}
			g_AxisBindings[aIndex].SetTrigger(index, action, value);
		}

		ActionBinding* const GetActionBinding(const U32 id)
		{
			const U32 index = GetActionIndex(id);
			if (index > g_AxisCount)
			{
				NE_LOG_WARNING("No matching Action ID!");
				return nullptr;
			}

			return &g_ActionBindings[index];
		}

		AxisBinding* const GetAxisBinding(const U32 id)
		{
			const U32 index = GetAxisIndex(id);
			if (index > g_AxisCount)
			{
				NE_LOG_WARNING("No matching Axis ID!");
				return nullptr;
			}

			return &g_AxisBindings[index];
		}

		void ParseConfigBinding(char line[128])
		{
			if (StrStartsWith(line, "\tActionBinding"))
			{
				char bindingName[24];
				Action action;
				U32 bindingIndex = 0;

				// begin parsing action binding
				I32 ind = IndexOf(' ', &line[15]) + 15;
				line[ind] = '\0';
				//PRINT_DEBUG("Binding name: " << &line[15] << "\n");
				CopyInto(&line[15], bindingName);
				line[ind] = ' ';
				++ind;

				// Now iterate over the bindings
				I32 endInd = IndexOf('\r', line);
				while (ind != endInd)
				{
					if (line[ind] == '[')
					{
						// start of binding
						I32 endBracketInd = IndexOf(']', &line[ind]);
						// now ind -> endBracketInd should be [ to ]
						// so ind + 1 -> endBracketInd - 1 should be the binding string
						line[ind + endBracketInd] = '\0';

						//PRINT_DEBUG("Binding: " << &line[ind + 1] << "\n");
						action = GetAction(&line[ind + 1]);
						SetActionBinding(HASH(bindingName), bindingIndex, action);
						ind += endBracketInd;
						bindingIndex++;
					}
					else
					{
						// shift index forward until we find a [
						++ind;
					}
				}
			}
			else if (StrStartsWith(line, "\tAxisBinding"))
			{
				// begin parsing axis binding
				char bindingName[24];
				bool isAction = false;
				union
				{
					struct
					{
						Axis axis;
						F32 scale;
					};
					struct
					{
						Action action;
						F32 value;
					};
				};
				U32 bindingIndex = 0;

				// begin parsing axis binding
				I32 ind = IndexOf(' ', &line[13]) + 13;
				line[ind] = '\0';
				CopyInto(&line[13], bindingName);
				line[ind] = ' ';
				++ind;

				I32 lineEnd = IndexOf('\r', line);
				while (ind != lineEnd)
				{
					if (line[ind] == '{')
					{
						// Axis value
						isAction = false;
						I32 comma = IndexOf(',', &line[ind]);
						line[comma + ind] = '\0'; // Set the comma to a null terminator for a sec

						// PROCESS THE BINDING NAME: line[ind+1]
						axis = GetAxis(&line[ind + 1]);
						// done processing
						ind += comma;
						line[ind] = ','; // Set the comma back to a comma
						// Now &line[ind + 1] -> bracketEnd should be the value(???)
						I32 bracketEnd = IndexOf('}', &line[ind]);
						line[ind + bracketEnd] = '\0'; // Set it to null term

						scale = (float)atof(&line[ind + 2]);
						line[ind + bracketEnd] = '}'; // Set it back

						SetAxisBinding(HASH(bindingName), bindingIndex, axis, scale);

						ind += bracketEnd;
						++bindingIndex;
					}
					else if (line[ind] == '[')
					{
						// Action value
						isAction = true;
						I32 comma = IndexOf(',', &line[ind]);
						line[comma + ind] = '\0'; // Set the comma to a null terminator for a sec

						// PROCESS THE BINDING NAME: line[ind+1]
						action = GetAction(&line[ind + 1]);
						// done processing
						ind += comma;
						line[ind] = ','; // Set the comma back to a comma
						// Now &line[ind + 1] -> bracketEnd should be the value(???)
						I32 bracketEnd = IndexOf(']', &line[ind]);
						line[ind + bracketEnd] = '\0'; // Set it to null term

						value = (float)atof(&line[ind + 2]);
						line[ind + bracketEnd] = ']'; // Set it back

						SetAxisBinding(HASH(bindingName), bindingIndex, action, value);

						ind += bracketEnd;
						++bindingIndex;
					}
					else
					{
						++ind;
					}
				}
			}
		}

		void LoadBindings()
		{
			json& inputCfg = Config::GetSubConfig("Input");

			if (inputCfg.size() <= 0)
			{
				return;
			}

			// Read in the number of bindings
			g_ActionCount = inputCfg["ActionCount"];
			g_AxisCount = inputCfg["AxisCount"];

			// Parse action bindings
			for (json::iterator it = inputCfg["Action"].begin(); it != inputCfg["Action"].end(); ++it) // for each action binding
			{
				RegisterActionBinding(it.key().c_str(), Action::ACTION_UNASSIGNED); // Register with a blank binding
				U32 bindingHash = HASH(it.key().c_str()); // Hash the name for easy access

				U32 bindingIndex = 0;
				for (json::iterator binding = (*it)[0].begin(); binding != (*it)[0].end(); ++binding) // for each trigger
				{
					// Read in the bound action and set it to the corresponding index
					SetActionBinding(bindingHash, bindingIndex++, 
						Input::GetAction(binding->get<std::string>().c_str())
					);
				}
			}

			// Now parse axis bindings
			for (json::iterator it = inputCfg["Axis"].begin(); it != inputCfg["Axis"].end(); ++it)
			{
				// Grab the name and hash it for easy access
				const char* bindingName = it.key().c_str();
				U32 bindingHash = HASH(it.key().c_str());

				// Register with a blank binding
				RegisterAxisBinding(bindingName, Axis::AXIS_UNASSIGNED, 0.0F);

				// Grab a ref to the array holding the triggers
				json& bindingArray = (*it);

				for (U32 bindingIndex = 0; bindingIndex < bindingArray.size(); bindingIndex++) // for each trigger
				{
					// Check if it's Action-As-Axis
					bool isAction = inputCfg["Axis"][bindingName][bindingIndex]["Action"];
					if (isAction)
					{
						// Extract the trigger from the JSON config
						Action act = Input::GetAction(
							inputCfg["Axis"][bindingName][bindingIndex]["Binding"].get<std::string>().c_str()
						);
						// Extract the scale
						float scale = inputCfg["Axis"][bindingName][bindingIndex]["Scale"];

						// Set the binding info for the corresponding trigger index
						SetAxisBinding(bindingHash, bindingIndex, act, scale);
					}
					else
					{
						// Extract the trigger from the JSON config
						Axis ax = Input::GetAxis(
							inputCfg["Axis"][bindingName][bindingIndex]["Binding"].get<std::string>().c_str()
						);
						// Extract the scale
						float scale = inputCfg["Axis"][bindingName][bindingIndex]["Scale"];

						// Set the binding info for the corresponding trigger index
						SetAxisBinding(bindingHash, bindingIndex, ax, scale);
					}
				}
			}
		}

		void SaveBindings()
		{
			// Grab the input sub-level of the config data
			json& inputCfg = Config::GetSubConfig("Input");
			// Clear all of its data
			inputCfg.clear();

			// Write the number of action and axis bindings
			inputCfg["ActionCount"] = g_ActionCount;
			inputCfg["AxisCount"] = g_AxisCount;

			U32 ind = 0;
			for (; ind < g_ActionCount; ind++) // for each action binding
			{
				for (U32 binding = 0; binding < 3; binding++) // for each trigger on the binding
				{
					if (g_ActionBindings[ind].GetTrigger(binding) != ACTION_UNASSIGNED) // make sure the trigger is set
					{
						// Store the trigger in the following format
						// Config -> Input -> Action -> [Binding Name] -> [Binding Index]
						inputCfg["Action"][g_ActionBindings[ind].GetIdentifier()][binding] =
							Input::GetActionString(g_ActionBindings[ind].GetTrigger(binding));
					}
				}
			}

			for (ind = 0; ind < g_AxisCount; ind++) // for each axis binding
			{
				for (U32 binding = 0; binding < 3; binding++) // for each trigger on the binding
				{
					if (g_AxisBindings[ind].IsTriggerAction(binding)) // check if the trigger is an Action-As-Axis
					{
						if (g_AxisBindings[ind].GetTrigger(binding) == ACTION_UNASSIGNED) // make sure the trigger is set
						{
							continue;
						}

						// Store binding type
						inputCfg["Axis"][g_AxisBindings[ind].GetIdentifier()][binding]["Action"] = true;
						
						// store the trigger name
						inputCfg["Axis"][g_AxisBindings[ind].GetIdentifier()][binding]["Binding"] =
							Input::GetActionString((Action)g_AxisBindings[ind].GetTrigger(binding));
						// store the scale
						inputCfg["Axis"][g_AxisBindings[ind].GetIdentifier()][binding]["Scale"] =
							g_AxisBindings[ind].GetTriggerValue(binding);
					}
					else
					{
						if (g_AxisBindings[ind].GetTrigger(binding) == AXIS_UNASSIGNED) // make sure the trigger is set
						{
							continue;
						}

						// Store binding type
						inputCfg["Axis"][g_AxisBindings[ind].GetIdentifier()][binding]["Action"] = false;

						// store the trigger name
						inputCfg["Axis"][g_AxisBindings[ind].GetIdentifier()][binding]["Binding"] =
							Input::GetAxisString((Axis)g_AxisBindings[ind].GetTrigger(binding));
						// store the scale
						inputCfg["Axis"][g_AxisBindings[ind].GetIdentifier()][binding]["Scale"] =
							g_AxisBindings[ind].GetTriggerValue(binding);
					}
				}
			}
		}
	}
}