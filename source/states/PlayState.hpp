#pragma once

#include "State.hpp"
#include "States.hpp"
#include "ecs/ECS.hpp"
#include "resourceManaging/ResourceHolder.hpp"
#include "subsystems/Renderer.hpp"
#include "components/Drawable.hpp"
#include "PlayMainSubState.hpp"

namespace pg
{
	class PlayState final :
		public pi::State
	{
	public:
		explicit PlayState( sf::RenderWindow& win, pi::ResourceHolder& resCache, ecs::SystemBase& ecsSys ) :
			State( win ),
			resourceCache( resCache ),
			ecsSystem( ecsSys ),
			renderer( win )
		{}

		void OnStart() override;
		void OnStop() override;
		uint8_t Run() override;

		void UpdateThread( float time ) override;

	private:
		pi::ResourceHolder& resourceCache;
		ecs::SystemBase& ecsSystem;
		Renderer renderer;
	
		void loadTextures();
		void freeResources();
	};
}