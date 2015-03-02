#include "Editor/EditorPipeline.h"

#include "Core.h"
#include <SDL/SDL_thread.h>
#include <windows.h>

BEGIN_UNNAMED_NAMESPACE()
const char* const ENGINE_TO_EDITOR_PIPENAME = "\\\\.\\pipe\\EngineToEditorPipeline";
const char* const EDITOR_TO_ENGINE_PIPENAME = "\\\\.\\pipe\\EditorToEnginePipeline";
END_UNNAMED_NAMESPACE()

EditorPipeline::EditorPipeline()
{
	m_outboundThread.m_commandQueueLock = SDL_CreateSemaphore(1);
	m_outboundThread.m_commandQueueNotifier = SDL_CreateSemaphore(0);
	m_outboundThread.m_thread = SDL_CreateThread(&EditorPipeline::OutboundThread, "OutboundThread", &m_outboundThread);

	m_inboundThread.m_commandQueueLock = SDL_CreateSemaphore(1);
	m_inboundThread.m_commandQueueNotifier = SDL_CreateSemaphore(0);
	m_inboundThread.m_thread = SDL_CreateThread(&EditorPipeline::InboundThread, "OutboundThread", &m_inboundThread);
}

void EditorPipeline::writeCommand(EditorCommands::CommandType a_commandType, uint a_numBytes, byte* a_bytes)
{
	if (!m_outboundThread.m_connected)
		return;

	SDL_SemWait(m_outboundThread.m_commandQueueLock);
	byte* contents = new byte[a_numBytes + 2 * sizeof(uint)];
	*((uint*) contents) = a_commandType;
	*((uint*) (contents + 4)) = a_numBytes;
	if (a_numBytes > 0)
		memcpy(contents + 2 * sizeof(uint), a_bytes, a_numBytes);

	m_outboundThread.m_commandQueue.push_back(contents);

	SDL_SemPost(m_outboundThread.m_commandQueueLock);
	SDL_SemPost(m_outboundThread.m_commandQueueNotifier);
}

int EditorPipeline::OutboundThread(void* a_ptr)
{
	EditorPipeline::PipelineThread& pipelineThread = *((EditorPipeline::PipelineThread*) a_ptr);

	HANDLE pipelineHandle = CreateNamedPipe(
		ENGINE_TO_EDITOR_PIPENAME, // name of the pipe
		PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED, // 1-way pipe, read
		PIPE_TYPE_BYTE, // send data as a byte stream
		1, // only allow 1 instance of this pipe
		0, // no outbound buffer
		0, // no inbound buffer
		PIPE_WAIT,
		NULL // use default security attributes
		);

	if (pipelineHandle == NULL || pipelineHandle == INVALID_HANDLE_VALUE)
	{
		print("Failed to create outbound pipe instance. Error code: %i \n", GetLastError());
		pipelineThread.m_running = false;
	}
	else
	{
		print("Waiting for a client to connect to the pipe...\n");

		OVERLAPPED overlapped = {0};
		overlapped.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

		BOOL connectResult = ConnectNamedPipe(pipelineHandle, &overlapped);
		if (!connectResult)
		{
			DWORD err = GetLastError();
			if (err == ERROR_IO_PENDING || err == ERROR_PIPE_LISTENING)
			{
				while (pipelineThread.m_running && (WaitForSingleObject(overlapped.hEvent, 1) != WAIT_OBJECT_0));
			}
			else
			{
				pipelineThread.m_running = false;
				print("Failed to wait for connection \n");
			}
		}

		if (pipelineThread.m_running)
		{
			pipelineThread.m_connected = true;
			print("Client connected to the pipe\n");
		}
	}

	while (pipelineThread.m_running)
	{
		SDL_SemWait(pipelineThread.m_commandQueueNotifier);
		SDL_SemWait(pipelineThread.m_commandQueueLock);
		if (!pipelineThread.m_running)
			break;

		byte* command = pipelineThread.m_commandQueue.front();

		uint commandID = *((uint*) command);
		uint numBytes = *((uint*) (command + 4));

		print("Sending data to pipe...\n");

		DWORD numBytesWritten = 0;
		BOOL writeResult = WriteFile(
			pipelineHandle, // handle to our outbound pipe
			command, // data to send
			(DWORD) (numBytes * sizeof(byte) + 2 * sizeof(uint)), // length of data to send (bytes)
			&numBytesWritten, // will store actual amount of data sent
			NULL // not using overlapped IO
			);

		if (writeResult)
		{
			assert(numBytesWritten == numBytes + 2 * sizeof(uint));
			pipelineThread.m_commandQueue.pop_front();
			delete [] command;
			SDL_SemPost(pipelineThread.m_commandQueueLock);
		}
		else
		{
			print("Disconnected from client.. waiting for reconnect\n");
			pipelineThread.m_commandQueue.clear();
			pipelineThread.m_connected = false;
			SDL_SemPost(pipelineThread.m_commandQueueLock);

			DisconnectNamedPipe(pipelineHandle);

			// This call blocks until a client process connects to the pipe
			BOOL result = ConnectNamedPipe(pipelineHandle, NULL);
			pipelineThread.m_connected = true;
			if (!result)
			{
				print("Failed to make connection on named pipe.\n");
				pipelineThread.m_running = false;
			}
		}
	}
	CloseHandle(pipelineHandle); // close the pipe
	pipelineThread.m_stopped = true;
	return 0;
}

int EditorPipeline::InboundThread(void* a_ptr)
{
	EditorPipeline::PipelineThread& pipelineThread = *((EditorPipeline::PipelineThread*) a_ptr);
	HANDLE handle = CreateFile(EDITOR_TO_ENGINE_PIPENAME,
							   GENERIC_READ,
							   FILE_SHARE_READ | FILE_SHARE_WRITE,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL);
}

EditorPipeline::~EditorPipeline()
{
	m_outboundThread.m_running = false;
	if (!m_outboundThread.m_stopped)
		printNow("Waiting for pipeline thread to stop..\n");
	while (!m_outboundThread.m_stopped)
	{
		SDL_SemPost(m_outboundThread.m_commandQueueLock);
		SDL_SemPost(m_outboundThread.m_commandQueueNotifier);
	}
}
