#include "MathUtil.h"
#include "Vector.hpp"

#include "Engine.h"
#include "World.h"
#include "InputSystem.h"
#include "GameModule.hpp"

#include "Actors/Asteroid.h"
#include "Actors/Ship.h"
#include "Components/SpriteComponent.h"

namespace oeng
{
	const char* GetGameName() noexcept
	{
		return "Test Game";
	}
	
	void LoadGameModule(CEngine& engine)
	{
		auto& input = engine.GetInputSystem();
		input.AddAxis("MoveForward", {
			{'w', EInputType::keyboard, 1},
			{'s', EInputType::keyboard, -1},
		});
		
		input.AddAxis("Turn", {
			{'a', EInputType::keyboard, -1},
			{'d', EInputType::keyboard, 1},
		});

		input.AddAction("Shoot", {
			{' ', EInputType::keyboard}
		});

		auto& world = engine.GetWorld();
		for (auto i = 0; i < 20; ++i)
		{
			auto& ast = world.SpawnActor<AAsteroid>();
			ast.SetPos(math::RandVec(engine.GetScreenSize() / -2.f, engine.GetScreenSize() / 2.f));
			ast.SetRot(math::RandAng());
		}

		auto& s = world.SpawnActor<ship>();
		s.SetRot(-90_deg);

		auto& bg = world.SpawnActor<AActor>();
		auto& bg_comp = bg.AddComponent<CSpriteComponent>(10);
		bg_comp.SetTexture("../Assets/Background.png");
	}
}
