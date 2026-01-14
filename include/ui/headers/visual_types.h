#ifndef UI_VISUAL_TYPES_H
#define UI_VISUAL_TYPES_H



// Outline effect info
// Confidence: 40%
// Error count: 516
// Character outline glow effect (PvP highlighting, etc.)
struct OutLineEFFECTInfoTemp {
    int effectId;       // Effect template ID
    int colorR;         // Red component (0-255)
    int colorG;         // Green component (0-255)
    int colorB;         // Blue component (0-255)
    int thickness;      // Outline thickness in pixels (1-5)
    int alpha;          // Transparency (0-255)
    /* Used for team highlighting in PvP modes */
};

// Avatar variations
// Confidence: 45%
// Error count: 324
// Dye/color variation for avatar items
struct AvatarVariation {
    int avatarId;       // Base avatar item ID
    int variationId;    // Variation index (0-15 typically)
    int colorScheme;    // Color scheme ID (maps to palette)
    int dyeItemId;      // Dye item used (0 if none)
    /* Avatar dye system - allows color customization */
};

// Avatar hidden options
// Confidence: 40%
// Error count: 12
// Hidden stats/bonuses on avatar items
struct AvatarFixedHiddenOptionServer {
    int avatarId;       // Avatar item ID
    int hiddenOptionId; // Option type (0=str, 1=int, 2=vit, 3=spr)
    int optionValue;    // Bonus value (+1 to +15 typical)
    int revealLevel;    // Level required to reveal option
    /* Hidden options revealed after certain conditions met */
};

#endif // UI_VISUAL_TYPES_H
