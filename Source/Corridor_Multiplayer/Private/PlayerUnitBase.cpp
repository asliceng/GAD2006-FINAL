// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUnitBase.h"
#include "BoxSlot.h"

// Sets default values
APlayerUnitBase::APlayerUnitBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerUnitBase::AssignToSlot(ABoxSlot* NewSlot)
{
	check(NewSlot && NewSlot->Unit == nullptr);

	if (Slot) Slot->Unit = nullptr;
	Slot = NewSlot;
	Slot->Unit = this;
	SetActorLocation(Slot->GetActorLocation() + StartOffset);
}

// Called when the game starts or when spawned
void APlayerUnitBase::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void APlayerUnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

