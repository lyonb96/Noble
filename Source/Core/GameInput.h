#pragma once

#include "Input.h"
#include "String.h"

namespace Noble
{
	namespace Input
	{
		/**
		 * Wraps the functionality for binding an action binding
		 */
		class ActionBinding
		{
		private:

			// Array of triggers, all default to unassigned
			Action m_Triggers[3];
			// String identifier
			NStringFixed<16> m_Identifier;
			// Hash of the identifier
			U32 m_IdentifierHash;

		public:

			ActionBinding();

			explicit ActionBinding(const char* id);

			/**
			 * Returns a pre-computed hash of the binding's identifier
			 */
			const U32 GetIdentifierHash() const;

			/**
			 * Returns the binding's identifier as a c-string
			 */
			const char* GetIdentifier() const;

			/**
			 * Returns the action input bound to the given index
			 */
			const Action GetTrigger(U32 index) const;

			/**
			 * Sets the trigger at the given index to a new value
			 */
			void SetTrigger(U32 index, Action act);

			/**
			 * Returns true if the input is pressed - will return true if any single trigger is true
			 */
			bool IsPressed() const;

			/**
			 * Returns true if the input is JUST pressed - will return true if any single trigger is true
			 */
			bool IsJustPressed() const;

			/**
			 * Returns true if the input is released - will return true if any single trigger is true
			 */
			bool IsReleased() const;

			/**
			 * Returns true if the input is JUST released - will return true if any single trigger is true
			 */
			bool IsJustReleased() const;
		};

		/**
		 * Wraps the functionality of an axis binding
		 */
		struct AxisBinding
		{
		private:

			/**
			 * Yeah it's a little ugly, but it gets the job done
			 */
			struct AxisTrigger
			{
				union
				{
					struct
					{
						Action ActionIn;
						F32 Value;
					};
					struct
					{
						Axis AxisIn;
						F32 Scale;
					};
				};
				bool IsAction;
			} m_Triggers[6];

			// String identifier
			NStringFixed<16> m_Identifier;
			// Hash of the string identifier
			U32 m_IdentifierHash;

		public:

			AxisBinding();

			explicit AxisBinding(const char* id);

			/**
			 * Returns a pre-computed hash of the binding's identifier
			 */
			const U32 GetIdentifierHash() const;

			/**
			 * Returns the binding's identifier as a c-string
			 */
			const char* GetIdentifier() const;

			/**
			 * Returns the state of the binding (uses largest absolute value of all bindings)
			 */
			float GetState() const;

			/**
			 * Sets the trigger at the given index to be an action-as-axis binding
			 */
			void SetTrigger(U32 index, Axis axis, F32 scale);

			/**
			 * Sets the trigger at the given index to be a natural axis binding
			 */
			void SetTrigger(U32 index, Action action, F32 value);

			/**
			 * Returns a generic (U16) representation of the given trigger, use in concert with IsTriggerAction
			 */
			U16 GetTrigger(U32 index) const;

			/**
			 * Returns the value/scale of the given trigger
			 */
			F32 GetTriggerValue(U32 index) const;

			/**
			 * Returns true if the given trigger is action, false if axis
			 */
			bool IsTriggerAction(U32 index) const;
		};

		/**
		 * Registers a new action binding with the given ID and default binding
		 */
		void RegisterActionBinding(const char* id, Action defaultAction);

		/**
		 * Registers a new axis binding with the given ID and default binding
		 */
		void RegisterAxisBinding(const char* id, Axis defaultAxis, F32 scale);

		/**
		 * Registers a new axis binding with the given ID and default binding
		 */
		void RegisterAxisBinding(const char* id, Action defaultAction, F32 value);

		/**
		 * Sets a trigger on the given action binding
		 * The "id" parameter should be filled with the SID() macro
		 */
		void SetActionBinding(const U32 id, const U32 index, Action action);

		/**
		 * Sets a natural axis trigger for the given axis binding
		 * The "id" parameter should be filled with the SID() macro
		 */
		void SetAxisBinding(const U32 id, const U32 index, Axis axis, float scale);

		/**
		 * Sets an action trigger acting as an axis for the given axis binding
		 * The "id" parameter should be filled with the SID() macro
		 */
		void SetAxisBinding(const U32 id, const U32 index, Action action, float value);

		/**
		 * Returns a pointer to the action binding of the given ID
		 * The "id" parameter should be filled with the SID() macro
		 */
		ActionBinding* const GetActionBinding(const U32 id);

		/**
		 * Returns a pointer to the axis binding of the given ID
		 * The "id" parameter should be filled with the SID() macro
		 */
		AxisBinding* const GetAxisBinding(const U32 id);

		/**
		 * Used by the Config Manager to load in settings from the config file
		 */
		void ParseConfigBinding(char line[128]);

		/**
		 * Loads the bindings from the config file
		 */
		void LoadBindings();

		/**
		 * Saves the bindings to the config file
		 */
		void SaveBindings();
	}
}