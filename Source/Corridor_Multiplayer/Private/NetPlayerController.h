// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPlayerController.generated.h"

class AGameManager;

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

	void OnActorClicked(AActor* Actor, FKey key);

	AGameManager* GameManager;

	UFUNCTION(BlueprintCallable, Category = "Player Activation")
	void SetPlayerActive(bool bIsActive);
	
};
