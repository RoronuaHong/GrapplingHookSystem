// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GHAnimInstance.generated.h"

class UAnimNotify;
class AGHCharacter;
/**
 * 
 */
UCLASS()
class GRAPPLINGHOOKSYSTEM_API UGHAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	AActor* OwningActor;
	AGHCharacter* MyCharacter;

	void BlueprintBeginPlay();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "MyAnim")
	void AnimNotify_ThrowRope(UAnimNotify* Notify);

public:
	UGHAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool IsInAir;
};