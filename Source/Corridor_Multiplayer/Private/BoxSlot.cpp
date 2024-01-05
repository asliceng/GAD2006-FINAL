// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxSlot.h"
#include "NetPlayerController.h"

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

	SetState(GS_Default);
}

void ABoxSlot::OnGridClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (auto PlayerController = GWorld->GetFirstPlayerController<ANetPlayerController>())
	{
		PlayerController->OnActorClicked(this, ButtonPressed);
	}
}

void ABoxSlot::SetState(EBoxState NewState)
{
	BoxState = NewState;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		DefaultSlotMaterial(TEXT("/Script/Engine.Material'/Game/Materials/M_Default.M_Default'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		DefaultSlotMaterial_A(TEXT("/Script/Engine.Material'/Game/Materials/M_Acceptable.M_Acceptable'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		DefaultSlotMaterial_U(TEXT("/Script/Engine.Material'/Game/Materials/M_Unacceptable.M_Unacceptable'"));

	switch (NewState)
	{
	case GS_Default:
		Plane->SetMaterial(0, DefaultSlotMaterial.Object);
		break;
	case GS_Acceptable:
		Plane->SetMaterial(0, DefaultSlotMaterial_A.Object);
		break;
	case GS_Unacceptable:
		Plane->SetMaterial(0, DefaultSlotMaterial_U.Object);
		break;

	}
}

// Called when the game starts or when spawned
void ABoxSlot::BeginPlay()
{
	Super::BeginPlay();
	OnClicked.AddUniqueDynamic(this, &ABoxSlot::OnGridClicked);	
}

// Called every frame
void ABoxSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

