#pragma once

#include "Core.h"
#include "Utils/ListenerMacros.h"

#ifdef EDITOR
#include "Editor/EditorPipeline.h"
#endif

class Editor
{
public:
	Editor();

	void quit();
	void updateUIPosition();

	void sendTest0Message();
	void sendTest1Message();

	DECLARE_LISTENER(TestButtonClick, void);
	DECLARE_LISTENER(ResizeButtonClick, void);

private:

#ifdef EDITOR
	EditorPipeline m_pipeLine;
#endif

};