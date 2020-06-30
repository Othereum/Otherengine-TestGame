#include "Engine.hpp"
#include "Actors/Actor.hpp"
#include "Actors/DirLight.hpp"
#include "Actors/PointLight.hpp"
#include "Actors/SkyLight.hpp"
#include "Camera/CameraComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/SpotLightComponent.hpp"

using namespace oeng;

OE_DEFINE_GAME_MODULE(TestGame);

class SimplePawn : public AActor
{
public:
	explicit SimplePawn(World& world)
		:AActor{world},
		movement_{AddComponent<MovementComponent>()},
		camera_{AddComponent<CameraComponent>()},
		light_{AddComponent<SpotLightComponent>()}
	{
		SetRootComponent(&camera_);
		camera_.Activate();
		camera_.SetNearFar(10, 10000);
		camera_.SetVFov(60_deg);

		movement_.SetMaxSpeed(300);

		auto& input = AddComponent<InputComponent>();
		input.BindAxis("MoveForward", [this](Float f) { MoveForward(f); });
		input.BindAxis("MoveRight", [this](Float f) { MoveRight(f); });
		//input.BindAxis("MoveUp", [this](Float f) { MoveUp(f); });
		input.BindAxis("Turn", [this](Float f) { Turn(f); });
		input.BindAxis("LookUp", [this](Float f) { LookUp(f); });
		input.BindAction("Flash", true, [this]()
		{
			light_.IsActive() ? light_.Deactivate() : light_.Activate();
		});

		light_.AttachTo(&camera_, AttachRule::kKeepRelative);
		light_.SetAngle({0_deg, 22.5_deg});
		light_.SetRadius(500);
		light_.SetColor({All{}, 1.5_f});
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
			movement_.AddRotInput({UVec3::up, 30_deg * f * GetWorld().GetDeltaSeconds()});
	}

	void LookUp(Float f) noexcept
	{
		if (!IsNearlyZero(f))
			movement_.AddRotInput({GetRight(), 30_deg * f * GetWorld().GetDeltaSeconds()});
	}

	MovementComponent& movement_;
	CameraComponent& camera_;
	SpotLightComponent& light_;
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

class RotatingLight : public ADirLight
{
public:
	explicit RotatingLight(World& world)
		:ADirLight{world}
	{
	}

private:
	void OnUpdate(Float delta_seconds) override
	{
		time_ += delta_seconds;
		const auto rot = 5_deg * time_;
		SetRot(Quat{UVec3::forward, rot} * init_rot_);

		const auto alpha = Abs(Cos(rot/2));
		SetColor(Vec3::one * (alpha*alpha));
	}

	const Quat init_rot_{UVec3::right, 1_rad};
	Float time_ = 0;
};

class BouncingLight : public APointLight
{
public:
	BouncingLight(World& world)
		:APointLight{world}
	{
		SetRadius(500);
	}

private:
	void OnUpdate(Float delta_seconds) override
	{
		time_ += delta_seconds;
		SetPos(init_pos_ + Vec3{150 * std::sin(time_ * 2), 0, 0});
	}

	const Vec3 init_pos_{200, -20, 0};
	Float time_ = 0;
};

OE_GAME_API void LoadGame(Engine& e)
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
		{Keycode::W, 1},
		{Keycode::S, -1},
	});
	is.AddAxis("MoveRight", {
		{Keycode::A, -1},
		{Keycode::D, 1},
	});
	is.AddAxis("Turn", {
		{MouseAxis::X, 1}
	});
	is.AddAxis("LookUp", {
		{MouseAxis::Y, 1}
	});
	is.AddAxis("MoveUp", {
		{Keycode::L_SHIFT, -1},
		{Keycode::SPACE, 1},
	});
	is.AddAction("Flash", {Keycode::F});
	
	world.SpawnActor<SimplePawn>();
	world.SpawnActor<ASkyLight>();
	world.SpawnActor<BouncingLight>();
	
	auto& sun = world.SpawnActor<RotatingLight>();
	sun.SetRot({UVec3::right, 1_rad});
}

