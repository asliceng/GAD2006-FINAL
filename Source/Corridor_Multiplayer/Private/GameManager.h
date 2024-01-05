// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetBaseCharacter.h"
#include "NetPlayerController.h"
#include "GameManager.generated.h"

UCLASS()
class AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

	UFUNCTION(BlueprintCallable)
	void SetupPlayers();

	UPROPERTY()
	int32 CurrentPlayerIndex;

	UFUNCTION(BlueprintCallable)
	void SwitchPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TArray<TSubclassOf<ANetBaseCharacter>> PlayerClasses;
	TArray<ANetBaseCharacter*> Players;
	int32 ActivePlayerIndex;

	void ActivatePlayer(int32 PlayerIndex);
};
