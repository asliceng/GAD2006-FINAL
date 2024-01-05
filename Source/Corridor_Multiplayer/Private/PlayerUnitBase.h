// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerUnitBase.generated.h"

class ABoxSlot;

UCLASS()
class APlayerUnitBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerUnitBase();

	void AssignToSlot(ABoxSlot* NewSlot);

	//UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	//bool IsControlledByThePlayer();

	UPROPERTY(EditAnywhere)
	FVector StartOffset;

	UPROPERTY(VisibleAnywhere)
	ABoxSlot* Slot;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
