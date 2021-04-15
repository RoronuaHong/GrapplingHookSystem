// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/GHAnimInstance.h"
#include "Character/GHCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGHAnimInstance::UGHAnimInstance() {
	Speed = 0.0f;
	IsInAir = false;
}

void UGHAnimInstance::BlueprintBeginPlay() {
	Super::BlueprintBeginPlay();
}

void UGHAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	OwningActor = GetOwningActor();

	if(OwningActor) {
		Speed = OwningActor->GetVelocity().Size();

		MyCharacter = Cast<AGHCharacter>(OwningActor);

		if(MyCharacter) {
			IsInAir = MyCharacter->GetCharacterMovement()->IsFalling();
		}
	}
}

void UGHAnimInstance::AnimNotify_ThrowRope(UAnimNotify* Notify)
{
	OwningActor = GetOwningActor();

	if(OwningActor) {
		MyCharacter = Cast<AGHCharacter>(OwningActor);

		if(MyCharacter) {
			MyCharacter->PlayGrappleWave();
		}
	}
}

void UGHAnimInstance::AnimNotify_StartMovement(UAnimNotify * Notify) {
	OwningActor=GetOwningActor();

	if(OwningActor) {
		MyCharacter=Cast<AGHCharacter>(OwningActor);

		if(MyCharacter) {
			MyCharacter->StartGrapplingMovement();
		}
	}
}

void UGHAnimInstance::AnimNotify_RopeAppears(UAnimNotify * Notify) {
	OwningActor=GetOwningActor();

	if(OwningActor) {
		MyCharacter=Cast<AGHCharacter>(OwningActor);

		if(MyCharacter) {
			MyCharacter->RopeVisibility(true);
		}
	}
}

void UGHAnimInstance::AnimNotify_RopeDissapear(UAnimNotify * Notify) {
	OwningActor=GetOwningActor();

	if(OwningActor) {
		MyCharacter=Cast<AGHCharacter>(OwningActor);

		if(MyCharacter) {
			MyCharacter->RopeVisibility(false);
		}
	}
}

void UGHAnimInstance::AnimNotify_AnimEnd(UAnimNotify * Notify) {
	OwningActor=GetOwningActor();

	if(OwningActor) {
		MyCharacter=Cast<AGHCharacter>(OwningActor);

		if(MyCharacter) {
			MyCharacter->ResetMovement();
		}
	}
}

