#include "stdafx.h"

#include <block/specific/MonsterSpawner.h>

bool MonsterSpawner::ImportNBT(NBT* nbt)
{
	auto spawnCountTag = nbt->GetTag<TagShort>(L"SpawnCount");
	if (spawnCountTag)
		m_SpawnCount = spawnCountTag->GetValue();

	auto spawnRangeTag = nbt->GetTag<TagShort>(L"SpawnRange");
	if (spawnRangeTag)
		m_SpawnRange = spawnRangeTag->GetValue();

	auto delayTag = nbt->GetTag<TagShort>(L"Delay");
	if (delayTag)
		m_Delay = delayTag->GetValue();

	auto minSpawnDelayTag = nbt->GetTag<TagShort>(L"MinSpawnDelay");
	if (minSpawnDelayTag)
		m_MinSpawnDelay = minSpawnDelayTag->GetValue();

	auto maxSpawnDelayTag = nbt->GetTag<TagShort>(L"MaxSpawnDelay");
	if (maxSpawnDelayTag)
		m_MaxSpawnDelay = maxSpawnDelayTag->GetValue();

	auto maxNearbyEntitiesTag = nbt->GetTag<TagShort>(L"MaxNearbyEntities");
	if (maxNearbyEntitiesTag)
		m_MaxNearbyEntities = maxNearbyEntitiesTag->GetValue();

	auto requiredPlayerRangeTag = nbt->GetTag<TagShort>(L"RequiredPlayerRange");
	if (requiredPlayerRangeTag)
		m_RequiredPlayerRange = requiredPlayerRangeTag->GetValue();

	auto spawnDataTag = nbt->GetTag<TagCompound>(L"SpawnData");
	if (spawnDataTag)
	{
		auto idTag = spawnDataTag->GetTag<TagString>(L"id");
		if (idTag)
			m_EntityId = idTag->GetValue();
	}

	return true;
}
