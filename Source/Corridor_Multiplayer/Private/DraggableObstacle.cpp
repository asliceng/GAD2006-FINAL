// Fill out your copyright notice in the Description page of Project Settings.


#include "DraggableObstacle.h"

// Sets default values
ADraggableObstacle::ADraggableObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    Obstacle = CreateDefaultSubobject<UBoxComponent>(TEXT("Obstacle"));
    Obstacle->SetupAttachment(RootComponent);
    Obstacle->SetBoxExtent(FVector(22, 10, 2));
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

    PlayerController = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());

    GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
    if (!GameManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameManager not found!"));
    }
}

void ADraggableObstacle::StartDragging()
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

void ADraggableObstacle::StopDragging()
{ 
    if (bIsDragging)
    {
        bIsDragging = false;
        if (GameManager)
        {
            GameManager->SwitchPlayer();
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

        //UE_LOG(LogTemp, Warning, TEXT("CurrentRotation: %s"), *CurrentRotation.ToString());

        // Log NewRotation
        //UE_LOG(LogTemp, Warning, TEXT("NewRotation: %s"), *NewRotation.ToString());
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


