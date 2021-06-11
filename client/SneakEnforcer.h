#ifndef CLIENT_SNEAKENFORCER_H_
#define CLIENT_SNEAKENFORCER_H_

#include <mclib/core/Client.h>
#include <mclib/core/PlayerManager.h>

namespace example
{

class SneakEnforcer 
	: public PlayerListener
	, public ClientListener
{
public:
	SneakEnforcer(CMinecraftClient* client);
	virtual ~SneakEnforcer();

	// ClientListener
	void OnTick() override;

	// PlayerListener
	void OnClientSpawn(PlayerPtr player) override;

private:
	CMinecraftClient* m_Client;
	PlayerManager* m_PlayerManager;
	Connection* m_Connection;
	int64 m_StartTime;
};

} // ns example

#endif
