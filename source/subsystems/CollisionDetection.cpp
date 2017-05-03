#include "CollisionDetection.hpp"

namespace pg
{
	void CollisionDetection::Update( float dt )
	{
		std::vector<std::pair<std::shared_ptr<VelocityComponent>, std::shared_ptr<ColliderComponent>>> colliders;
		colliders = this->getColliders();

		for ( auto& objectA : colliders)
		{
			for ( auto& objectB : colliders )
			{
				VelocityComponent& velocity = *objectA.first;
				sf::FloatRect objectUpdate = *objectA.second;
				objectUpdate.left += velocity.x * dt;
				objectUpdate.top += velocity.y * dt;

				collidedDirection_t direction = COLLISION_NONE;
				if ( *objectA.second != *objectB.second )
				{
					if ( !objectA.second->intersects(*objectB.second) )
					{
						if ( objectA.second->top >= objectB.second->top + objectB.second->height && objectUpdate.top <= objectB.second->top + objectB.second->height )
							direction = COLLISION_TOP;
						else if ( objectA.second->top + objectA.second->height <= objectB.second->top && objectUpdate.top + objectUpdate.height >= objectB.second->top )
							direction = COLISION_BOTTOM;
						else if ( objectA.second->left + objectA.second->width <= objectB.second->left && objectUpdate.left + objectUpdate.width >= objectB.second->left )
							direction = COLLISION_LEFT;
						else if ( objectA.second->left >= objectB.second->left + objectB.second->width && objectUpdate.left <= objectB.second->left + objectB.second->width )
							direction = COLLISION_RIGHT;
					}
				}
				objectA.second->callback( *objectB.second, direction, *systemBase );
			}
		}
	}

	std::vector<std::pair<std::shared_ptr<VelocityComponent>, std::shared_ptr<ColliderComponent>>> CollisionDetection::getColliders()
	{
		std::vector<std::pair<std::shared_ptr<VelocityComponent>, std::shared_ptr<ColliderComponent>>> colliders;
		for ( ecs::internal::componentBlock_t& block : this->collisionBlocks )
		{
			for ( auto& wrapper : block.data )
			{
				if ( wrapper.ownerEntityID != ecs::UNASSIGNED_ENTITY_ID )
				{
					std::pair<std::shared_ptr<VelocityComponent>, std::shared_ptr<ColliderComponent>> coll;
					coll.first = std::static_pointer_cast<VelocityComponent>( this->systemBase->GetComponent<VelocityComponent>( wrapper.ownerEntityID ).data );
					if ( !coll.first ) coll.first = std::make_shared<VelocityComponent>( VelocityComponent( { 0.0f, 0.0f } ) );
					coll.second = std::static_pointer_cast<ColliderComponent>( wrapper.data );

					colliders.push_back( coll );
				}
			}
		}
		return colliders;
	}
}