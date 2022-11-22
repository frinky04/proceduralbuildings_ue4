// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"


UENUM(BlueprintType)
enum EWallType
{
	Wall,
	Door,
	Window
};

USTRUCT(BlueprintType)
struct FRoomTheme
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString themeName = "Default";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance = 1.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Offset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Size;
	

	

	
	
};

USTRUCT(BlueprintType)
struct FWall
{
	
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Rotation;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntPoint Position;

	FWall(int InRotation, FIntPoint InPosition)
	{
		Rotation = InRotation;
		Position = InPosition;
		
	}

	FWall()
	{
		Rotation = 0;
		Position = FIntPoint();
	}
	
};

USTRUCT(BlueprintType)
struct FRoom
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRoomTheme Theme;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FIntPoint> Floors;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FWall> Walls;
	
};

UCLASS()
class PROCEDURALBUILDINGS_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;


private:
	UPROPERTY(EditAnywhere)
	class USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	class UInstancedStaticMeshComponent* FloorInstances;

	UPROPERTY(EditAnywhere)
	class UInstancedStaticMeshComponent* WallInstances;

	UPROPERTY(EditAnywhere)
	class UInstancedStaticMeshComponent* DoorwayInstances;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float Size = 200;

	UPROPERTY()
	TArray<FWall> AllWalls;

	UPROPERTY()
	TArray<FIntPoint> AllFloors;

	// FUNCTIONS

	UFUNCTION(BlueprintCallable)
	FRoom GenerateRoomStruct(FRoomTheme Theme);
	

	UFUNCTION(BlueprintCallable)
	void DrawRooms(TArray<FRoom> Rooms);

	UFUNCTION(BlueprintCallable)
	void DrawRoom(FRoom Room);

	UFUNCTION(BlueprintCallable)
	FIntPoint GetDirection(int Rotation);

	UFUNCTION(BlueprintCallable)
	FIntPoint GetInverseDirection(int Rotation);

	UFUNCTION(BlueprintCallable)
	FVector GetLocation(FIntPoint IntPoint);

	UFUNCTION(BlueprintCallable)
	FRotator GetRotation(int intRotation);

	UFUNCTION(BlueprintCallable)
	int GetInverseRotation(int intRotation);

	UFUNCTION(BlueprintCallable)
	bool DoesWallExist(FIntPoint Position, int Rotation);
	
	UFUNCTION(BlueprintCallable)
	void ClearAll();

	UFUNCTION(BlueprintCallable, CallInEditor)
	TArray<FRoom> GenerateRandomRoomsV1(int NumRooms, int MaxExtent);

	UFUNCTION(BlueprintCallable)
	bool DoFloorsIntersect(TArray<FIntPoint> A, TArray<FIntPoint> B);

	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> GenerateFloors(FIntPoint RoomSize, FIntPoint Offset);

	UFUNCTION(BlueprintCallable)
	TArray<FWall> GenerateWalls(TArray<FIntPoint> Floors);

	UFUNCTION(BlueprintCallable)
	FRoomTheme FitRoomTheme(bool& success, FRoomTheme Theme, TArray<FIntPoint> Fit);

	UFUNCTION(BlueprintCallable)
	FIntPoint FlipIntPoint(FIntPoint Point);

	UFUNCTION(BlueprintCallable)
	bool isEdge(FIntPoint Point, TArray<FIntPoint> All, FIntPoint& Direction);
	
};
