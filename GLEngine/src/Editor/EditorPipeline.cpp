#include "Editor/EditorPipeline.h"

#include "Core.h"
#include <SDL/SDL_thread.h>
#include <windows.h>

BEGIN_UNNAMED_NAMESPACE()
const char* const ENGINE_TO_EDITOR_PIPENAME = "\\\\.\\pipe\\EngineToEditorPipeline";
const char* const EDITOR_TO_ENGINE_PIPENAME = "\\\\.\\pipe\\EditorToEnginePipeline";

/** Will block waiting for a connection untill a_running is set to false or an IO error occurs
	Returns if a client has connected */
bool waitForConnection(HANDLE a_pipe, const bool& a_running)
{
    OVERLAPPED overlapped = {0};
    overlapped.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    BOOL connectResult = ConnectNamedPipe(a_pipe, &overlapped);
    if (!connectResult)
    {
	DWORD err = GetLastError();
	if (err == ERROR_IO_PENDING || err == ERROR_PIPE_LISTENING)
	{
	    while (a_running && (WaitForSingleObject(overlapped.hEvent, 1) != WAIT_OBJECT_0));

	    return a_running;
	}
	else
	{
	    return false;
	    print("Failed to wait for connection \n");
	}
    }

    return a_running;
}

END_UNNAMED_NAMESPACE()

EditorPipeline::EditorPipeline()
{
    m_outboundThread.m_commandWrittenNotifier = SDL_CreateSemaphore(0);
    m_outboundThread.m_thread = SDL_CreateThread(&EditorPipeline::OutboundThread, "OutboundThread", &m_outboundThread);

    m_inboundThread.m_commandWrittenNotifier = SDL_CreateSemaphore(0);
    m_inboundThread.m_thread = SDL_CreateThread(&EditorPipeline::InboundThread, "OutboundThread", &m_inboundThread);
}

void EditorPipeline::writeCommand(EditorCommands::CommandType a_commandType, uint a_numBytes, byte* a_bytes)
{
    if (!m_outboundThread.m_connected)
	return;

    byte* contents = new byte[a_numBytes + 2 * sizeof(uint)];
    *((uint*) contents) = a_commandType;
    *((uint*) (contents + 4)) = a_numBytes;
    if (a_numBytes)
	memcpy(contents + 2 * sizeof(uint), a_bytes, a_numBytes);

    m_outboundThread.m_concurrentQueue.push_back(contents);

    SDL_SemPost(m_outboundThread.m_commandWrittenNotifier);
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

	if (waitForConnection(pipelineHandle, pipelineThread.m_running))
	{
	    print("Client connected to the pipe\n");
	    pipelineThread.m_connected = true;
	}
	else
	{
	    print("Failed to wait for connection \n");
	    pipelineThread.m_running = false;
	}
    }

    while (pipelineThread.m_running)
    {
	SDL_SemWait(pipelineThread.m_commandWrittenNotifier);

	if (!pipelineThread.m_running)
	    break;

	byte* command = pipelineThread.m_concurrentQueue.pop_front();
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
	    delete [] command;
	}
	else
	{
	    print("Disconnected from client.. waiting for reconnect\n");
	    DisconnectNamedPipe(pipelineHandle);

	    pipelineThread.m_concurrentQueue.clear();
	    pipelineThread.m_connected = false;

	    // This call blocks until a client process connects to the pipe
	    if (waitForConnection(pipelineHandle, pipelineThread.m_running))
	    {
		print("Client connected to the pipe\n");
		pipelineThread.m_connected = true;
	    }
	    else
	    {
		print("Failed to wait for connection \n");
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
    /*
    EditorPipeline::PipelineThread& pipelineThread = *((EditorPipeline::PipelineThread*) a_ptr);
    HANDLE handle = CreateFile(EDITOR_TO_ENGINE_PIPENAME,
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL);
    */
    return 0;
}

EditorPipeline::~EditorPipeline()
{
    m_outboundThread.m_running = false;
    if (!m_outboundThread.m_stopped)
	printNow("Waiting for pipeline thread to stop..\n");
    while (!m_outboundThread.m_stopped)
    {
	SDL_SemPost(m_outboundThread.m_commandWrittenNotifier);
    }
}
