// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

// Sets default values
AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerClasses.Add(APlayerUnitBase::StaticClass());
	PlayerClasses.Add(APlayerUnitBase::StaticClass());

	ActivePlayerIndex = -1; 

}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	
    CurrentPlayerIndex = 0;

    if (Levels.IsValidIndex(CurrentLevel))
    {
        SetupPlayers(Levels[CurrentLevel]);
    }
}

void AGameManager::SetupPlayers(FSLevelInfo& Info)
{
    ThePlayer = nullptr;

    for (auto UnitInfo : Info.Units)
    {
        if (ABoxSlot* Slot = GameGrid->GetBoxSlot(UnitInfo.StartPosition))
        {
            Slot->SpawnPlayerUnitHere(UnitInfo.UnitClass);

            if (Slot->Unit)
            {
                ThePlayer = Slot->Unit;
                Players.Add(ThePlayer);
                ThePlayer->bIsActivePlayer = false;

                ANetPlayerController* Controller = GetWorld()->SpawnActor<ANetPlayerController>(ANetPlayerController::StaticClass());
                if (Controller)
                {
                    ThePlayer->NetPlayerController = Controller;
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("NetPlayerController not found."));
                }
            }
        }
    }

    ActivePlayerIndex = (ActivePlayerIndex + 1) % Players.Num();
    ActivatePlayer(ActivePlayerIndex);

}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameManager::ActivatePlayer(int32 PlayerIndex)
{
    for (int32 i = 0; i < Players.Num(); ++i)
    {
        ThePlayer = Players[i];
        if (ThePlayer)
        {
            ThePlayer->bIsActivePlayer = (i == PlayerIndex);

            ANetPlayerController* Controller = ThePlayer->NetPlayerController;
            
            if (Controller)
            {
                Controller->SetPlayerActive(ThePlayer->bIsActivePlayer);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("NetPlayerController not found for ThePlayer at index %d."), i);
            }
        }      

        if (ThePlayer->bIsActivePlayer)
        {
            UE_LOG(LogTemp, Warning, TEXT("Player Index: %d   TRUE"), i);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Player Index: %d   FALSE"), i);

        }

    }

    CurrentPlayerIndex = (CurrentPlayerIndex + 1) % Players.Num(); 
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Current Player Index: %d"), CurrentPlayerIndex));
}

void AGameManager::SwitchPlayer()
{
    ActivatePlayer(CurrentPlayerIndex);
}
