#pragma once
#include "Actor.h"

namespace oeng
{
	class AAsteroid : public AActor
	{
	public:
		explicit AAsteroid(CWorld& world);

	private:
		void UpdateActor(float delta_seconds) override;
	};
}
