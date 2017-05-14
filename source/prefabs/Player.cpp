#include "Player.hpp"

namespace pg
{
	void Player::SetUpComponents()
	{
		auto playerController = this->AddComponent<PlayerControllerComponent>();
		auto position = this->AddComponent<PositionComponent>();
		auto velocity = this->AddComponent<VelocityComponent>();
		auto collider = this->AddComponent<ColliderComponent>();
		auto tag = this->AddComponent<TagComponent>();

		playerController->left = movementKey_t( sf::Keyboard::A, { -100.0f, 0.0f } );
		playerController->right = movementKey_t( sf::Keyboard::D, { 100.0f, 0.0f } );
		playerController->jump = movementKey_t( sf::Keyboard::Space, { 0.0f, -250.0f } );

		*position = sf::Vector2f( 0.0f, 0.0f );
		*velocity = sf::Vector2f( 0.0f, 0.0f );

		*tag = entityID_t::PLAYER;
	}

	void Player::Update()
	{
		auto playerController = this->GetComponent<PlayerControllerComponent>();
		auto position = this->GetComponent<PositionComponent>();
		auto velocity = this->GetComponent<VelocityComponent>();
		auto drawable = this->GetComponent<DrawableComponent>();

		if ( sf::Keyboard::isKeyPressed( playerController->left.key ) )
			playerController->pendingForce.x = playerController->left.force.x;
		if ( sf::Keyboard::isKeyPressed( playerController->right.key ) )
			playerController->pendingForce.x = playerController->right.force.x;
		if ( sf::Keyboard::isKeyPressed( playerController->jump.key ) && !playerController->lockJump )
		{
			playerController->pendingForce.y = playerController->jump.force.y;
			playerController->lockJump = true;
		}

		*velocity += playerController->pendingForce;
		
		playerController->pendingForce = sf::Vector2f();
		if ( drawable )
			drawable->sprites[0].setPosition( *position );
	}
}