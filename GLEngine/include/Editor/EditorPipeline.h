#pragma once

#include "Core.h"

#include "Editor/EditorCommands.h"

#include "rde/list.h"

struct Command;
struct SDL_semaphore;
struct SDL_Thread;

class EditorPipeline
{
public:
	enum { MAX_COMMAND_SIZE = 1024 };

	EditorPipeline();
	~EditorPipeline();

	void writeCommand(EditorCommands::CommandType commandType, uint numBytes, byte* bytes);
	const rde::list<byte*>& 

	struct PipelineThread
	{
		bool m_running = true;
		bool m_stopped = false;
		bool m_connected = false;

		SDL_semaphore* m_commandQueueLock;
		SDL_semaphore* m_commandQueueNotifier;
		rde::list<byte*> m_commandQueue;
		SDL_Thread* m_thread;
	};

private:

	static int OutboundThread(void* ptr);
	static int InboundThread(void* ptr);

private:

	PipelineThread m_outboundThread;
	PipelineThread m_inboundThread;
};