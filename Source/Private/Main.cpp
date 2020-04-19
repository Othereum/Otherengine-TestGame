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
			ast.SetPos(math::RandVec({0, 0}, Vec2{1024, 768}));
			ast.SetRot(math::RandAng());
		}

		auto& sh = world.SpawnActor<ship>();
		sh.SetPos(Vec2{512, 384});
	}
}
