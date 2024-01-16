// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameGrid.h"
#include "PlayerUnitBase.h"
#include "NetPlayerController.h"
#include "GameManager.generated.h"

class DraggableObstacle;

USTRUCT(BlueprintType)
struct FSLevelInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TArray<FSUnitInfo> Units;
};

UCLASS()
class AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

	void OnActorClicked(AActor* SelectedBox);

	UPROPERTY(EditAnywhere)
	AGameGrid* GameGrid;

	UFUNCTION(BlueprintCallable)
	void SetupPlayers(FSLevelInfo& Info);

	UFUNCTION(BlueprintCallable)
	void SpawnObstaclesForPlayer(FSUnitInfo& UnitInfo);

	UFUNCTION()
	void SetPlayerWinningBoxes(FSUnitInfo& UnitInfo, ABoxSlot* Slot);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentLevel;

	UPROPERTY(EditAnywhere)
	TArray<FSLevelInfo> Levels;

	UPROPERTY()
	int32 CurrentPlayerIndex;

	UFUNCTION(BlueprintCallable)
	void SwitchPlayer();
	ANetPlayerController* GetCurrentPlayer();

	bool CheckBoxSlots();
	TArray<ABoxSlot*> AcceptableBoxes;
	void ClearBoxesState();

	int32 CheckHitInDirection(const APlayerUnitBase* Player, const FVector& TraceDirection, bool bHitPlayer);

	void RemoveObstacleFromPlayerList(ADraggableObstacle* Obstacle);

	UPROPERTY(BlueprintReadOnly)
	APlayerUnitBase* WinnerPlayer;

	UFUNCTION(BlueprintImplementableEvent)
	void OnGameOverScreen();

	void FloodFillCheck(); //oyuncu sayýsý kadar for'da floodfill çalýþtýracak
	void FloodFillRecursive(ABoxSlot* StartSlot, APlayerUnitBase* Player, TArray<ABoxSlot*> VisitedBoxes); // 
	void GetNeighborBoxes(ABoxSlot* CenterSlot, TArray<ABoxSlot*>& Neighbors); //
	bool DoesContainObstacleSlot(FSBoxPosition CheckingObstacle);

	UFUNCTION(BlueprintCallable)
	void ClearAllBoxStates();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	APlayerUnitBase* ThePlayer;
	TArray<TSubclassOf<APlayerUnitBase>> PlayerClasses;
	TArray<APlayerUnitBase*> Players;
	int32 ActivePlayerIndex;
	void ActivatePlayer(int32 PlayerIndex);
};
