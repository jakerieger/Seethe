// Copyright (C) 2026 Jake Rieger


#include "InventoryWidget.h"

#include "../Inventory/InventoryItem.h"
#include "../Inventory/InventoryComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridSlot.h"

void UInventoryWidget::UpdateLookAxes(const float X, const float Y) {
    LookX = X;
    LookY = Y;
}

int32 UInventoryWidget::GetInventoryIndex() const {
    if (!LinkedInventory) { return -1; }

    const int32 NumItems = Items.Num();

    if (NumItems <= 0) {
        return -1;
    }

    // 1. Get Angle (-180 to 180)
    float LookDegrees = FMath::RadiansToDegrees(FMath::Atan2(LookY, LookX));

    // 2. Offset by -90 so Index 0 starts at Top (North)
    LookDegrees -= 90.f;

    // 3. Normalize to 0-360 range
    // FMath::Fmod with negative numbers can be tricky, 
    // so this is the safest way to wrap it:
    while (LookDegrees < 0.f) {
        LookDegrees += 360.f;
    }
    while (LookDegrees >= 360.f) {
        LookDegrees -= 360.f;
    }

    // 4. Calculate Index
    const float DegreesPerSlot = 360.f / static_cast<float>(NumItems);
    const int32 SelectedIndex  = FMath::FloorToInt(LookDegrees / DegreesPerSlot);

    // Final safety clamp
    return FMath::Clamp(SelectedIndex, 0, NumItems - 1);
}

UInventoryComponent* UInventoryWidget::GetInventoryComponent() {
    return LinkedInventory;
}

TArray<FInventorySlot> UInventoryWidget::GetInventoryItems() {
    return Items;
}

void UInventoryWidget::OnInventoryUpdate() {
    if (!LinkedInventory || !SlotWidgetClass) { return; }
    Items.Reset();
    Items = LinkedInventory->InventoryItems;

    ItemContainer->ClearChildren();

    constexpr int32 kColumns = 3;

    for (int32 i = 0; i < Items.Num(); i++) {
        UInventorySlotWidget* NewSlot = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
        if (NewSlot) {
            NewSlot->SetupSlot(Items[i], i);
            UUniformGridSlot* GridSlot = ItemContainer->AddChildToUniformGrid(NewSlot);
            if (GridSlot) {
                const int32 Row = i / kColumns;
                const int32 Col = i % kColumns;
                GridSlot->SetRow(Row);
                GridSlot->SetColumn(Col);
            }
        }
    }
}

void UInventoryWidget::InitializeWidget(UInventoryComponent* InInventory) {
    if (InInventory) {
        LinkedInventory = InInventory;
        LinkedInventory->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::OnInventoryUpdate);
    }
}

void UInventoryWidget::PositionSlotInCircle(const UInventorySlotWidget* InSlot,
                                            const int32 Index,
                                            const int32 Total) const {
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(InSlot->Slot)) {
        // Calculate the angle (Offset by -90 to start at the top)
        const float Angle   = (360.f / Total) * Index - 90.f;
        const float Radians = FMath::DegreesToRadians(Angle);

        // Calculate position based on a radius variable
        const float Radius = ItemContainer->GetDesiredSize().X / 2.f;
        const float PosX   = Radius * FMath::Cos(Radians);
        const float PosY   = Radius * FMath::Sin(Radians);

        CanvasSlot->SetPosition(FVector2D(PosX, PosY));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f)); // Center it
        CanvasSlot->SetAutoSize(true);
    }
}