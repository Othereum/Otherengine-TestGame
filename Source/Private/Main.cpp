#include "MathUtil.h"
#include "Vector.hpp"

#include "Engine.h"
#include "World.h"
#include "InputSystem.h"

#include "Actors/Asteroid.h"
#include "Actors/Ship.h"

namespace oeng
{
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

		world.SpawnActor<ship>();
	}
}
