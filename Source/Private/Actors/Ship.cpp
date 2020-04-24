#include "Actors/Ship.h"
#include <array>
#include "Actors/Asteroid.h"
#include "Actors/Laser.h"
#include "Components/SpriteComponent.h"
#include "Components/InputComponent.h"
#include "Components/PawnMoveComponent.h"
#include "Components/CircleComponent.h"
#include "World.h"
#include "TimerManager.h"
#include "Engine.h"

namespace oeng
{
	static constexpr std::array kShipPng{"../Assets/Ship.png", "../Assets/ShipWithThrust.png"};
	
	ship::ship(CWorld& world)
		:AActor{world}
	{
		auto& sprite = AddComponent<CSpriteComponent>();
		sprite.SetTexture(kShipPng[0]);

		auto& highlight = AddComponent<CSpriteComponent>(99);
		highlight.SetTexture("../Assets/ShipHighlight.png");

		auto& movement = AddComponent<CPawnMoveComp>();

		auto& col = AddComponent<CCircleComponent>();
		col.BindOnOverlap([&](CCircleComponent& c)
		{
			if (dynamic_cast<AAsteroid*>(&c.GetOwner()))
			{
				SetEnabled(false);
				
				GetTimerManager().SetTimer(2s, [&]()
				{
					SetEnabled(true);
					SetPos({});
					SetRot(-90_deg);
				});
			}
		});

		auto& input = AddComponent<CInputComponent>();
		
		input.BindAxis("MoveForward", [&](float f)
		{
			const auto should_move = !math::IsNearlyZero(f);
			if (should_move) movement.AddInput(GetForward() * f);

			if (should_move != is_moving_)
			{
				sprite.SetTexture(kShipPng[should_move]);
				is_moving_ = should_move;
			}
		});

		input.BindAxis("Turn", [&](float f)
		{
			movement.AddRotationInput(f);
		});

		input.BindAction("Shoot", true, [this]()
		{
			const auto cur = GetWorld().GetTime();
			if (next_attack_ <= cur)
			{
				auto& l = GetWorld().SpawnActor<ALaser>();
				l.SetPos(GetPos());
				l.SetRot(GetRot());

				using namespace std::chrono_literals;
				next_attack_ = cur + 500ms;
			}
		});
	}
}
