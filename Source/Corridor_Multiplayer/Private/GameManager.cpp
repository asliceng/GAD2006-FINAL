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
    }

    CurrentPlayerIndex = (CurrentPlayerIndex + 1) % Players.Num(); 
    ClearBoxesState();
    CheckBoxSlots();
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

bool AGameManager::CheckBoxSlots()
{
    APlayerUnitBase* Player = Players[CurrentPlayerIndex];

    if (Player)
    {       
        ABoxSlot* PlayerBoxSlot = AGameGrid::FindBoxSlot(Player->Slot->BoxPosition);
        FString BoxName = PlayerBoxSlot->GetActorLabel();

        BoxName.ReplaceInline(TEXT("Box"), TEXT(""));
        TArray<FString> StringParts;
        BoxName.ParseIntoArray(StringParts, TEXT("x"), true);

        int32 Col, Row;
        if (StringParts.Num() == 2)
        {
            FString ColString = StringParts[0];
            FString RowString = StringParts[1]; 

            Col = FCString::Atoi(*ColString);
            Row = FCString::Atoi(*RowString);
        }

        if (PlayerBoxSlot)
        {
            if (Row - 1 >= 0) //Up
            {
                ABoxSlot* UpSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col, Row - 1));
                UpSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(UpSlot);
            }           
            if (Row + 1 <= 8) //Down
            {
                ABoxSlot* DownSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col, Row + 1));
                DownSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(DownSlot);
            }
            if (Col + 1 <= 8) //Right
            {
                ABoxSlot* RightSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col + 1, Row));
                RightSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(RightSlot);
            }
            if (Col - 1 >= 0) //Left
            {
                ABoxSlot* LeftSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col - 1, Row));
                LeftSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(LeftSlot);
            }

            return true;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Player's current slot not found!"));
        }
    }
    return false;
}

void AGameManager::ClearBoxesState()
{
    for (auto Box : AcceptableBoxes)
    {
        Box->SetState(EBoxState::GS_Default);
    }
    AcceptableBoxes.Empty();
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

            /*for (auto ObsIt = PInfo.ObstacleList.CreateIterator(); ObsIt; ++ObsIt)
            {
                UE_LOG(LogTemp, Error, TEXT("girdi"));
                ADraggableObstacle* Obs = *ObsIt;
                if (Obs == Obstacle)
                {
                    ObsIt.RemoveCurrent();
                    PInfo.RemainingObstaclesNum--;

                    UE_LOG(LogTemp, Error, TEXT("Removed Obstacle is = %s"), *Obs->GetName());

                    break;
                }
            }*/
        }
    }
    
}
