// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxSlot.h"
#include "NetPlayerController.h"
#include "PlayerUnitBase.generated.h"

class DraggableObstacle;

USTRUCT(BlueprintType)
struct FSPlayerInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* PlayerColor;

	bool Ready;
};

USTRUCT(BlueprintType)
struct FSUnitInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerUnitBase> UnitClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADraggableObstacle> ObstacleClass;

	UPROPERTY(EditAnywhere)
	FSPlayerInfo PlayerInfo;

	UPROPERTY(EditAnywhere)
	FSBoxPosition StartPosition;

	UPROPERTY(EditAnywhere)
	FVector ObstacleSpawnLocation;
};

UCLASS()
class APlayerUnitBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerUnitBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(BlueprintReadOnly)
	FName PlayerName;

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

	float RemainingObstaclesNum;

	TArray<ADraggableObstacle*> ObstacleList;

	UPROPERTY()
	TArray<ABoxSlot*> WinningBoxes;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
