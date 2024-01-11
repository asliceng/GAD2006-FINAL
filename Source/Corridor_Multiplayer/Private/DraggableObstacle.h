// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameManager.h"
#include "ObstacleSlot.h"
#include "Kismet/GameplayStatics.h"
#include "DraggableObstacle.generated.h"


UCLASS()
class ADraggableObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADraggableObstacle();

	AGameManager* GameManager;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere)
	UBoxComponent* Obstacle;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ObstacleMesh;

	bool bIsDragging;
	bool bCanRotate;

	void StartDragging();
	void StopDragging();
	void RotateObstacle(float DeltaScroll);

	FVector StartDragOffset;

	bool IsMouseOver();

	UPROPERTY()
	ANetPlayerController* PlayerController;
};
