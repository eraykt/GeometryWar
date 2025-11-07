#include "EntityManager.h"
EntityManager::EntityManager() = default;

// called at beginning of each frame by game engine
// entities added will now be available to use this frame
void EntityManager::update()
{
	for (auto e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}

	// remove dead entities from the vector of all entities
	removeDeadEntities(m_entities);

	// remove dead entities from each vector in the entity map
	// C++20 way ot iterating through [key, value] pairs in a map
//    for (auto &[tag, entityVec]: m_entityMap) {
//        removeDeadEntities(entityVec);
//    }

	m_entitiesToAdd.clear();
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	m_entities.erase(
		std::remove_if(m_entities.begin(), m_entities.end(),
			[](const std::shared_ptr<Entity>& e)
			{
				return !e->m_active;
			}
		),
		m_entities.end()
	);

	for (auto const& e : m_entityMap)
	{
		EntityVec& map = m_entityMap[e.first];

		map.erase(
			std::remove_if(map.begin(), map.end(),
				[](const std::shared_ptr<Entity>& e)
				{
					return !e->m_active;
				}
			),
			map.end()
		);
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	// create a new Entity object
	// store it in the vector of all entities
	// store it in the map of tag->entityVector
	// return the shared pointer pointing to that entity
	// auto e = std::make_shared<Entity>(tag, m_totalEntities++);
	// m_entities.push_back(e);
	// m_entityMap[tag].push_back(e);
	// return e;

	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(entity);
	return entity;
}
// note: above code doesn't handle some map-related edge cases

EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag)
{
	// TODO: check edge cases
	return m_entityMap[tag];
}

const std::map<std::string, EntityVec>& EntityManager::getEntityMap()
{
	return m_entityMap;
}

// Iterator Invalidation Example
// void sCollision()
// {
//     EntityVec bullets;
//     EntityVec tiles;
//     for (auto & b: bullets)
//         for (auto & t: tiles)
//             if(Physics::IsCollision(b,t)) bullets.erase(b);
// }
// Solution: Delayed Effects
// Idea: only add or remove entities at the beginning of a frame when it is safe
