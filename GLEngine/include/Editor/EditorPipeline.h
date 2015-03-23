#pragma once

#include "Core.h"

#include "Utils/ConcurrentQueue.h"

struct Command;
struct SDL_semaphore;
struct SDL_Thread;

class EditorPipeline
{
public:
	enum { MAX_COMMAND_SIZE = 1024 };

	EditorPipeline();
	~EditorPipeline();

	void writeCommand(uint commandID, uint numBytes, byte* bytes);

	struct PipelineThread
	{
		bool m_running   = true;
		bool m_stopped   = false;
		bool m_connected = false;

		SDL_semaphore* m_commandWrittenNotifier;

		ConcurrentQueue<byte*> m_concurrentQueue;
		SDL_Thread* m_thread;
	};

private:

	static int OutboundThread(void* ptr);
	static int InboundThread(void* ptr);

private:

	PipelineThread m_outboundThread;
	PipelineThread m_inboundThread;
};