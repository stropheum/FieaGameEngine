#pragma once
#include "Action.h"


namespace Library
{
	class ActionList :
		public Action
	{
		RTTI_DECLARATIONS(ActionList, Action)

	public:
		ActionList();
		~ActionList() = default;
		virtual void update(WorldState& worldState) override;

		/// Method for adding actions to the action list
		/// @Param className: The name of the base class being instantiated
		/// @Param instanceName: The name that the action will be instantiated with
		/// @Return: A freshly instantiated action object
		class Action* createAction(const std::string& className, const std::string& instanceName);
	};

	/// Macro for creating a factory for action list objects
	ActionFactory(ActionList)
}
