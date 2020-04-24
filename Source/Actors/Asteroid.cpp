#include "Asteroid.h"
#include "Components/MoveComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CircleComponent.h"
#include "MathUtil.h"
#include "Engine.h"

namespace oeng
{
	AAsteroid::AAsteroid(CWorld& world)
		:AActor{world}
	{
		auto& sprite = AddComponent<CSpriteComponent>();
		sprite.SetTexture("../Assets/Asteroid.png");

		auto& movement = AddComponent<CMoveComponent>();
		movement.SetVelocity(math::RandUnitVec() * 150);

		AddComponent<CCircleComponent>();
	}

	void AAsteroid::UpdateActor(float delta_seconds)
	{
		const auto& scr = GetEngine().GetScreenSize();
		auto pos = GetPos();
		
		if (pos.x < -scr.x/2 - 32) pos.x += scr.x + 64;
		else if (pos.x > scr.x/2 + 32) pos.x -= scr.x + 64;

		if (pos.y < -scr.y/2 - 32) pos.y += scr.y + 64;
		else if (pos.y > scr.y/2 + 32) pos.y -= scr.y + 64;

		SetPos(pos);
	}
}
