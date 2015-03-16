#pragma once

#include "Core.h"

#ifdef EDITOR

#include "Utils/ListenerMacros.h"
#include "Editor/EditorPipeline.h"

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

	EditorPipeline m_pipeLine;
};

#endif // EDITOR