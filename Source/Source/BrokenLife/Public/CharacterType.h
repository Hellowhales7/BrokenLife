#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayeName = "Unequipped"),
	ESC_EquippedOneHandedWeapon  UMETA(DisplayeName = "Equipped One-Handed Weapon"),
	ESC_EquippedTwoHandedWeapon  UMETA(DisplayeName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayeName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayeName = "HitReaction"),
	EAS_Attacking  UMETA(DisplayeName = "Attacking"),
	EAS_EquippingWeapon  UMETA(DisplayeName = "Equipping Weapon"),
	EAS_Dodge  UMETA(DisplayeName = "Dodge"),
	EAS_Dead  UMETA(DisplayeName = "Dead")

};

UENUM(BlueprintType)
enum  EDeathPose
{
	EDP_Death1  UMETA(DisplayeName = "Death1"),
	EDP_Death2  UMETA(DisplayeName = "Death2"),
	EDP_Death3  UMETA(DisplayeName = "Death3"),
	EDP_Death4  UMETA(DisplayeName = "Death4"),
	EDP_Death5  UMETA(DisplayeName = "Death5"),
	
	EDP_MAX  UMETA(DisplayeName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayeName = "NoState"),
	EES_Dead UMETA(DisplayeName = "Dead"),
	EES_Patrolling UMETA(DisplayeName = "Patrolling"),
	EES_Chasing UMETA(DisplayeName = "Chasing"),
	EES_Attacking UMETA(DisplayeName = "Attacking"),
	EES_Engaged UMETA(DisplayeName = "Engaged")
};