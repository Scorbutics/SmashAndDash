#pragma once
namespace ska {
	typedef char ScriptTriggerType;
	namespace EnumScriptTriggerType
	{
		static const ScriptTriggerType AUTO = 0;
		static const ScriptTriggerType ACTION = 1;
		static const ScriptTriggerType MOVE = 2;
		static const ScriptTriggerType NONE = 3;
	}
}