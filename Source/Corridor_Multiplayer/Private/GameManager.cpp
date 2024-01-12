// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "DraggableObstacle.h"


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

                SpawnObstaclesForPlayer(UnitInfo);
            }
        }       
    }    

    if (Players.Num() > 0)
    {
        ActivePlayerIndex = (ActivePlayerIndex + 1) % Players.Num();
        ActivatePlayer(ActivePlayerIndex);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Player Num = %d"), Players.Num());
    }

}

void AGameManager::SpawnObstaclesForPlayer(FSUnitInfo& UnitInfo)
{
    FSPlayerInfo& PInfo = UnitInfo.PlayerInfo;
    PInfo.RemainingObstaclesNum = 10;  

    for (int32 i = 0; i < PInfo.RemainingObstaclesNum; ++i)
    {
        if (UnitInfo.ObstacleClass)
        {
            FVector SpawnLocation = UnitInfo.ObstacleSpawnLocation + FVector(i * 100, 0, 0);
            FRotator SpawnRotation = FRotator::ZeroRotator;

            ADraggableObstacle* SpawnedObstacle = GetWorld()->SpawnActor<ADraggableObstacle>(UnitInfo.ObstacleClass, SpawnLocation, SpawnRotation);

            if (SpawnedObstacle)
            {               
                PInfo.ObstacleList.Add(SpawnedObstacle);
                SpawnedObstacle->PlayerController = ThePlayer->NetPlayerController;
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UnitInfo.ObstacleClass not found"));
        }
    }
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

ANetPlayerController* AGameManager::GetCurrentPlayer()
{
    if (Players.IsValidIndex(CurrentPlayerIndex))
    {
        return Players[CurrentPlayerIndex]->NetPlayerController;
    }
    UE_LOG(LogTemp, Warning, TEXT("ActivePlayerController : NULL"));
    return nullptr;
}

void AGameManager::RemoveObstacleFromPlayerList(ADraggableObstacle* Obstacle)
{
    if (Levels.IsValidIndex(CurrentLevel))
    {
        for (auto& UnitInfo : Levels[CurrentLevel].Units)
        {
            FSPlayerInfo& PInfo = UnitInfo.PlayerInfo;

            UE_LOG(LogTemp, Error, TEXT("ObstacleList.Num() is = %d"), UnitInfo.PlayerInfo.ObstacleList.Num());

            //for (auto& Obs : PInfo.ObstacleList)
            //{
            //    UE_LOG(LogTemp, Error, TEXT("RemoveObstacleFromPlayerList"));
            //    UE_LOG(LogTemp, Error, TEXT("Removed Obstacle is = %s"), *Obs->GetName());
            //    /* if (Obs == Obstacle)
            //     {
            //         PInfo.ObstacleList.Remove(Obs);
            //         PInfo.RemainingObstaclesNum--;

            //         UE_LOG(LogTemp, Error, TEXT("Removed Obstacle is = %s"), *Obs->GetName());

            //         break;
            //     }*/
            //}

            for (auto ObsIt = PInfo.ObstacleList.CreateIterator(); ObsIt; ++ObsIt)
            {
                ADraggableObstacle* Obs = *ObsIt;
                if (Obs == Obstacle)
                {
                    ObsIt.RemoveCurrent();
                    PInfo.RemainingObstaclesNum--;

                    UE_LOG(LogTemp, Error, TEXT("Removed Obstacle is = %s"), *Obs->GetName());

                    break;
                }
            }


        }
    }
    
}
