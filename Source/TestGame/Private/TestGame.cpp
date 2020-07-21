#include "Engine.hpp"
#include "Actors/DirLight.hpp"
#include "Actors/MeshActor.hpp"
#include "Actors/PointLight.hpp"
#include "Actors/SkyLight.hpp"
#include "Camera/CameraComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/SpotLightComponent.hpp"

using namespace oeng;

DEFINE_GAME_MODULE(Test Game);

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
		input.BindAxis(u8"MoveForward"sv, [this](Float f) { MoveForward(f); });
		input.BindAxis(u8"MoveRight"sv, [this](Float f) { MoveRight(f); });
		input.BindAxis(u8"MoveUp"sv, [this](Float f) { MoveUp(f); });
		input.BindAxis(u8"Turn"sv, [this](Float f) { Turn(f); });
		input.BindAxis(u8"LookUp"sv, [this](Float f) { LookUp(f); });
		input.BindAction(u8"Flash"sv, true, [this]()
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
		{
			const auto delta = GetWorld().GetDeltaSeconds();
			movement_.AddRotInput({UVec3::up, 360_deg * f * delta});
		}
	}

	void LookUp(Float f) noexcept
	{
		if (!IsNearlyZero(f))
		{
			const auto delta = GetWorld().GetDeltaSeconds();
			movement_.AddRotInput({GetRight(), 360_deg * f * delta});
		}
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
		mesh_.SetMesh(u8"../Assets/Plane.omesh"sv);
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

static void CreateFloor(World& world, const int size)
{
	const Path mesh = u8"../Assets/Plane.omesh"sv;

	constexpr auto plane = 100_f;
	constexpr auto scale = 10_f;
	constexpr auto real = plane * scale;
	
	const auto base = -real/2 * ToFloat(size-1);
	
	for (auto i=0; i<size; ++i)
	{
		for (auto j=0; j<size; ++j)
		{
			auto& floor = world.SpawnActor<AMeshActor>();
			floor.SetMesh(mesh);
			floor.SetTrsf({
				{base+i*real, base+j*real, -100},
				{},
				{scale, scale, 1}
			});
		}
	}
}

GAME_API void GameMain(Engine& e)
{
	auto& world = e.GetWorld();

	CreateFloor(world, 3);
	
	auto& cube = world.SpawnActor<AActor>();
	auto& cube_mesh = cube.AddComponent<MeshComponent>();
	cube_mesh.SetMesh(u8"../Assets/Cube.omesh"sv);
	cube.SetRootComponent(&cube_mesh);
	cube.SetTrsf({
		{200, 75, 0},
		Quat{UVec3::up, 225_deg} * Quat{UVec3::right, -90_deg},
		{All{}, 100}
	});

	auto& sphere = world.SpawnActor<AActor>();
	auto& sphere_mesh = sphere.AddComponent<MeshComponent>();
	sphere_mesh.SetMesh(u8"../Assets/Sphere.omesh"sv);
	sphere.SetRootComponent(&sphere_mesh);
	sphere.SetTrsf({ {200, -75, 0}, {}, {All{}, 3} });
	
	world.SpawnActor<SimplePawn>();
	world.SpawnActor<ASkyLight>();
	world.SpawnActor<BouncingLight>();
	
	auto& sun = world.SpawnActor<RotatingLight>();
	sun.SetRot({UVec3::right, 1_rad});
}
