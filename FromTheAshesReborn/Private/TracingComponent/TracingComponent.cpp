#include "TracingComponent/TracingComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UTracingComponent::UTracingComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	

}

void UTracingComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UTracingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	int privateModuloSkip = 1;
	if (ModuloSkip >= 1)
		privateModuloSkip = ModuloSkip;


	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CanTrace /*&& Substepping == false*/)
	{
		if (ModuloNumber % privateModuloSkip == 0)
		{
			if (bHitSameSocketAtDifferentTimes)
			{
				HitSameSocketAtDifferentTimes();
			}

			if (bHitOtherSocketsAtSameTime)
			{
				HitOtherSocketsAtSameTime();
			}

			if (bHitOtherSocketsAtDifferentTime)
			{
				HitOtherSocketsAtDifferentTime();
			}
		}

		UpdateLastSocketLocation();
		ModuloNumber++;
	}
}

void UTracingComponent::HitSameSocketAtDifferentTimes()
{
	for (auto Socket : MySockets)
	{
		//Private Variables
		const FVector* Start = LastKnownSocketLocation.Find(Socket);
		const FVector End = MyPrimitive->GetSocketLocation(Socket);
		TArray<FHitResult> OutHits;

		if (Start == nullptr)
		{
			return;
		}

		if (TraceByChannelOrObjects)
		{
			switch (MyKismetTraceType)
			{
			case ETraceType::LineTrace:
			{
				UKismetSystemLibrary::LineTraceMulti(MyActor, *Start, End, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
				AddHitToHitArray(OutHits);  break;
			}

			case ETraceType::BoxTrace:
			{
				UKismetSystemLibrary::BoxTraceMulti(MyActor, *Start, End, BoxHalfSize, BoxOrientation, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
				break;
			}

			case ETraceType::CapsuleTrace:
			{
				UKismetSystemLibrary::CapsuleTraceMulti(MyActor, *Start, End, CapsuleHalfHeight, CapsuleRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
				break;
			}

			case ETraceType::SphereTrace:
			{
				UKismetSystemLibrary::SphereTraceMulti(MyActor, *Start, End, SphereRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
				break;
			}
			}
		}
		else if (!TraceByChannelOrObjects)
		{
			switch (MyKismetTraceType)
			{
			case ETraceType::LineTrace:
			{
				UKismetSystemLibrary::LineTraceMultiForObjects(MyActor, *Start, End, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
				AddHitToHitArray(OutHits);  break;
			}

			case ETraceType::BoxTrace:
			{
				UKismetSystemLibrary::BoxTraceMultiForObjects(MyActor, *Start, End, BoxHalfSize, BoxOrientation, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
				break;
			}

			case ETraceType::CapsuleTrace:
			{
				UKismetSystemLibrary::CapsuleTraceMultiForObjects(MyActor, *Start, End, CapsuleHalfHeight, CapsuleRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
				break;
			}

			case ETraceType::SphereTrace:
			{
				UKismetSystemLibrary::SphereTraceMultiForObjects(MyActor, *Start, End, SphereRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
				break;
			}
			}
		}
		AddHitToHitArray(OutHits);
	}
	
}

void UTracingComponent::HitOtherSocketsAtSameTime()
{
	
	for (auto Socket1 : MySockets)
	{
		for (auto Socket2 : MySockets)
		{
			//Private Variables
			const FVector Start = MyPrimitive->GetSocketLocation(Socket1);
			const FVector End = MyPrimitive->GetSocketLocation(Socket2);
			TArray<FHitResult> OutHits;

			if (TraceByChannelOrObjects)
			{
				switch (MyKismetTraceType)
				{
				case ETraceType::LineTrace:
				{
					UKismetSystemLibrary::LineTraceMulti(MyActor, Start, End, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					//UKismetSystemLibrary::LineTraceMulti(MyWorldContextObject, Start, End, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					AddHitToHitArray(OutHits); break;

				}

				case ETraceType::BoxTrace:
				{
					UKismetSystemLibrary::BoxTraceMulti(MyActor, Start, End, BoxHalfSize, BoxOrientation, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::CapsuleTrace:
				{
					UKismetSystemLibrary::CapsuleTraceMulti(MyActor, Start, End, CapsuleHalfHeight, CapsuleRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::SphereTrace:
				{
					UKismetSystemLibrary::SphereTraceMulti(MyActor, Start, End, SphereRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}
				}
			}
			else if (!TraceByChannelOrObjects)
			{
				switch (MyKismetTraceType)
				{
				case ETraceType::LineTrace:
				{
					UKismetSystemLibrary::LineTraceMultiForObjects(MyActor, Start, End, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					AddHitToHitArray(OutHits); break;
				}

				case ETraceType::BoxTrace:
				{
					UKismetSystemLibrary::BoxTraceMultiForObjects(MyActor, Start, End, BoxHalfSize, BoxOrientation, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::CapsuleTrace:
				{
					UKismetSystemLibrary::CapsuleTraceMultiForObjects(MyActor, Start, End, CapsuleHalfHeight, CapsuleRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::SphereTrace:
				{
					UKismetSystemLibrary::SphereTraceMultiForObjects(MyActor, Start, End, SphereRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}
				}
			}
			AddHitToHitArray(OutHits);
		}
	}
}

void UTracingComponent::HitOtherSocketsAtDifferentTime()
{
	for (auto Socket1 : MySockets)
	{
		for (auto Socket2 : MySockets)
		{
			TArray<FHitResult> OutHits1;
			FVector Start = MyPrimitive->GetSocketLocation(Socket1);
			FVector* End = LastKnownSocketLocation.Find(Socket2);

			if (End == nullptr)
			{
				return;
			}

			if (TraceByChannelOrObjects)
			{
				switch (MyKismetTraceType)
				{
				case ETraceType::LineTrace:
				{
					UKismetSystemLibrary::LineTraceMulti(MyActor, Start, *End, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					AddHitToHitArray(OutHits1); break;
				}

				case ETraceType::BoxTrace:
				{
					UKismetSystemLibrary::BoxTraceMulti(MyActor, Start, *End, BoxHalfSize, BoxOrientation, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::CapsuleTrace:
				{
					UKismetSystemLibrary::CapsuleTraceMulti(MyActor, Start, *End, CapsuleHalfHeight, CapsuleRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::SphereTrace:
				{
					UKismetSystemLibrary::SphereTraceMulti(MyActor, Start, *End, SphereRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}
				}
			}
			else if (!TraceByChannelOrObjects)
			{
				switch (MyKismetTraceType)
				{
				case ETraceType::LineTrace:
				{
					UKismetSystemLibrary::LineTraceMultiForObjects(MyActor, Start, *End, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					AddHitToHitArray(OutHits1); break;
				}

				case ETraceType::BoxTrace:
				{
					UKismetSystemLibrary::BoxTraceMultiForObjects(MyActor, Start, *End, BoxHalfSize, BoxOrientation, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::CapsuleTrace:
				{
					UKismetSystemLibrary::CapsuleTraceMultiForObjects(MyActor, Start, *End, CapsuleHalfHeight, CapsuleRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::SphereTrace:
				{
					UKismetSystemLibrary::SphereTraceMultiForObjects(MyActor, Start, *End, SphereRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits1, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}
				}
			}

			AddHitToHitArray(OutHits1);
		}

		for (auto Socket3 : MySockets)
		{
			TArray<FHitResult> OutHits2;
			FVector* Start = LastKnownSocketLocation.Find(Socket3);
			FVector End = MyPrimitive->GetSocketLocation(Socket1);

			if (TraceByChannelOrObjects)
			{
				switch (MyKismetTraceType)
				{
				case ETraceType::LineTrace:
				{
					UKismetSystemLibrary::LineTraceMulti(MyActor, *Start, End, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					AddHitToHitArray(OutHits2); break;
				}

				case ETraceType::BoxTrace:
				{
					UKismetSystemLibrary::BoxTraceMulti(MyActor, *Start, End, BoxHalfSize, BoxOrientation, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::CapsuleTrace:
				{
					UKismetSystemLibrary::CapsuleTraceMulti(MyActor, *Start, End, CapsuleHalfHeight, CapsuleRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::SphereTrace:
				{
					UKismetSystemLibrary::SphereTraceMulti(MyActor, *Start, End, SphereRadius, MyTraceChannel, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}
				}
			}
			else if (!TraceByChannelOrObjects)
			{
				switch (MyKismetTraceType)
				{
				case ETraceType::LineTrace:
				{
					UKismetSystemLibrary::LineTraceMultiForObjects(MyActor, *Start, End, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					AddHitToHitArray(OutHits2); break;
				}

				case ETraceType::BoxTrace:
				{
					UKismetSystemLibrary::BoxTraceMultiForObjects(MyActor, *Start, End, BoxHalfSize, BoxOrientation, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::CapsuleTrace:
				{
					UKismetSystemLibrary::CapsuleTraceMultiForObjects(MyActor, *Start, End, CapsuleHalfHeight, CapsuleRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}

				case ETraceType::SphereTrace:
				{
					UKismetSystemLibrary::SphereTraceMultiForObjects(MyActor, *Start, End, SphereRadius, MyObjectTypesToHit, ShouldTraceComplex, MyActorsToIgnore, MyDrawDebugType, OutHits2, ShouldIgnoreSelf, MyTraceColor, MyTraceHitColor, MyDrawTime);
					break;
				}
				}
			}

			AddHitToHitArray(OutHits2);
		}
	}

}

void UTracingComponent::GetPrimaryComponent(AActor* ActorTarget)
{
	if (MyStaticMeshComponent)
	{
		MyPrimitive = MyStaticMeshComponent;
		return;
	}

	auto taggedComponents = ActorTarget->GetComponentsByTag(UStaticMeshComponent::StaticClass(), "DidItHit?");
	if (taggedComponents.Num() > 0)
	{
		MyPrimitive = Cast<UPrimitiveComponent>(taggedComponents[0]);
		return;
	}

	auto meshComponent = ActorTarget->GetComponentByClass(UStaticMeshComponent::StaticClass());
	if (meshComponent)
	{
		MyPrimitive = Cast<UPrimitiveComponent>(meshComponent);
		return;
	}
}

TArray<FName> UTracingComponent::SetupVariables(UPrimitiveComponent* MyPrimitiveTarget, AActor* MyActorTarget)
{
	TArray<FName> MyFoundSockets;
	MyPrimitive = MyPrimitiveTarget;
	MyActor = MyActorTarget;

	if (!MyActor)
	{
		MyActor = GetOwner();
	}

	GetSockets();
	MyFoundSockets = MySockets;
	return MyFoundSockets;
}

TArray<FName> UTracingComponent::GetSockets()
{
	if (MyPrimitive)
	{
		MySockets = MyPrimitive->GetAllSocketNames();

		
		if (SkipStringFilter != "")
		{
			for (int32 Index = MySockets.Num() - 1; Index >= 0; --Index)
			{
				if (MySockets[Index].ToString().Contains(SkipStringFilter))
				{
					MySockets.RemoveAt(Index);
				}
			}
		}

		if (InclusionStringFilter != "")
		{
			for (int32 Index = MySockets.Num() - 1; Index >= 0; --Index)
			{
				if (!MySockets[Index].ToString().Contains(InclusionStringFilter))
				{
					MySockets.RemoveAt(Index);
				}
			}
		}
	}

	return MySockets;
}

void UTracingComponent::ToggleTraceCheck(bool bTrace)
{
	if (bTrace)
	{
		ClearHitArray();
		ClearSocketLocationMap();
		GetSocket_t0();
		ModuloNumber = 0;
	}

	CanTrace = bTrace;
}

void UTracingComponent::UpdateLastSocketLocation()
{
	if (MySockets.Num() > 0) {

		for (auto Socket : MySockets)
		{
			LastKnownSocketLocation.Add(Socket, MyPrimitive->GetSocketLocation(Socket));
		}
	}
}

void UTracingComponent::ClearHitArray()
{
	HitArray.Empty();
}

void UTracingComponent::ClearSocketLocationMap()
{
	LastKnownSocketLocation.Empty();
}

void UTracingComponent::GetSocket_t0()
{
	for (auto Socket : MySockets)
	{
		LastKnownSocketLocation.Add(Socket, MyPrimitive->GetSocketLocation(Socket));
	}
}

void UTracingComponent::AddHitToHitArray(TArray<FHitResult> HitArrayToAdd)
{
	for (const auto& Hit : HitArrayToAdd)
	{
		if (!HitArray.ContainsByPredicate([&](const FHitResult& Inner) {  return Inner.GetActor() == Hit.GetActor(); }))
		{
			HitArray.Add(Hit);

			OnItemAdded.Broadcast(Hit);
		}
	}
}
