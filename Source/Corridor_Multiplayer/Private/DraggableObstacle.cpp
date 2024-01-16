// Fill out your copyright notice in the Description page of Project Settings.


#include "DraggableObstacle.h"
#include "ObstacleSlot.h"

// Sets default values
ADraggableObstacle::ADraggableObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    Obstacle = CreateDefaultSubobject<UBoxComponent>(TEXT("Obstacle"));
    Obstacle->SetupAttachment(RootComponent);
    Obstacle->SetBoxExtent(FVector(110, 10, 2));
    Obstacle->SetCollisionResponseToAllChannels(ECR_Block);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultSlotMesh(TEXT("/Engine/BasicShapes/Cube"));
    ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleBox"));
    ObstacleMesh->SetupAttachment(Obstacle);
    ObstacleMesh->SetStaticMesh(DefaultSlotMesh.Object);

	bIsDragging = false;
}

// Called when the game starts or when spawned
void ADraggableObstacle::BeginPlay()
{
	Super::BeginPlay();

    GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
    if (!GameManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameManager not found!"));
    }
    DefaultLocation = GetActorLocation();

    OverlappingSlots.Empty();
}

void ADraggableObstacle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDragging)
    {
        CanPlaceObstacle();
    }
}

void ADraggableObstacle::StartDragging()
{
    if (PlayerController && GameManager && GameManager->GetCurrentPlayer() == PlayerController)
    {
        bIsDragging = true;

        FVector2D MousePosition;
        if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
        {
            FVector WorldLocation, WorldDirection;
            PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

            StartDragOffset = GetActorLocation() - FVector(WorldLocation.X, WorldLocation.Y, GetActorLocation().Z);
        }
    }
}

void ADraggableObstacle::StopDragging()
{ 
    if (bIsDragging)
    {
        bIsDragging = false;

        for (AObstacleSlot* OverlappingSlot : OverlappingSlots)
        {
            if (OverlappingSlot)
            {
                OverlappingSlot->SetState(EBoxState::GS_Default);
            }
        }

        OverlappingSlots.Empty();

        if (CanPlaceObstacle())
        {
            SetActorLocation(SnapToSlots());
            RemoveFromObstacleList();
            RemoveObstacleSlot();

            if (GameManager)
            {
                GameManager->SwitchPlayer();
            }
        }
        else
        {
            SetActorLocation(DefaultLocation);
            SetActorRotation(FRotator::ZeroRotator);
        }
    }  
}

void ADraggableObstacle::RotateObstacle(float DeltaScroll)
{
    if (bIsDragging)
    {       
        FRotator CurrentRotation = GetActorRotation();
        FRotator NewRotation = FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw + DeltaScroll * 90.0f, CurrentRotation.Roll);
        SetActorRotation(NewRotation);
    }
}

bool ADraggableObstacle::IsMouseOver()
{
    FVector WorldLocation, WorldDirection;
    FHitResult HitResult;

    FVector2D MousePosition;
    if (PlayerController && PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
    {
        PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(this);

        if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, WorldLocation + WorldDirection * 1000.f, ECC_Visibility, CollisionParams))
        {
            AActor* HitActor = HitResult.GetActor();
            return HitActor && HitActor == this;
        }
    }

    return false;
}

FVector ADraggableObstacle::SnapToSlots()
{   
    FVector MidPoint = (Slot1->GetActorLocation() + Slot2->GetActorLocation()) / 2.0f;

    Slot1->SetState(EBoxState::GS_Acceptable);
    Slot2->SetState(EBoxState::GS_Acceptable);

    return MidPoint;
}

void ADraggableObstacle::RemoveFromObstacleList()
{
    if (GameManager && Slot1 && Slot2)
    {
        UE_LOG(LogTemp, Warning, TEXT("Gitsin istedigim Obstacle = %s"), *this->GetName());
        GameManager->RemoveObstacleFromPlayerList(this);
    }
}

void ADraggableObstacle::RemoveObstacleSlot()
{
    if (GameGrid && Slot1 && Slot2)
    {
        int32 Index1 = INDEX_NONE;
        int32 Index2 = INDEX_NONE;
       
        for (int32 Index = 0; Index < GameGrid->GridActors.Num(); ++Index)
        {
            UChildActorComponent* ChildActorComponent = GameGrid->GridActors[Index];
            AObstacleSlot* ObstacleSlot = Cast<AObstacleSlot>(ChildActorComponent->GetChildActor());

            if (ObstacleSlot == Slot1) Index1 = Index;
            else if (ObstacleSlot == Slot2) Index2 = Index;
        }

        if (Index1 != INDEX_NONE) GameGrid->GridActors.RemoveAt(Index1);
        if (Index2 != INDEX_NONE) GameGrid->GridActors.RemoveAt(Index2);

        if (Slot1) Slot1->Destroy();
        if (Slot2) Slot2->Destroy();
    }
}

bool ADraggableObstacle::CheckSlots()
{
    Slot1 = nullptr;
    Slot2 = nullptr;

    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors, AObstacleSlot::StaticClass());

    for (AActor* OverlappingActor : OverlappingActors)
    {
        AObstacleSlot* ObstacleSlot = Cast<AObstacleSlot>(OverlappingActor);
        FRotator ObstacleSlotRotation = ObstacleSlot->Obstacle->GetComponentRotation();
        FRotator ObstacleRotation = Obstacle->GetComponentRotation();

        if (ObstacleSlot &&
            FMath::IsNearlyEqual(ObstacleSlotRotation.Pitch, ObstacleRotation.Pitch, KINDA_SMALL_NUMBER) &&
            FMath::IsNearlyEqual(ObstacleSlotRotation.Yaw, ObstacleRotation.Yaw, KINDA_SMALL_NUMBER) &&
            FMath::IsNearlyEqual(ObstacleSlotRotation.Roll, ObstacleRotation.Roll, KINDA_SMALL_NUMBER))
        {
            if (!Slot1)
            {
                Slot1 = ObstacleSlot;
            }
            else if (!Slot2)
            {
                Slot2 = ObstacleSlot;
            }
            OverlappingSlots.Add(ObstacleSlot);
        }
    }

    if (Slot1 && Slot2)
    {
        if (CheckObstacles())
        {
            Slot1->SetState(EBoxState::GS_Acceptable);
            Slot2->SetState(EBoxState::GS_Acceptable);

            return true;
        }
        else
        {
            Slot1->SetState(EBoxState::GS_Unacceptable);
            Slot2->SetState(EBoxState::GS_Unacceptable);
        }        
    }

    return false;
}

bool ADraggableObstacle::CheckObstacles()
{
    bool bItIsNotObs = true;

    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors, ADraggableObstacle::StaticClass());
    for (AActor* OverlappingActor : OverlappingActors)
    {
        if (OverlappingActor == this || OverlappingActor->IsA(ADraggableObstacle::StaticClass()))
        {            
            bItIsNotObs = false;
        }       
    }
    return bItIsNotObs;
}

bool ADraggableObstacle::CanPlaceObstacle()
{
    if(CheckSlots() &&
        CheckObstacles())
    {
        return true;
    }
    
    return false;
}

