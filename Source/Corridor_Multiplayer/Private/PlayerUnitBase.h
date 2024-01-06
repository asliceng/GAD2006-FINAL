// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxSlot.h"
#include "NetPlayerController.h"
#include "PlayerUnitBase.generated.h"

USTRUCT(BlueprintType)
struct FSPlayerInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor PlayerColor;

	//PlayerLocation
	//RemainingObstaclesNum

	bool Ready;
};

USTRUCT(BlueprintType)
struct FSUnitInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerUnitBase> UnitClass;

	UPROPERTY(EditAnywhere)
	FSBoxPosition StartPosition;
};

UCLASS()
class APlayerUnitBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerUnitBase();

	void AssignToSlot(ABoxSlot* NewSlot);

	UPROPERTY(EditAnywhere)
	FVector StartOffset;

	UPROPERTY(VisibleAnywhere)
	ABoxSlot* Slot;

	UPROPERTY()
	USkeletalMeshComponent* PlayerPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsActivePlayer;

	UPROPERTY()
	ANetPlayerController* NetPlayerController;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
