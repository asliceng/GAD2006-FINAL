// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxSlot.h"
#include "NetPlayerController.h"
#include "GameManager.h"
#include "PlayerUnitBase.h"

// Sets default values
ABoxSlot::ABoxSlot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);
	Box->SetBoxExtent(FVector(50, 50, 2));
	Box->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultSlotMesh(TEXT("/Engine/BasicShapes/Plane"));
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(RootComponent);
	Plane->SetStaticMesh(DefaultSlotMesh.Object);

	DefaultMaterial = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Script/Engine.Material'/Game/Materials/M_Default_Box.M_Default_Box'")).Object;
	SetState(GS_Default);

}

void ABoxSlot::OnActorClicked(AActor* TouchedBox, FKey ButtonPressed)
{
	if (GameManager) GameManager->OnActorClicked(TouchedBox);
}

void ABoxSlot::SpawnPlayerUnitHere(TSubclassOf<APlayerUnitBase>& UnitClass)
{
	FVector Location = GetActorLocation();
	APlayerUnitBase* NewUnit = Cast<APlayerUnitBase>(GWorld->SpawnActor(UnitClass, &Location));
	if (NewUnit)
	{
		NewUnit->AssignToSlot(this);
	}
}

void ABoxSlot::SetState(EBoxState NewState)
{
	BoxState = NewState;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		AcceptableSlotMaterial(TEXT("/Script/Engine.Material'/Game/Materials/M_Acceptable.M_Acceptable'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		UnacceptableSlotMaterial(TEXT("/Script/Engine.Material'/Game/Materials/M_Unacceptable.M_Unacceptable'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		FloodFillSlotMaterial(TEXT("/Script/Engine.Material'/Game/Materials/M_FloodFill.M_FloodFill'"));

	switch (NewState)
	{
	case GS_Default:
		Plane->SetMaterial(0, DefaultMaterial);
		break;
	case GS_Acceptable:
		Plane->SetMaterial(0, AcceptableSlotMaterial.Object);
		break;
	case GS_Unacceptable:
		Plane->SetMaterial(0, UnacceptableSlotMaterial.Object);
		break;
	case GS_FloodFill:
		Plane->SetMaterial(0, FloodFillSlotMaterial.Object);
		break;
	}
}

// Called when the game starts or when spawned
void ABoxSlot::BeginPlay()
{
	Super::BeginPlay();
	OnClicked.AddUniqueDynamic(this, &ABoxSlot::OnActorClicked);	
}

// Called every frame
void ABoxSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

