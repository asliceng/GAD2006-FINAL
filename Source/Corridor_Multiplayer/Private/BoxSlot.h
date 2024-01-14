// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BoxSlot.generated.h"

class APlayerUnitBase;
class AGameManager;

USTRUCT(Blueprintable)
struct FSBoxPosition
{
	GENERATED_USTRUCT_BODY();

	FSBoxPosition() { }
	FSBoxPosition(uint8 col, uint8 row) :
		Col(col),
		Row(row) { }

	UPROPERTY(EditAnywhere)
	uint8 Col;

	UPROPERTY(EditAnywhere)
	uint8 Row;
};

UENUM(Blueprintable)
enum EBoxState
{
	GS_Default,
	GS_Acceptable,
	GS_Unacceptable,
};

UCLASS()
class ABoxSlot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoxSlot();

	UPROPERTY(EditAnywhere)
	UBoxComponent* Box;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Plane;

	UPROPERTY(BlueprintReadWrite)
	FSBoxPosition BoxPosition;

	EBoxState BoxState;

	AGameManager* GameManager;

	UFUNCTION()
	void OnActorClicked(AActor* TouchedBox, FKey ButtonPressed);

	UPROPERTY(VisibleAnywhere)
	APlayerUnitBase* Unit;

	void SpawnPlayerUnitHere(TSubclassOf<APlayerUnitBase >& UnitClass);

	UFUNCTION()
	void SetState(EBoxState NewState);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
