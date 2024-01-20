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

        for (auto Box : Player->WinningBoxes)
        {
            if (Slot == Box)
            {
                UE_LOG(LogTemp, Warning, TEXT("%s kazandý: %s"), *Player->GetActorLabel(), *Slot->GetActorLabel());
                WinnerPlayer = Player;
                OnGameOverScreen();
                return;
            }
        }
        ClearAllBoxStates();
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
                ThePlayer->StaticMesh->SetOverlayMaterial(UnitInfo.PlayerInfo.PlayerColor);
                //ThePlayer->PInfo = UnitInfo.PlayerInfo;

                ANetPlayerController* Controller = GetWorld()->SpawnActor<ANetPlayerController>(ANetPlayerController::StaticClass());
                if (Controller)
                {
                    ThePlayer->NetPlayerController = Controller;
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("NetPlayerController not found."));
                }

                SetPlayerWinningBoxes(UnitInfo, Slot);
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
                SpawnedObstacle->GameGrid = GameGrid;
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UnitInfo.ObstacleClass not found"));
        }
    }
}

void AGameManager::SetPlayerWinningBoxes(FSUnitInfo& UnitInfo, ABoxSlot* Slot)
{
    APlayerUnitBase* Player = ThePlayer;
    FSBoxPosition PlayerSlotPosition = Slot->BoxPosition;
    int32 WinningCol;

    if (PlayerSlotPosition.Col == 0) WinningCol = 8;
    else WinningCol = 0;

    for (int32 Row = 0; Row < GameGrid->NumRows; ++Row)
    {
        ABoxSlot* WinningBox = AGameGrid::FindBoxSlot(FSBoxPosition(WinningCol, Row));

        if (WinningBox)
        {
            Player->WinningBoxes.Add(WinningBox);      
            WinningBox->DefaultMaterial = UnitInfo.PlayerInfo.PlayerColor;
            WinningBox->SetState(GS_Default);
        }
    }
}

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
            int32 SkipBox;
            bool bHitPlayer = false;

            //Up
            if (CheckHitInDirection(Player, FVector::ForwardVector, bHitPlayer) == 2)
            {
                SkipBox = 2;
                    Player = Players[(CurrentPlayerIndex + 1) % Players.Num()];
                    bHitPlayer = true;
            }
            else SkipBox = 1;
            if (Row - SkipBox >= 0 && CheckHitInDirection(Player, FVector::ForwardVector, bHitPlayer) != 1)
            {        
                ABoxSlot* UpSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col, Row - SkipBox));
                UpSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(UpSlot);
            }
            Player = Players[CurrentPlayerIndex];
            bHitPlayer = false;


            //Down
            if (CheckHitInDirection(Player, FVector::BackwardVector, bHitPlayer) == 2)
            {
                SkipBox = 2;
                Player = Players[(CurrentPlayerIndex + 1) % Players.Num()];
                bHitPlayer = true;
            }
            else SkipBox = 1;
            if (Row + SkipBox <= 8 && CheckHitInDirection(Player, FVector::BackwardVector, bHitPlayer) != 1)
            {
                ABoxSlot* DownSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col, Row + SkipBox));
                DownSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(DownSlot);
            }
            bHitPlayer = false;
            Player = Players[CurrentPlayerIndex];

            //Right
            if (CheckHitInDirection(Player, FVector::RightVector, bHitPlayer) == 2)
            {
                SkipBox = 2;
                Player = Players[(CurrentPlayerIndex + 1) % Players.Num()];
                bHitPlayer = true;
            }
            else SkipBox = 1;
            if (Col + SkipBox <= 8 && CheckHitInDirection(Player, FVector::RightVector, bHitPlayer)!= 1)
            {
                ABoxSlot* RightSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col + SkipBox, Row));
                RightSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(RightSlot);
            }
            bHitPlayer = false;
            Player = Players[CurrentPlayerIndex];

            //Left
            if (CheckHitInDirection(Player, FVector::LeftVector, bHitPlayer) == 2)
            {
                SkipBox = 2;
                Player = Players[(CurrentPlayerIndex + 1) % Players.Num()];
                bHitPlayer = true;
            }
            else SkipBox = 1; 
            if (Col - SkipBox >= 0 && CheckHitInDirection(Player, FVector::LeftVector, bHitPlayer) != 1)
            {
                ABoxSlot* LeftSlot = AGameGrid::FindBoxSlot(FSBoxPosition(Col - SkipBox, Row));
                LeftSlot->SetState(EBoxState::GS_Acceptable);
                AcceptableBoxes.Add(LeftSlot);
            }
            bHitPlayer = false;
            Player = Players[CurrentPlayerIndex];

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

int32 AGameManager::CheckHitInDirection(const APlayerUnitBase* Player, const FVector& TraceDirection, bool bHitPlayer)
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
            DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 10, 0, 10); //LINETRACE GÖRÜNTÜSÜ -> çarpýþma oldu

            AActor* HitActor = HitResult.GetActor();
            ADraggableObstacle* DraggableObstacle = Cast<ADraggableObstacle>(HitActor);
            if (DraggableObstacle)
            {
                return 1; 
            }
           
            if (!bHitPlayer)
            {
                APlayerUnitBase* PlayerUnit = Cast<APlayerUnitBase>(HitActor);
                if (PlayerUnit)
                {
                    return 2;
                }
            }                       
        }
    }      
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10, 0, 2.0f); //LINETRACE GÖRÜNTÜSÜ -> çarpýþma olmadý
    //CollisionParams.ClearIgnoredActors();
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

bool AGameManager::FloodFillCheck()
{
    bool bCanPlaceObstacle;
    for (auto CurrentPlayer : Players)
    {
        ABoxSlot* CurrentSlot = CurrentPlayer->Slot;
        TArray<ABoxSlot*> VisitedBoxes;
        FloodFillRecursive(CurrentSlot, CurrentPlayer, VisitedBoxes);

        for (int32 i = 0; i < CurrentPlayer->WinningBoxes.Num(); ++i)
        {
            ABoxSlot* CurrentBox = CurrentPlayer->WinningBoxes[i];
            if (CurrentBox->bFloodFill)
            {
                bCanPlaceObstacle = true;
                ClearAllBoxStates();
                break;
            }
            else
            {
                bCanPlaceObstacle = false;
            }
        }
        if (!bCanPlaceObstacle)
        {
            break;
        }
    }
    return bCanPlaceObstacle;
}

void AGameManager::FloodFillRecursive(ABoxSlot* StartSlot, APlayerUnitBase* Player, TArray<ABoxSlot*> VisitedBoxes)
{
    if (!StartSlot || !Player || VisitedBoxes.Contains(StartSlot)) return;

    VisitedBoxes.Add(StartSlot);

    TArray<ABoxSlot*> Neighbors;
    GetNeighborBoxes(StartSlot, Neighbors);

    for (ABoxSlot* Neighbor : Neighbors)
    {
        //Debug Color for Flood Fill
        //if (Neighbor && Neighbor->BoxState != EBoxState::GS_FloodFill)
        //{
        //    Neighbor->SetState(EBoxState::GS_FloodFill);

        //    FloodFillRecursive(Neighbor, Player, VisitedBoxes);
        //}

        if (Neighbor && !Neighbor->bFloodFill)
        {
            Neighbor->bFloodFill = true;
            if (Neighbor->bFloodFill)
            {
                FloodFillRecursive(Neighbor, Player, VisitedBoxes);
            }
        }
    }
}

void AGameManager::GetNeighborBoxes(ABoxSlot* CenterSlot, TArray<ABoxSlot*>& Neighbors)
{
    if (!CenterSlot) return;
    FString CenterSlotName = CenterSlot->GetActorLabel();

    CenterSlotName.ReplaceInline(TEXT("Box"), TEXT(""));
    TArray<FString> StringParts;
    CenterSlotName.ParseIntoArray(StringParts, TEXT("x"), true);

    int32 CenterCol, CenterRow;
    if (StringParts.Num() == 2)
    {
        FString ColString = StringParts[0];
        FString RowString = StringParts[1];

        CenterCol = FCString::Atoi(*ColString);
        CenterRow = FCString::Atoi(*RowString);
    }

    CenterCol = FCString::Atoi(*StringParts[0]);
    CenterRow = FCString::Atoi(*StringParts[1]);

    static const FSBoxPosition NeighborOffsets[] = {
        FSBoxPosition(-1, 0), //sol
        FSBoxPosition(1, 0),  //sað
        FSBoxPosition(0, -1), // yukarý
        FSBoxPosition(0, 1)   //aþaðý
    };

    const FSBoxPosition ObstacleOffset[] = {
        FSBoxPosition(CenterCol - 1, CenterRow * 2), //sol
        FSBoxPosition(CenterCol, CenterRow * 2),  //sað
        FSBoxPosition(CenterCol, CenterRow * 2 - 1), // yukarý
        FSBoxPosition(CenterCol, CenterRow * 2 + 1)   //aþaðý
    };

    FSBoxPosition CenterPosition;
    CenterPosition.Col = CenterCol;
    CenterPosition.Row = CenterRow;

    for (int8 i = 0; i < sizeof(NeighborOffsets); i++)
    {
        FSBoxPosition NeighborPosition = CenterPosition + NeighborOffsets[i];

        if (DoesContainObstacleSlot(ObstacleOffset[i]))
        {
            if (NeighborPosition.Col >= 0 && NeighborPosition.Col < GameGrid->NumCols &&
                NeighborPosition.Row >= 0 && NeighborPosition.Row < GameGrid->NumRows)
            {
                FString NeighborName = FString::Printf(TEXT("Box%dx%d"), NeighborPosition.Col, NeighborPosition.Row);

                for (UChildActorComponent* GridActor : GameGrid->GridActors)
                {
                    ABoxSlot* Neighbor = Cast<ABoxSlot>(GridActor->GetChildActor());
                    if (Neighbor && Neighbor->GetActorLabel() == NeighborName)
                    {
                        Neighbors.Add(Neighbor);
                    }
                }
            }          
        }       
    }
}

bool AGameManager::DoesContainObstacleSlot(FSBoxPosition CheckingObstacle)
{
    FString ObstacleSlotName = FString::Printf(TEXT("Obstacle%dx%d"), CheckingObstacle.Col, CheckingObstacle.Row);

    for (UChildActorComponent* GridActor : GameGrid->GridActors)
    {
        AObstacleSlot* Neighbor = Cast<AObstacleSlot>(GridActor->GetChildActor());
        if (Neighbor && Neighbor->GetActorLabel() == ObstacleSlotName &&
            Slot1->GetActorLabel() != ObstacleSlotName &&
            Slot2->GetActorLabel() != ObstacleSlotName)
        {
            return true;
        }
    }
    return false;
}

void AGameManager::ClearAllBoxStates()
{
    if (!GameGrid) return;

    for (UChildActorComponent* ChildActorComponent : GameGrid->GridActors)
    {
        ABoxSlot* Box = Cast<ABoxSlot>(ChildActorComponent->GetChildActor());

        if (Box)
        {
            Box->bFloodFill = false;        
        }
    }
}


