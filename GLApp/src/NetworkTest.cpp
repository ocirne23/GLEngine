#include "NetworkTest.h"

#include "Core.h"

#include "GLEngine.h"
#include "Input/Input.h"
#include "Network/TCPSocket.h"
#include <glm/gtc/random.hpp>

static int ctr = 1;
#pragma pack(push, 1)
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
	byte data[255];
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
#pragma pack(pop)

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
					if (s.read(as_span(rcast<byte*>(&a), sizeof(a))))
					{
						h.header = 0;
					}
					break;
				}
				case 2:
				{
					DataB b;
					if (s.read(as_span(rcast<byte*>(&b), sizeof(b))))
					{
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
			else if(s.read(as_span(rcast<byte*>(&h), sizeof(h))))
			{
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
			PacketA p;
			PacketB p2;
			if (glm::linearRand(0, 1))
				s.send(as_span(rcast<const byte*>(&p), sizeof(p)));
			else
				s.send(as_span(rcast<const byte*>(&p2), sizeof(p2)));
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