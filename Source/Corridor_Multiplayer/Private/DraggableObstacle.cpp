// Fill out your copyright notice in the Description page of Project Settings.


#include "DraggableObstacle.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ADraggableObstacle::ADraggableObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	IsDragging = false;
}

// Called when the game starts or when spawned
void ADraggableObstacle::BeginPlay()
{
	Super::BeginPlay();
	
    if (GetWorld() && GetWorld()->GetFirstPlayerController())
    {
        // Enable input for the player controller
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            EnableInput(PlayerController);
            PlayerController->InputComponent->BindAction("Drag", IE_Pressed, this, &ADraggableObstacle::StartDragging);
            PlayerController->InputComponent->BindAction("Drag", IE_Released, this, &ADraggableObstacle::StopDragging);
        }
    }
}

// Called every frame
void ADraggableObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (IsDragging)
    {
        if (GetWorld() && GetWorld()->GetFirstPlayerController())
        {
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (PlayerController)
            {
                FVector MouseLocation, MouseDirection;
                PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

                FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
                FVector End = Start + MouseDirection * 1000.f; // Adjust the drag distance as needed

                FHitResult HitResult;
                FCollisionQueryParams CollisionParams;
                CollisionParams.AddIgnoredActor(this); // Ignore self to prevent hit on the dragged object

                if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
                {
                    SetActorLocation(HitResult.Location);
                }
            }
        }
    }
}

void ADraggableObstacle::StartDragging()
{
    if (IsMouseOver())
    {
        IsDragging = true;

        if (GetWorld() && GetWorld()->GetFirstPlayerController())
        {
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (PlayerController)
            {
                FVector MouseLocation, MouseDirection;
                PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

                FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
                FVector End = Start + MouseDirection * 1000.f;

                FHitResult HitResult;
                FCollisionQueryParams CollisionParams;

                if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
                {
                    // Calculate the offset between the mouse and the object
                    StartDragOffset = HitResult.Location - GetActorLocation();
                }
            }
        }
    }
}

void ADraggableObstacle::StopDragging()
{
    IsDragging = false;
}

bool ADraggableObstacle::IsMouseOver()
{
    if (GetWorld() && GetWorld()->GetFirstPlayerController())
    {
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            FVector MouseLocation, MouseDirection;
            PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

            FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
            FVector End = Start + MouseDirection * 1000.f; // Adjust the ray length as needed

            FHitResult HitResult;
            FCollisionQueryParams CollisionParams;

            if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
            {
                return HitResult.GetActor() == this;
            }
        }
    }

    return false;
}

