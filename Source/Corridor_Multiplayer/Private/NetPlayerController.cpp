// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "GameManager.h"
#include "DraggableObstacle.h"


ANetPlayerController::ANetPlayerController()
{
}

void ANetPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetupInputComponent();
    TraceDistance = 10000.0f;
    bHasScrolled = false;
}

void ANetPlayerController::Tick(float DeltaTime)
{
    if (DraggableObstacle && DraggableObstacle->bIsDragging)
    {
        FVector MouseLocation, MouseDirection;
        DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

        FVector Start = PlayerCameraManager->GetCameraLocation();
        FVector End = Start + MouseDirection * TraceDistance;

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility))
        {            
            FVector NewLocation = HitResult.Location;

            NewLocation.Z = DraggableObstacle->GetActorLocation().Z; 
            DraggableObstacle->SetActorLocation(NewLocation);
        }
    } 
}

void ANetPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    bEnableClickEvents = true;
    bShowMouseCursor = true;
    ClickEventKeys.AddUnique(EKeys::RightMouseButton);

    if (InputComponent)
    {
        InputComponent->BindAction("Drag", IE_Pressed, this, &ANetPlayerController::StartDragging);
        InputComponent->BindAction("Drag", IE_Released, this, &ANetPlayerController::StopDragging);
        InputComponent->BindAxis("MouseWheel", this, &ANetPlayerController::OnMouseScroll);
    }
}

void ANetPlayerController::StartDragging()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

    if (HitResult.GetActor() && HitResult.GetActor()->IsA(ADraggableObstacle::StaticClass()))
    {
        DraggableObstacle = Cast<ADraggableObstacle>(HitResult.GetActor());
        if (DraggableObstacle)
        {
            DraggableObstacle->StartDragging();
        }
    }
}

void ANetPlayerController::StopDragging()
{
    if(DraggableObstacle)
    {
        DraggableObstacle->StopDragging();
        DraggableObstacle = nullptr;
    }
}

void ANetPlayerController::OnMouseScroll(float DeltaScroll)
{
    if (DeltaScroll != 0.0f && !bHasScrolled)
    {
        if (DraggableObstacle)
        {
            DraggableObstacle->RotateObstacle(DeltaScroll);
        }
        bHasScrolled = true;
    }
    else if (DeltaScroll == 0.0f)
    {
        bHasScrolled = false;
    }
}

void ANetPlayerController::SetPlayerActive(bool bIsActive)
{
    if (bIsActive)
    {
        EnableInput(this);
    }
    else
    {
        DisableInput(this);
    }
}
