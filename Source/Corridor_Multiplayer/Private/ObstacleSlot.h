// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxSlot.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ObstacleSlot.generated.h"


UCLASS()
class AObstacleSlot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObstacleSlot();

	UPROPERTY(EditAnywhere)
	UBoxComponent* Obstacle;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Plane;

	UPROPERTY(BlueprintReadWrite)
	FSBoxPosition ObstaclePosition;

	EBoxState ObstacleState;

	UFUNCTION()
	void SetState(EBoxState NewState);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
