#include "Engine.hpp"
#include "Log.hpp"
#include "Actors/Actor.hpp"
#include "Actors/DirLight.hpp"
#include "Actors/PointLight.hpp"
#include "Actors/SkyLight.hpp"
#include "Camera/CameraComponent.hpp"
#include "Components/DirLightComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/SpotLightComponent.hpp"
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
		camera_.SetNearFar(10, 10000);
		camera_.SetVFov(60_deg);

		movement_.SetMaxSpeed(300);

		auto& input = AddComponent<InputComponent>();
		input.BindAxis("MoveForward", [this](Float f) { MoveForward(f); });
		input.BindAxis("MoveRight", [this](Float f) { MoveRight(f); });
		input.BindAxis("MoveUp", [this](Float f) { MoveUp(f); });
		input.BindAxis("Turn", [this](Float f) { Turn(f); });
		input.BindAxis("LookUp", [this](Float f) { LookUp(f); });

		auto& light = AddComponent<SpotLightComponent>();
		light.AttachTo(GetRootComponent(), AttachRule::kKeepRelative);
		light.SetAngle({0_deg, 22.5_deg});
		light.SetRadius(500);
		light.SetColor({All{}, 1.5_f});
	}

private:
	void MoveForward(Float f) noexcept
	{
		if (!IsNearlyZero(f))
		{
			auto input = *GetForward();
			input[2] = 0; input.TryNormalize();
			movement_.AddMovInput(input * f);
		}
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

class PlaneActor : public AActor
{
public:
	explicit PlaneActor(World& world)
		:AActor{world}, mesh_{AddComponent<MeshComponent>()}
	{
		SetRootComponent(&mesh_);
		mesh_.SetRelScale({All{}, 10});
		mesh_.SetMesh("../Assets/Plane.omesh");
	}

private:
	MeshComponent& mesh_;
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
	
	constexpr auto start = -1250.0_f;
	constexpr auto size = 250.0_f;
	for (auto i = 0; i < 10; i++)
	{
		for (auto j = 0; j < 10; j++)
		{
			auto& a = world.SpawnActor<PlaneActor>();
			a.SetPos(Vec3(start + i * size, start + j * size, -100.0_f));
		}
	}

	Quat q(UVec3::forward, 90_deg);
	for (auto i = 0; i < 10; i++)
	{
		auto& a = world.SpawnActor<PlaneActor>();
		a.SetPos(Vec3(start + i * size, start - size, 0.0_f));
		a.SetRot(q);
		
		auto& b = world.SpawnActor<PlaneActor>();
		b.SetPos(Vec3(start + i * size, -start + size, 0.0_f));
		b.SetRot(q);
	}

	q = Quat(UVec3::up, 90_deg) * q;
	for (auto i = 0; i < 10; i++)
	{
		auto& a = world.SpawnActor<PlaneActor>();
		a.SetPos(Vec3(start - size, start + i * size, 0.0_f));
		a.SetRot(q);

		auto& b = world.SpawnActor<PlaneActor>();
		b.SetPos(Vec3(-start + size, start + i * size, 0.0_f));
		b.SetRot(q);
	}
	
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
	world.SpawnActor<ASkyLight>();
	
	// auto& sun = world.SpawnActor<ADirLight>();
	// sun.SetRot({UVec3::right, 1_rad});

	// auto& light = world.SpawnActor<APointLight>();
	// light.SetPos({300, 0, 0});
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
