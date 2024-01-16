// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameManager.h"
#include "ObstacleSlot.h"
#include "GameGrid.h"
#include "Kismet/GameplayStatics.h"
#include "DraggableObstacle.generated.h"

//class AUnitBaseBase;

UCLASS()
class ADraggableObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADraggableObstacle();

	AGameManager* GameManager;
	AGameGrid* GameGrid;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Tick(float DeltaTime);

public:	
	UPROPERTY(EditAnywhere)
	UBoxComponent* Obstacle;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ObstacleMesh;

	bool bIsDragging;
	bool bCanScroll;

	void StartDragging();
	void StopDragging();
	void RotateObstacle(float DeltaScroll);

	FVector StartDragOffset;
	FVector DefaultLocation;

	bool IsMouseOver();

	UPROPERTY(EditAnywhere)
	ANetPlayerController* PlayerController;

	TSet<AObstacleSlot*> OverlappingSlots;
	FVector SnapToSlots();

	AObstacleSlot* Slot1 = nullptr;
	AObstacleSlot* Slot2 = nullptr;

	void RemoveFromObstacleList();

	void RemoveObstacleSlot();

	bool CanPlaceObstacle();
	bool CheckSlots();
	bool CheckObstacles();

};
