// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxSlot.h"
#include "ObstacleSlot.h"
//#include "GameManager.h"
#include "Components/ChildActorComponent.h"
#include "GameGrid.generated.h"

UCLASS()
class AGameGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameGrid();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void SpawnAndLocateSlots(bool bIsItObstacle);

	//UFUNCTION()
	//void SpawnSlots(UChildActorComponent* Grid, bool bIsItObstacle, FString SlotName);

	//UFUNCTION()
	//void LocateBoxSlot(UChildActorComponent* Grid, int Rows, int Cols, FName SlotName, FVector BoxExtend, FVector ObstacleExtend);
	//void LocateObstacleSlot(UChildActorComponent* Grid, int Rows, int Cols, FName SlotName);

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABoxSlot> BoxClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AObstacleSlot> ObstacleClass;

	UPROPERTY(EditAnywhere)
	int NumCols;

	UPROPERTY(EditAnywhere)
	int NumRows;

	UPROPERTY(VisibleAnywhere)
	TArray<UChildActorComponent*> GridActors;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ABoxSlot* GetBoxSlot(FSBoxPosition& Position);
	static ABoxSlot* FindBoxSlot(FSBoxPosition Position);

	AObstacleSlot* GetObstacleSlot(FSBoxPosition& Position);
	static AObstacleSlot* FindObstacleSlot(FSBoxPosition Position);

private:
	static AGameGrid* GameGrid;
};
