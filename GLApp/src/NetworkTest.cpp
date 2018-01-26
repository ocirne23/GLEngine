#include "NetworkTest.h"

#include "Core.h"

#include "GLEngine.h"
#include "Input/Input.h"
#include "Network/TCPSocket.h"

NetworkTest::NetworkTest()
{
	print("Elloh wurld\n");

	GLEngine::createThread("TCPListenThread", []()
	{
		byte buffer[512];

		TCPSocket s;
		s.listen("localhost", 23232);
		while (!GLEngine::isShutdown() && s.receive(buffer, [](gsl::span<byte> data)
		{
			print("received: %i - %s\n", data.length_bytes(), rcast<const char*>(data.data()));
		}));
	});

	GLEngine::createThread("TCPSendThread", []()
	{
		byte data[] = "ehuehue";

		TCPSocket s;
		s.connect("localhost", 23232);
		for (uint i = 0; i < 10; ++i)
		{
			s.send(data);
		}
	});
}

NetworkTest::~NetworkTest()
{
}

void NetworkTest::update(float a_deltaSec)
{
	if (GLEngine::input->isKeyPressed(EKey::ESCAPE))
		GLEngine::shutdown();
}