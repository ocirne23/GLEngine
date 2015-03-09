#include "Editor/Editor.h"

#ifdef EDITOR

#include "Utils/WindowsPlatformData.h"

Editor::Editor()
{

}

void Editor::quit()
{
	WindowsPlatformData::quitEditor();
}

void Editor::updateUIPosition()
{
	WindowsPlatformData::setEditorTop();
}

void Editor::sendTest0Message()
{
	m_pipeLine.writeCommand(EditorCommands::CommandType_TEST0, 0, 0);
}

void Editor::sendTest1Message()
{
	rde::string da("heuhuehue");
	m_pipeLine.writeCommand(EditorCommands::CommandType_TEST1, da.length(), (byte*) da.c_str());
}

#endif // EDITOR