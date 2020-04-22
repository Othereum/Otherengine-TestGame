#include "Actors/Asteroid.h"
#include "Components/MoveComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CircleComponent.h"
#include "MathUtil.h"

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
}
