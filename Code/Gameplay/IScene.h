#pragma once
class IScene
{
public:
	IScene(){}
	virtual void graphicUpdate(void) = 0;
	virtual void eventUpdate(bool movingDisallowed) = 0;
	virtual ~IScene() {}
};

