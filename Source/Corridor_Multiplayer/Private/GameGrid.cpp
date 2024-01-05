// Fill out your copyright notice in the Description page of Project Settings.


#include "GameGrid.h"

AGameGrid* AGameGrid::GameGrid = nullptr;

// Sets default values
AGameGrid::AGameGrid() :
	NumCols(5),
	NumRows(5)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	GameGrid = this;
}

void AGameGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AGameGrid::SpawnAndLocateSlots(bool bIsItObstacle)
{
	ABoxSlot* Slot_Box = BoxClass->GetDefaultObject<ABoxSlot>();
	AObstacleSlot* Slot_Obstacle = ObstacleClass->GetDefaultObject<AObstacleSlot>();

	if (Slot_Box == nullptr) return;
	if (Slot_Obstacle == nullptr) return;

	FVector BoxExtend = Slot_Box->Box->GetScaledBoxExtent() * 2;
	FVector ObstacleExtend = Slot_Obstacle->Obstacle->GetScaledBoxExtent() * 2;

	FVector StartSlotLocation = GetActorLocation();

	int Rows;
	int Cols;
	UChildActorComponent* Grid;

	if (bIsItObstacle)
	{
		Rows = NumRows * 2 - 1;
		Cols = NumCols;
	}
	else
	{
		Rows = NumRows;
		Cols = NumCols;
	}

	for (int i = 0; i < Rows; i++)
	{
		for (int j = 0; j < Cols; j++)
		{
			FName ObstacleName(FString::Printf(TEXT("Obstacle%dx%d"), j, i));
			FName BoxName(FString::Printf(TEXT("Box%dx%d"), j, i));

			if (bIsItObstacle)
			{
				if (i % 2 == 0 && j == Cols - 1)
				{
					Grid = nullptr;
				}
				else
				{
					Grid = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass(), ObstacleName);		
				}
			}
			else
			{
				Grid = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass(), BoxName);

			}

			if (Grid != nullptr)
			{
				Grid->RegisterComponent();
				Grid->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);


				if (bIsItObstacle)
				{
					if (i % 2 == 0) //vertical obstacle
					{
						if (j + 2 <= NumCols)
						{
							Grid->SetRelativeRotation(FRotator(0, 0, 0));
							Grid->SetRelativeLocation(
								FVector(StartSlotLocation.X - i * (BoxExtend.X + ObstacleExtend.Y) / 2,
									StartSlotLocation.Y + (BoxExtend.Y + ObstacleExtend.Y) * (j + .5f), 0));
						}
					}
					else //horizontal obstacle
					{
						Grid->SetRelativeRotation(FRotator(0, 90, 0));
						Grid->SetRelativeLocation(
							FVector(StartSlotLocation.X - i * (BoxExtend.X + ObstacleExtend.Y) / 2,
								StartSlotLocation.Y + j * (BoxExtend.Y + ObstacleExtend.Y), 0));
					}

					GridActors.Add(Grid);

					Grid->SetChildActorClass(ObstacleClass);
					AObstacleSlot* ObstacleSlot = Cast<AObstacleSlot>(Grid->GetChildActor());

					ObstacleSlot->SetActorLabel(ObstacleName.ToString());
					ObstacleSlot->ObstaclePosition.Col = j;
					ObstacleSlot->ObstaclePosition.Row = i;
				}
				else
				{
					Grid->SetRelativeLocation(
						FVector(StartSlotLocation.X - i * (BoxExtend.X + ObstacleExtend.Y),
							StartSlotLocation.Y + j * (BoxExtend.Y + ObstacleExtend.Y), 0));

					GridActors.Add(Grid);
					Grid->SetChildActorClass(BoxClass);
					ABoxSlot* BoxSlot = Cast<ABoxSlot>(Grid->GetChildActor());

					BoxSlot->SetActorLabel(BoxName.ToString());
					BoxSlot->BoxPosition.Col = j;
					BoxSlot->BoxPosition.Row = i;
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void AGameGrid::BeginPlay()
{
	Super::BeginPlay();

	for (auto Grid : GridActors)
	{
		Grid->DestroyComponent();
	}

	GridActors.Empty();

	if (!BoxClass->IsValidLowLevel()) return;
	if (!ObstacleClass->IsValidLowLevel()) return;

	SpawnAndLocateSlots(false);
	SpawnAndLocateSlots(true);
}

// Called every frame
void AGameGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ABoxSlot* AGameGrid::GetBoxSlot(FSBoxPosition& Position)
{
	int GridIndex = Position.Row * NumCols + Position.Col;
	if (GridActors.IsValidIndex(GridIndex))
	{
		return Cast<ABoxSlot>(GridActors[GridIndex]->GetChildActor());
	}
	return nullptr;
}

ABoxSlot* AGameGrid::FindBoxSlot(FSBoxPosition Position)
{
	if (GameGrid)
	{
		return GameGrid->GetBoxSlot(Position);
	}
	return nullptr;
}

AObstacleSlot* AGameGrid::GetObstacleSlot(FSBoxPosition& Position)
{
	return nullptr;
}

AObstacleSlot* AGameGrid::FindObstacleSlot(FSBoxPosition Position)
{
	return nullptr;
}

