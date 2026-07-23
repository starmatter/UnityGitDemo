#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScrollDataAsset.generated.h"

UENUM(BlueprintType)
enum class EScrollType : uint8
{
    Earth  UMETA(DisplayName = "Earth  - Chi  (地)"),
    Water  UMETA(DisplayName = "Water  - Sui  (水)"),
    Fire   UMETA(DisplayName = "Fire   - Ka   (火)"),
    Wind   UMETA(DisplayName = "Wind   - Fu   (風)"),
    Void   UMETA(DisplayName = "Void   - Ku   (空)")
};

/**
 * UScrollDataAsset
 * One instance per scroll (Earth/Water/Fire/Wind/Void).
 * Mirrors Unity's ScrollData ScriptableObject.
 * Create via: Content Browser → Right-click → Miscellaneous → Data Asset → ScrollDataAsset
 */
UCLASS(BlueprintType, Blueprintable)
class BOOKOFFIVERINGS_API UScrollDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // ── Identity ─────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    EScrollType ScrollType = EScrollType::Earth;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FString ScrollName = TEXT("Earth");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FString JapaneseCharacter = TEXT("地\nChi");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
    FString AssociatedCharacterName;

    // ── Visual Theme ──────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Theme")
    FLinearColor PrimaryColor   = FLinearColor(0.38f, 0.22f, 0.08f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Theme")
    FLinearColor SecondaryColor = FLinearColor(0.18f, 0.10f, 0.03f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Theme")
    FLinearColor TextColor      = FLinearColor(0.92f, 0.82f, 0.60f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Theme")
    FLinearColor AccentColor    = FLinearColor(0.55f, 0.85f, 0.25f, 1.f);

    // ── Narration ─────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content", meta = (MultiLine = true))
    FString OpeningNarration;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content", meta = (MultiLine = true))
    FString MusashiQuote;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
    TArray<FString> Principles;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content", meta = (MultiLine = true))
    FString CombatPhilosophy;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
    TArray<FString> KeyTechniques;

    // ── Timing ────────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
    float DisplayDuration = 11.f;

    // UPrimaryDataAsset interface
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId("ScrollData", GetFName());
    }
};
