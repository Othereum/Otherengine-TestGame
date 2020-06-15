#include "Engine.hpp"
#include "Log.hpp"
#include "GameFramework/Actor.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/DirLightComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/InputComponent.hpp"
#include "GameFramework/MovementComponent.hpp"
#include "SDL2/SDL_keycode.h"

using namespace oeng;

class SimplePawn : public AActor
{
public:
	explicit SimplePawn(World& world)
		:AActor{world},
		movement_{AddComponent<MovementComponent>()},
		camera_{AddComponent<CameraComponent>()}
	{
		SetRootComponent(&camera_);
		camera_.Activate();

		auto& input = AddComponent<InputComponent>();
		input.BindAxis("MoveForward", [this](Float f) { MoveForward(f); });
		input.BindAxis("MoveRight", [this](Float f) { MoveRight(f); });
		input.BindAxis("MoveUp", [this](Float f) { MoveUp(f); });
		input.BindAxis("Turn", [this](Float f) { Turn(f); });
		input.BindAxis("LookUp", [this](Float f) { LookUp(f); });

		movement_.SetMaxSpeed(200);
	}

private:
	void MoveForward(Float f) noexcept
	{
		if (!IsNearlyZero(f))
			movement_.AddMovInput(*GetForward() * f);
	}

	void MoveRight(Float f) noexcept
	{
		if (!IsNearlyZero(f))
			movement_.AddMovInput(*GetRight() * f);
	}

	void MoveUp(Float f) noexcept
	{
		if (!IsNearlyZero(f))
			movement_.AddMovInput(Vec3::up * f);
	}

	void Turn(Float f) noexcept
	{
		if (!IsNearlyZero(f))
			movement_.AddRotInput({UVec3::up, 1.5_rad * f * GetWorld().GetDeltaSeconds()});
	}

	void LookUp(Float f) noexcept
	{
		if (!IsNearlyZero(f))
			movement_.AddRotInput({GetRight(), 1.5_rad * f * GetWorld().GetDeltaSeconds()});
	}

	MovementComponent& movement_;
	CameraComponent& camera_;
};

class RotatingLight : public AActor
{
public:
	explicit RotatingLight(World& world)
		:AActor{world}, light_{AddComponent<DirLightComponent>()}
	{
		SetRootComponent(&light_);
		light_.Activate();
	}

private:
	void OnUpdate(Float delta_seconds) override
	{
		time_ += delta_seconds;
		const auto rot = 1_rad * time_;
		SetRot(Quat{UVec3::forward, rot} * init_rot_);

		light_.SetColor(Vec3::one * Abs(Cos(rot/2)));
	}

	DirLightComponent& light_;
	const Quat init_rot_{UVec3::right, 1_rad};
	Float time_ = 0;
};

static void LoadGame(Engine& e)
{
	auto& world = e.GetWorld();
	
	auto& cube = world.SpawnActor<AActor>();
	auto& cube_mesh = cube.AddComponent<MeshComponent>();
	cube_mesh.SetMesh("../Assets/Cube.omesh");
	cube.SetRootComponent(&cube_mesh);
	cube.SetTrsf({
		{200, 75, 0},
		Quat{UVec3::up, 225_deg} * Quat{UVec3::right, -90_deg},
		{All{}, 100}
	});

	auto& sphere = world.SpawnActor<AActor>();
	auto& sphere_mesh = sphere.AddComponent<MeshComponent>();
	sphere_mesh.SetMesh("../Assets/Sphere.omesh");
	sphere.SetRootComponent(&sphere_mesh);
	sphere.SetTrsf({ {200, -75, 0}, {}, {All{}, 3} });

	auto& is = e.GetInputSystem();
	is.AddAxis("MoveForward", {
		{'w', InputType::kKeyboard, 1},
		{'s', InputType::kKeyboard, -1},
	});
	is.AddAxis("MoveRight", {
		{'a', InputType::kKeyboard, -1},
		{'d', InputType::kKeyboard, 1},
	});
	is.AddAxis("Turn", {
		{SDLK_RIGHT, InputType::kKeyboard, 1},
		{SDLK_LEFT, InputType::kKeyboard, -1}
	});
	is.AddAxis("LookUp", {
		{SDLK_UP, InputType::kKeyboard, -1},
		{SDLK_DOWN, InputType::kKeyboard, 1},
	});
	is.AddAxis("MoveUp", {
		{SDLK_LSHIFT, InputType::kKeyboard, -1},
		{' ', InputType::kKeyboard, 1},
	});
	
	world.SpawnActor<SimplePawn>();
	world.SpawnActor<RotatingLight>();
}

int main()
{
	try
	{
		Engine engine{"Test Game", &LoadGame};
		engine.RunLoop();
	}
	catch (const std::exception& e)
	{
		log::Critical(e.what());
	}
}
