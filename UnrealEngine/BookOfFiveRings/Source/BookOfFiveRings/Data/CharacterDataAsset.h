#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScrollDataAsset.h"
#include "CharacterDataAsset.generated.h"

// ── Sword move type ───────────────────────────────────────────────────────────
UENUM(BlueprintType)
enum class EMoveType : uint8
{
    HorizontalSlash  UMETA(DisplayName = "Horizontal Slash"),
    VerticalSlash    UMETA(DisplayName = "Vertical Slash"),
    DiagonalSlash    UMETA(DisplayName = "Diagonal Slash"),
    Thrust           UMETA(DisplayName = "Thrust"),
    Combo            UMETA(DisplayName = "5-Hit Combo"),
    PowerStrike      UMETA(DisplayName = "Power Strike"),
    AerialAssault    UMETA(DisplayName = "Aerial Assault"),
    EnergyBlade      UMETA(DisplayName = "Energy Blade")
};

// ── Individual move descriptor ────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct BOOKOFFIVERINGS_API FSwordplayMove
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString MoveName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
    FString Description;          // Which scroll principle this move demonstrates

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor EffectColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Duration = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EMoveType MoveType = EMoveType::HorizontalSlash;
};

/**
 * UCharacterDataAsset
 * One instance per fighter (Broly / Superman-Kal-El / Vegeta / Goku).
 * Mirrors Unity's CharacterData ScriptableObject.
 */
UCLASS(BlueprintType, Blueprintable)
class BOOKOFFIVERINGS_API UCharacterDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // ── Identity ──────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FString CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FString Origin;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    EScrollType AssociatedScroll = EScrollType::Earth;

    // ── Visual ────────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    FLinearColor BodyColor  = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    FLinearColor AuraColor  = FLinearColor::Blue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    FLinearColor HairColor  = FLinearColor::Black;

    // ── Combat Stats (0–10) ───────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.0", ClampMax = "10.0"))
    float PowerLevel = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.0", ClampMax = "10.0"))
    float Speed = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.0", ClampMax = "10.0"))
    float Technique = 5.f;

    // ── Philosophy ────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Philosophy")
    FString StyleDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Philosophy", meta = (MultiLine = true))
    FString PhilosophyStatement;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Philosophy")
    FString BattleCry;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Philosophy")
    FLinearColor BattleCryColor = FLinearColor::Yellow;

    // ── Swordplay ─────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swordplay")
    TArray<FSwordplayMove> SignatureMoves;

    // UPrimaryDataAsset interface
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId("CharacterData", GetFName());
    }
};
