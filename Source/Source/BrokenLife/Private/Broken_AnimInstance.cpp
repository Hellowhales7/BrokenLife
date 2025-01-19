// Fill out your copyright notice in the Description page of Project Settings.


#include "Broken_AnimInstance.h"
#include "../BrockenLifeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBroken_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BrokenCharacter = Cast<ABrockenLifeCharacter>(TryGetPawnOwner());
	if (BrokenCharacter)
	{
		BrokenCharacterMovement = BrokenCharacter->GetCharacterMovement();
	}
}

void UBroken_AnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BrokenCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(BrokenCharacterMovement->Velocity);
		IsFalling = BrokenCharacterMovement->IsFalling();
		CharacterState = BrokenCharacter->GetCharacterState();
		ActionState = BrokenCharacter->GetActionState();
		DeathPose = BrokenCharacter->GetDeathPose();
	}
}
