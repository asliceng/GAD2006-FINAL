// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerClasses.Add(ANetBaseCharacter::StaticClass());
	PlayerClasses.Add(ANetBaseCharacter::StaticClass());

	ActivePlayerIndex = -1; // No active player initially

}

void AGameManager::SetupPlayers()
{
    for (int32 i = 0; i < PlayerClasses.Num(); ++i)
    {
        // Spawn player characters
        ANetBaseCharacter* NewPlayer = GetWorld()->SpawnActor<ANetBaseCharacter>(PlayerClasses[i], FVector::ZeroVector, FRotator::ZeroRotator);
        if (NewPlayer)
        {
            Players.Add(NewPlayer);
            NewPlayer->bIsActivePlayer = false;
        }
    }

    ActivePlayerIndex = (ActivePlayerIndex + 1) % Players.Num();
    ActivatePlayer(ActivePlayerIndex);
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	
    CurrentPlayerIndex = 0;
    SetupPlayers();
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
        ANetBaseCharacter* Player = Players[i];
        if (Player)
        {
            // Activate the specified player
            Player->bIsActivePlayer = (i == PlayerIndex);

            // Get the associated player controller
            ANetPlayerController* NetPlayerController = Cast<ANetPlayerController>(Player->GetController());
            if (NetPlayerController)
            {
                // Set player active status in the NetPlayerController
                NetPlayerController->SetPlayerActive(Player->bIsActivePlayer);
            }
        }
    }

    CurrentPlayerIndex = (CurrentPlayerIndex + 1) % Players.Num();

    // Print the current player index to the screen
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Current Player Index: %d"), CurrentPlayerIndex));
}

void AGameManager::SwitchPlayer()
{
    // Assuming this function is called when an object is dropped

    // Switch to the next player
    ActivatePlayer(CurrentPlayerIndex);
}

//void AGameManager::InitializeGridActors(AGameGrid* GameGrid)
//{
//    //if (!GameGrid)
//    //{
//    //    UE_LOG(LogTemp, Error, TEXT("GameManager: GameGrid is null."));
//    //    return;
//    //}
//
//    //GameGrid->GridActors.Empty(); 
//
//    //for (auto& GridActor : GameGrid->GridActors)
//    //{
//    //    if (GridActor.IsValid())
//    //    {
//    //        TSharedRef<UChildActorComponent> SharedGridActor = MakeShareable(GridActor.Get());
//    //        GridActors.Add(SharedGridActor);
//    //    }
//
//    //    UE_LOG(LogTemp, Warning, TEXT("AAAAAAAAAAAAAAA"));
//    //    //UE_LOG(LogTemp, Warning, TEXT(SharedGridActor));
//    //}
//}
//
//void AGameManager::DestroyGridActors()
//{
//    //for (auto& GridActor : GridActors)
//    //{
//    //    if (GridActor.IsValid())
//    //    {
//    //        GridActor->DestroyComponent();
//    //    }
//    //}
//
//    //GridActors.Empty();
//}

