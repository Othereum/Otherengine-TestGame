#include "Engine.hpp"
#include "Log.hpp"

#include "Actor.hpp"
#include "Components/MeshComponent.hpp"

using namespace oeng;

static void LoadGame(Engine& e)
{
	auto& world = e.GetWorld();
	
	auto& cube = world.SpawnActor<AActor>();
	auto& cube_mesh = cube.AddComponent<MeshComponent>();
	cube_mesh.SetMesh("../Assets/Cube.omesh");
	cube.SetRootComponent(&cube_mesh);
	cube.SetTransform({
		{200, 75, 0},
		Quat{UVec3::up, 225_deg} * Quat{UVec3::right, -90_deg},
		{All{}, 100}}
	);

	auto& sphere = world.SpawnActor<AActor>();
	auto& sphere_mesh = sphere.AddComponent<MeshComponent>();
	sphere_mesh.SetMesh("../Assets/Sphere.omesh");
	sphere.SetRootComponent(&sphere_mesh);
	sphere.SetTransform({ {200, -75, 0}, {}, {All{}, 3} });
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
