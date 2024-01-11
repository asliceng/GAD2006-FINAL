// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "NetPlayerController.generated.h"

class AGameManager;
class ADraggableObstacle;
/**
 * 
 */
UCLASS()
class ANetPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANetPlayerController();

	void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

	void StartDragging();
	void StopDragging();

	void OnActorClicked(AActor* Actor, FKey key);

	void OnMouseScroll(float DeltaScroll);

	AGameManager* GameManager;

	float TraceDistance;

	UFUNCTION(BlueprintCallable, Category = "Player Activation")
	void SetPlayerActive(bool bIsActive);

	UPROPERTY(EditAnywhere)
	ADraggableObstacle* DraggableObstacle;
};
