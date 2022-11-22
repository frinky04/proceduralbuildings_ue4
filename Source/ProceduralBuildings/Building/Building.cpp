// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	FloorInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstanceFloor"));
	WallInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstanceWall"));
	DoorwayInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstanceDoorway"));

	
	FloorInstances->SetStaticMesh((ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/ProceduralBuildings/Meshes/Floor.Floor'"))).Object);
	WallInstances->SetStaticMesh((ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/ProceduralBuildings/Meshes/Wall.Wall'"))).Object);
	DoorwayInstances->SetStaticMesh((ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/ProceduralBuildings/Meshes/Doorway.Doorway'"))).Object);

}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
}

void ABuilding::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}

FRoom ABuilding::GenerateRoomStruct(FRoomTheme Theme)
{
	FRoom Room;
	Room.Theme = Theme;

	Room.Floors = GenerateFloors(Theme.Size, Theme.Offset);
	Room.Walls = GenerateWalls(Room.Floors);

	AllFloors.Append(Room.Floors);
	AllWalls.Append(Room.Walls);

	return Room;
}

void ABuilding::DrawRooms(TArray<FRoom> Rooms)
{
	for (auto Room : Rooms)
	{
		DrawRoom(Room);

	}
	
	
}

void ABuilding::DrawRoom(FRoom Room)
{
	for (auto floor : Room.Floors)
	{
		FloorInstances->AddInstanceWorldSpace(FTransform(FVector(GetLocation(floor)+GetActorLocation())));

	}

	for (auto wall : Room.Walls)
	{
		WallInstances->AddInstanceWorldSpace(FTransform(FRotator(GetRotation(wall.Rotation)),FVector(GetLocation(wall.Position)+GetActorLocation())));
	}
}

FIntPoint ABuilding::GetInverseDirection(int Rotation)
{
	switch (Rotation)
	{
	case 0:
		return FIntPoint(-1,0);
	case 1:
		return FIntPoint(0,-1);
	case 2:
		return FIntPoint(1,0);
	case 3:
		return FIntPoint(0,1);
	default:
		return FIntPoint(0,1);
	}
}

FIntPoint ABuilding::GetDirection(int Rotation)
{
	switch (Rotation)
	{
	case 0:
		return FIntPoint(1,0);
	case 1:
		return FIntPoint(0,1);
	case 2:
		return FIntPoint(-1,0);
	case 3:
		return FIntPoint(0,-1);
	default:
		return FIntPoint(0,-1);
	}
}

FVector ABuilding::GetLocation(FIntPoint IntPoint)
{
	return FVector( IntPoint.X * Size, IntPoint.Y * Size, 0);
}

FRotator ABuilding::GetRotation(int intRotation)
{
	return FRotator(0.0,  intRotation * 90, 0.0);
}

int ABuilding::GetInverseRotation(int intRotation)
{
	switch (intRotation)
	{
	case 0:
		return 2;
	case 1:
		return 3;
	case 2:
		return 0;
	case 3:
		return 1;
	default:
		return 1;
	}
}

bool ABuilding::DoesWallExist(FIntPoint Position, int Rotation)
{
	for (auto Wall : AllWalls)
	{
		if(Wall.Position == Position && Wall.Rotation == Rotation)
		{
			return true;
		}
		
		/*if(Wall.Position == Position - GetDirection(Rotation) && Wall.Rotation == GetInverseRotation(Rotation))
		{
			return true;
		}*/
	}
	return false;
}

void ABuilding::ClearAll()
{
	AllWalls.Empty();
	AllFloors.Empty();
	WallInstances->ClearInstances();
	FloorInstances->ClearInstances();
	DoorwayInstances->ClearInstances();
}

TArray<FRoom> ABuilding::GenerateRandomRoomsV1(int NumRooms, int MaxExtent)
{
	ClearAll();
	
	TArray<FRoom> Rooms;
	TArray<FIntPoint> Floors;
	FRoomTheme BaseRoom = FRoomTheme();

	BaseRoom.Size = FIntPoint(4, 4);
	
	
	Rooms.Add(GenerateRoomStruct(BaseRoom));
	DrawRoom(Rooms[0]);
	
	Floors = GenerateFloors(BaseRoom.Size, BaseRoom.Offset);
	
	for (int i = 0; i < NumRooms; ++i)
	{
		FRoomTheme NewRoom = FRoomTheme();
		NewRoom.Size = FIntPoint(UKismetMathLibrary::RandomIntegerInRange(2,6), UKismetMathLibrary::RandomIntegerInRange(2,6));

		int iterator = 0;
		bool success = false;

		while (!success && iterator < 2048)
		{
			iterator++;
			FIntPoint Point = Floors[FMath::RandRange(0, Floors.Num()-1)];
			FIntPoint Direction;
			bool CouldFit;
			if(isEdge(Point, Floors, Direction))
			{
				FRoomTheme UpdatedTheme = BaseRoom;
				UpdatedTheme.Offset = Point + Direction;
				UpdatedTheme.Size = FIntPoint(FMath::RandRange(3, 5), FMath::RandRange(3, 5));
				UpdatedTheme = FitRoomTheme(CouldFit, UpdatedTheme, Floors);
				FIntPoint Extent = UpdatedTheme.Offset + UpdatedTheme.Size - 1;
				if(CouldFit)
				{
					DrawDebugLine(GetWorld(), GetLocation(UpdatedTheme.Offset), GetLocation(Extent), FColor::Red, true);
					DrawDebugString(GetWorld(), GetLocation(UpdatedTheme.Offset), "X: " + FString::SanitizeFloat(Extent.X) + " | Y: " + FString::SanitizeFloat(Extent.Y), this, FColor::Red);
					FRoom GeneratedRoom = GenerateRoomStruct(UpdatedTheme);
					Floors.Append(GeneratedRoom.Floors);
					DrawRoom(GeneratedRoom);
					success = true;
				}
			}
		}
		
		
		// todo generation
		
		
	}
	return Rooms;
}

bool ABuilding::DoFloorsIntersect(TArray<FIntPoint> A, TArray<FIntPoint> B)
{
	for (auto Position : A)
	{
		if(B.Contains(Position))
		{
			return true;
		}
	}
	return false;
}

TArray<FIntPoint> ABuilding::GenerateFloors(FIntPoint RoomSize, FIntPoint Offset)
{
	TArray<FIntPoint> Floors;
	for (int x = 0; x < RoomSize.X; ++x)
	{
		for (int y = 0; y < RoomSize.Y; y++)
		{
			Floors.Add(FIntPoint(x+Offset.X, y+Offset.Y));
		}
	}

	return Floors;
}

TArray<FWall> ABuilding::GenerateWalls(TArray<FIntPoint> Floors)
{
	TArray<FWall> Walls;
	for (auto floor : Floors)
	{
		for (int i = 0; i < 4; ++i)
		{
			if(!Floors.Contains(floor + GetDirection(i)) && !DoesWallExist(floor + GetDirection(i), i))
			{
				Walls.Add(FWall(i, floor + GetDirection(i)));
			}

		}
	}

	return Walls;
	
}

FRoomTheme ABuilding::FitRoomTheme(bool& success, FRoomTheme Theme, TArray<FIntPoint> Fit)
{
	success = true;
	
	// Test Base Layout
	FRoomTheme UpdatedTheme = Theme;
	TArray<FIntPoint> TestTiles = GenerateFloors(UpdatedTheme.Size, UpdatedTheme.Offset);
	if(!DoFloorsIntersect(TestTiles, Fit)){ return UpdatedTheme; }

	// Test Flipping Size
	UpdatedTheme = Theme;
	UpdatedTheme.Size = FlipIntPoint(Theme.Size);
	TestTiles = GenerateFloors(UpdatedTheme.Size, UpdatedTheme.Offset);
	if(!DoFloorsIntersect(TestTiles, Fit)){ return UpdatedTheme; }

	// Test Flipping Offset
	UpdatedTheme = Theme;	
	UpdatedTheme.Offset -= Theme.Size - 1;
	TestTiles = GenerateFloors(UpdatedTheme.Size, UpdatedTheme.Offset);
	if(!DoFloorsIntersect(TestTiles, Fit)){ return UpdatedTheme; }

	// Test Flipping X Offset
	UpdatedTheme = Theme;
	UpdatedTheme.Offset.X -= Theme.Size.X - 1;
	TestTiles = GenerateFloors(UpdatedTheme.Size, UpdatedTheme.Offset);
	if(!DoFloorsIntersect(TestTiles, Fit)){ return UpdatedTheme; }

	// Test Flipping X Offset
	UpdatedTheme = Theme;
	UpdatedTheme.Offset.Y -= Theme.Size.Y - 1;
	TestTiles = GenerateFloors(UpdatedTheme.Size, UpdatedTheme.Offset);
	if(!DoFloorsIntersect(TestTiles, Fit)){ return UpdatedTheme; }

	success = false;
	return Theme;
	
	
}

FIntPoint ABuilding::FlipIntPoint(FIntPoint Point)
{
	return FIntPoint(Point.Y, Point.X);
}

bool ABuilding::isEdge(FIntPoint Point, TArray<FIntPoint> All, FIntPoint& Direction)
{
	for (int i = 0; i < 4; ++i)
	{
		if(!All.Contains(Point + GetDirection(i)))
		{
			Direction = GetDirection(i);
			return true;
		}

	}
	return false;
}


