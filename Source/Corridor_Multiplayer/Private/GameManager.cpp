// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "DraggableObstacle.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerClasses.Add(APlayerUnitBase::StaticClass());
	PlayerClasses.Add(APlayerUnitBase::StaticClass());

	ActivePlayerIndex = -1; 
}

void AGameManager::OnActorClicked(AActor* SelectedBox)
{
    APlayerUnitBase* Player = Players[CurrentPlayerIndex];
    ABoxSlot* Slot = Cast<ABoxSlot>(SelectedBox);

    if (!Slot) return;

    if (Slot->BoxState == EBoxState::GS_Acceptable)
    {
        Player->AssignToSlot(Slot);
        SwitchPlayer();
    }
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
            int32 SkipBox = 1;

            /*if (CheckHitInDirection(Player, FVector::ForwardVector) == 2) SkipBox = 2;
            else SkipBox = 1;*/
            if (Row - SkipBox >= 0 && CheckHitInDirection(Player, FVector::ForwardVector) != 1) //Up
            {        
                ABoxSlot* UpSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col, Row - SkipBox));
                UpSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(UpSlot);
            }

            if (CheckHitInDirection(Player, FVector::BackwardVector) == 2) SkipBox = 2;
            else SkipBox = 1;
            if (Row + SkipBox <= 8 && CheckHitInDirection(Player, FVector::BackwardVector) != 1) //Down
            {
                

                ABoxSlot* DownSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col, Row + SkipBox));
                DownSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(DownSlot);
            }

            if (CheckHitInDirection(Player, FVector::RightVector) == 2) SkipBox = 2;
            else SkipBox = 1;
            if (Col + SkipBox <= 8 && CheckHitInDirection(Player, FVector::RightVector)!= 1) //Right
            {
               

                ABoxSlot* RightSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col + SkipBox, Row));
                RightSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(RightSlot);
            }
            if (CheckHitInDirection(Player, FVector::LeftVector) == 2) SkipBox = 2;
            else SkipBox = 1;
            if (Col - SkipBox >= 0 && CheckHitInDirection(Player, FVector::LeftVector) != 1) //Left
            {
                

                ABoxSlot* LeftSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col - SkipBox, Row));
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

int32 AGameManager::CheckHitInDirection(const APlayerUnitBase* Player, const FVector& TraceDirection)
{
    //0 = nothing
    //1 = Obstacle
    //2 = Opponent
    TArray<FHitResult> HitResults;
    const FVector& StartLocation = Player->GetActorLocation() + FVector (0,0,30);
    float TraceDistance = 100;
    FVector EndLocation = StartLocation + TraceDirection * TraceDistance;

    // Linetrace
    FCollisionQueryParams CollisionParams;
    TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
    CollisionParams.AddIgnoredActor(Player);

    bool bHit = GetWorld()->LineTraceMultiByChannel(HitResults, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

    if (bHit)
    {
        for (const FHitResult& HitResult : HitResults)
        {
            DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Green, false, 10, 0, 10); //LINETRACE GÖRÜNTÜSÜ -> çarpýþma oldu

            AActor* HitActor = HitResult.GetActor();
            ADraggableObstacle* DraggableObstacle = Cast<ADraggableObstacle>(HitActor);
            if (DraggableObstacle)
            {
                UE_LOG(LogTemp, Error, TEXT("DraggableObs"));
                return 1; 
            }

            APlayerUnitBase* PlayerUnit = Cast<APlayerUnitBase>(HitActor);
            if (PlayerUnit)
            {
                UE_LOG(LogTemp, Error, TEXT("APlayerUnitBase hit"));
                return 2;
            }
        }
    }      
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10, 0, 2.0f); //LINETRACE GÖRÜNTÜSÜ -> çarpýþma olmadý
    UE_LOG(LogTemp, Error, TEXT("Nothing"));

    return 0;
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
