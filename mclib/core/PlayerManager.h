#ifndef MCLIB_CORE_PLAYER_MANAGER_H_
#define MCLIB_CORE_PLAYER_MANAGER_H_

#include <common/UUID.h>
#include <entity/EntityManager.h>
#include <entity/Player.h>
#include <util/ObserverSubject.h>


class PlayerManager;


class Player
{
	friend class PlayerManager;
public:
	Player(CUUID uuid, std::wstring name)
		: m_UUID(uuid)
		, m_Name(name)
	{ }

	std::shared_ptr<PlayerEntity> GetEntity() const { return m_Entity.lock(); }
	void SetEntity(PlayerEntityPtr entity) { m_Entity = entity; }

	const std::wstring& GetName() const { return m_Name; }
	CUUID GetUUID() const { return m_UUID; }

private:
	CUUID m_UUID;
	std::wstring m_Name;
	PlayerEntityPtr m_Entity;
};
typedef std::shared_ptr<Player> PlayerPtr;



class PlayerListener
{
public:
	virtual ~PlayerListener() {}

	// Called when a PlayerPositionAndLook packet is received (when client spawns or is teleported by server). The player's position is already updated in the entity.
	virtual void OnClientSpawn(PlayerPtr player) {}
	// Called when a player joins the server.
	virtual void OnPlayerJoin(PlayerPtr player) {}
	// Called when a player leaves the server
	virtual void OnPlayerLeave(PlayerPtr player) {}
	// Called when a player comes within visible range of the client
	virtual void OnPlayerSpawn(PlayerPtr player) {}
	// Called when a player leaves visible range of the client The PlayerPtr entity is already set to null at this point. The entity still exists in the entity manager, which can be grabbed by the entity id
	virtual void OnPlayerDestroy(PlayerPtr player, EntityId eid) {}
	// Called when a player changes position. Isn't called when player only rotates.
	virtual void OnPlayerMove(PlayerPtr player, glm::dvec3 oldPos, glm::dvec3 newPos) {}
};



class PlayerManager
	: public PacketHandler
	, public EntityListener
	, public ObserverSubject<PlayerListener>
{
public:
	typedef std::map<CUUID, PlayerPtr> PlayerList;
	typedef PlayerList::iterator iterator;

public:
	MCLIB_API PlayerManager(PacketDispatcher* dispatcher, EntityManager* entityManager);
	MCLIB_API ~PlayerManager();

	PlayerManager(const PlayerManager& rhs) = delete;
	PlayerManager& operator=(const PlayerManager& rhs) = delete;
	PlayerManager(PlayerManager&& rhs) = delete;
	PlayerManager& operator=(PlayerManager&& rhs) = delete;

	iterator MCLIB_API begin();
	iterator MCLIB_API end();

	// Gets a player by their UUID. Fast method, just requires map lookup.
	PlayerPtr MCLIB_API GetPlayerByUUID(CUUID uuid) const;
	// Gets a player by their EntityId. Somewhat slow method, has to loop through player map to find the player with that eid. It should still be pretty fast though since there aren't many players on a server usually.
	PlayerPtr MCLIB_API GetPlayerByEntityId(EntityId eid) const;
	// Gets a player by their username.
	PlayerPtr MCLIB_API GetPlayerByName(const std::wstring& name) const;

	// EntityListener
	void MCLIB_API OnPlayerSpawn(PlayerEntityPtr entity, CUUID uuid);
	void MCLIB_API OnEntityDestroy(EntityPtr entity);
	void MCLIB_API OnEntityMove(EntityPtr entity, glm::dvec3 oldPos, glm::dvec3 newPos);

	// PacketHandler
	void MCLIB_API HandlePacket(login::in::LoginSuccessPacket* packet);
	void MCLIB_API HandlePacket(in::PlayerPositionAndLookPacket* packet);
	void MCLIB_API HandlePacket(in::PlayerListItemPacket* packet);

private:
	PlayerList m_Players;
	EntityManager* m_EntityManager;
	CUUID m_ClientUUID;
};

#endif
