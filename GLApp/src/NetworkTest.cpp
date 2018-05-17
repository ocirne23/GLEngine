#include "NetworkTest.h"

#include "Core.h"

#include "GLEngine.h"
#include "Input/Input.h"
#include "Network/TCPReceiveSocket.h"
#include "Network/Protocol.h"

#include <glm/gtc/random.hpp>

#pragma pack(push, 1)
struct MyHeader
{
	MyHeader(short a_id = 0)
		: id(a_id)
	{}
	int magic = 0xc0ffee;
	short id = 0;

	bool isValid() const { return id != 0; }
	bool invalidate() { id = 0; }
};

class PacketReceiver : public IPacketReceiver<MyHeader>
{
public:
	PacketReceiver() {}

	virtual size_t getPacketSize(const MyHeader& header) override final
	{
		return 0;
	}

	virtual void receivePacket(span<byte>& data) override final
	{

	}
};

#pragma pack(pop)


NetworkTest::NetworkTest()
{
	print("Elloh wurld\n");

	GLEngine::createThread("TCPListenThread", []()
	{
		PacketReceiver r;
		TCPReceiveSocket<512> s;
		Protocol<MyHeader, 512, PacketReceiver> p(s, r);

		s.listen("localhost", 23235);

		while (!GLEngine::isShutdown() && s.receive())
		{
			p.process();
		}
	});
	GLEngine::sleep(1000);
	GLEngine::createThread("TCPSendThread", [&]()
	{
		TCPSocket s;
		s.connect("localhost", 23235);
		while (!GLEngine::isShutdown())
		{
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