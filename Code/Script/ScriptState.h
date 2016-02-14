#ifndef DEF_SCRIPTSTATE
#define DEF_SCRIPTSTATE

typedef char ScriptState;
namespace EnumScriptState
{
	static const ScriptState RUNNING = 0;
	static const ScriptState PAUSED = 1;
	static const ScriptState STOPPED = 2;
}


#endif