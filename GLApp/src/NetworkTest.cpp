#include "NetworkTest.h"

#include "Core.h"

#include "GLEngine.h"
#include "Input/Input.h"
#include "Network/TCPSocket.h"
#include <glm/gtc/random.hpp>
static int ctr = 1;
#pragma pack(1)
struct Header
{
	Header(short a_id, short a_size) : header(0xfefefe), id(a_id), size(a_size) {}
	Header() {}
	int header = 0;
	int id = 0;
	int size = 0;
};

struct DataA
{
	int a = 0xf0f0f0;
	short b = 4;
	byte c = 3;
	int64 d = 23;
};

struct DataB
{
	int a = 0xfafafa;
	short b = 4;
	byte c = 3;
	int64 d = 23;
	byte e = 3;
	byte f = 3;

};
struct PacketA
{
	PacketA() : h(1, sizeof(a)) {}
	Header h;
	DataA a;
};

struct PacketB
{
	PacketB() : h(2, sizeof(b)) {}
	Header h;
	DataB b;
};
NetworkTest::NetworkTest()
{
	print("Elloh wurld\n");

	GLEngine::sleep(1000);

	GLEngine::createThread("TCPListenThread", []()
	{
		TCPSocket s;
		s.listen("localhost", 23235);

		Header h;
		while (!GLEngine::isShutdown() && s.receive())
		{
			if (h.header != 0)
			{
				switch (h.id)
				{
				case 1:
				{
					DataA a;
					if (s.readFromMessageQueue(span<byte>(rcast<byte*>(&a), sizeof(a))))
					{
						print("Read packet: %i, %i, %i\n", h.id, h.size, s.m_ringQueue.headIdx());
						h.header = 0;
					}
					break;
				}
				case 2:
				{
					DataB b;
					if (s.readFromMessageQueue(span<byte>(rcast<byte*>(&b), sizeof(b))))
					{
						print("Read packet: %i, %i, %i\n", h.id, h.size, s.m_ringQueue.headIdx());
						h.header = 0;
					}
					break;
				}
				default:
					assert(false);
					s.disconnect();
					h.header = 0;
					print("disconnected");
					return;
				}
			}
			else if(s.readFromMessageQueue(span<byte>(rcast<byte*>(&h), sizeof(h))))
			{
				print("Read header: %i, %i, %i, %i\n", h.header, h.id, h.size, s.m_ringQueue.headIdx());
			}
		}
	});
	GLEngine::sleep(1000);
	
	GLEngine::createThread("TCPSendThread", [&]()
	{
		TCPSocket s;
		s.connect("localhost", 23235);
		
		while (!GLEngine::isShutdown())
		{
			//GLEngine::sleep(100);
			PacketA p;
			PacketB p2;
			if (glm::linearRand(0, 1))
				s.send(span<const byte>(rcast<const byte*>(&p), sizeof(p)));
			else
			{
			}//s.send(span<const byte>(rcast<const byte*>(&p2), sizeof(p2)));
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