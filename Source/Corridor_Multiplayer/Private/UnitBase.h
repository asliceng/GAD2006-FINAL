// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitBase.generated.h"

UENUM(BlueprintType)
enum class ESlotType : uint8
{
    /*ST_BoxSlot UMETA(DisplayName = "MovePlayer"),
    ST_ObstacleSlot UMETA(DisplayName = "PlaceObstacle")*/

	ST_BoxSlot = 0 UMETA(DisplayName = "MovePlayer"),
	ST_ObstacleSlot = 1 UMETA(DisplayName = "PlaceObstacle"),
	ST_COUNT = 2 UMETA(DisplayName = "NotSelected")
};

UCLASS()
class AUnitBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnitBase();

	void AssignToSlot(int SlotTypeIndex);

	UFUNCTION(BlueprintCallable)
	void ChangeSlotType(ESlotType SlotType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESlotType SlotTypeEnum;

	UFUNCTION()
	void OnActorClicked(AActor* TouchedActor, FKey ButtonPressed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
