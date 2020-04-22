#pragma once
#include <chrono>
#include "Actor.h"

namespace oeng
{
	class ship : public AActor
	{
	public:
		explicit ship(CWorld& world);

	private:
		std::chrono::steady_clock::time_point next_attack_;
		bool is_moving_ = false;
	};
}
