// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitBase.h"
#include "NetPlayerController.h"


// Sets default values
AUnitBase::AUnitBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AUnitBase::AssignToSlot(int SlotTypeIndex)
{
	/*if (SlotType > ESlotType::ST_COUNT) return;*/

	if (SlotTypeIndex == 0)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, TEXT("BOX"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Black, TEXT("OBSTACLE"));
	}
}

void AUnitBase::ChangeSlotType(ESlotType SlotType)
{
	int SlotTypeIndex;

	switch (SlotType)
	{
	case ESlotType::ST_BoxSlot:
		SlotTypeIndex = 0;
		break;

	case ESlotType::ST_ObstacleSlot:
		SlotTypeIndex = 1;
		break;
	case ESlotType::ST_COUNT:
		SlotTypeIndex = 2;
		break;

	default: break;
	}

	AssignToSlot(SlotTypeIndex);
}


// Called when the game starts or when spawned
void AUnitBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AUnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

