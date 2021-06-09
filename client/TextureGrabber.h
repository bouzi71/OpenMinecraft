#ifndef CLIENT_TEXTUREGRABBER_H_
#define CLIENT_TEXTUREGRABBER_H_

#include <mclib/protocol/PacketHandler.h>

namespace example
{

class TextureGrabber 
	: public PacketHandler
{
public:
	TextureGrabber(PacketDispatcher* dispatcher);
	~TextureGrabber();

	// PacketHandler
	void HandlePacket(in::PlayerListItemPacket* packet);

private:
	bool ContainsTextureURL(const json& root);
};

} // ns example

#endif
