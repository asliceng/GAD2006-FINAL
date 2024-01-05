// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameManager.h"
#include "NetPlayerController.generated.h"

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

	UPROPERTY(BlueprintReadWrite)
	AGameManager* GameManager;
	
};
