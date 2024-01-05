// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"

ANetPlayerController::ANetPlayerController()
{
}

void ANetPlayerController::BeginPlay()
{
	bEnableClickEvents = true;
	bShowMouseCursor = true;
	ClickEventKeys.AddUnique(EKeys::RightMouseButton);
}

void ANetPlayerController::OnActorClicked(AActor* Actor, FKey key)
{
	UE_LOG(LogTemp, Warning, TEXT("OnClicked: %s - %s"), *Actor->GetActorLabel(), *key.ToString());
}