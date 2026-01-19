# DNF Resource System

## Status: Verified Working (2026-01-04)

NPK/IMG parsing verified against Taiwan DNF client resources.

## Components

| Component | File | Status |
|-----------|------|--------|
| NPK Parser | `npk_parser.c` | ✅ Working |
| IMG Decoder | `img_decoder.c` | ✅ Working |
| Resource Manager | `resource_manager.c` | Ready |
| ANI Loader | `ani_loader.c` | Ready |
| Cache Manager | `cache_manager.c` | Ready |

## Key Technical Details

### NPK Format
- Magic: `NeoplePack_Bill` (16 bytes)
- File count: uint32 at offset 16
- Entry: 264 bytes (4 offset + 4 size + 256 encrypted_name)
- **Offset is ABSOLUTE** (verified via testing)
- Decrypt key: `puchikon@neople dungeon and fighter DNF` + `DNF` padding (256 bytes)

### IMG Format (Interleaved)
```
Header (32 bytes):
  [0-17]  Magic: "Neople Img File"
  [24-27] Version
  [28-31] Image count

Frame Data (INTERLEAVED):
  For each frame:
    Entry (36 bytes):
      [0-3]   Color format (14=ARGB1555, 15=ARGB4444, 16=ARGB8888, 0=link)
      [4-7]   Compress type (6=zlib, 5=raw, 0=link)
      [8-11]  Width
      [12-15] Height
      [16-19] Compressed data size
      [20-23] Key X (offset)
      [24-27] Key Y (offset)
      [28-31] Max Width
      [32-35] Max Height
    Data (size bytes): Immediately follows entry
```

## Test Results

```
Testing: sprite_character.NPK (7 files)
  ✅ sprite/character/atgunner.img - 18 frames decoded
  ✅ sprite/character/defaultfaces.img - 6 frames decoded
  ✅ sprite/character/fighter.img - 12 frames decoded

Testing: sprite_monster.NPK (3 files)
  ✅ sprite/monster/flyingmonstermark.img - 2 frames
  ✅ sprite/monster/monsterface.img - 510 frames
  ✅ sprite/monster/summonmonstermark.img - 2 frames

Testing: sprite_interface.NPK (154 files)
  ✅ (tn)characterillust.img - 5 frames (309x480 up to 511x480)
  ✅ (tn)npc_dialog_face.img - 57 frames
  ✅ additionalavatarinventoryimage.img - 5 frames

Summary: 39 frames decoded, 0 failures
```

## Usage

```c
#include "resource/npk_parser.h"
#include "resource/img_decoder.h"

// Initialize
NPK_Initialize();

// Open NPK
NPKArchive archive;
NPK_Open("sprite_character.NPK", &archive);

// Find and read IMG
NPKFileEntry* entry = NPK_FindFile(&archive, "sprite/character/fighter.img");
uint8_t* buffer = malloc(entry->size);
NPK_ReadFile(&archive, entry, buffer, entry->size);

// Parse IMG
IMGFile img;
IMG_Parse(buffer, entry->size, &img);

// Decode frame
IMGPixels pixels;
IMG_DecodeFrame(&img, 0, &pixels);
// pixels.data = RGBA8888 pixel array

// Cleanup
IMG_FreePixels(&pixels);
IMG_Free(&img);
NPK_Close(&archive);
```

## Build

Resource module is included in client Makefile:
```makefile
SOURCES += \
    $(SRC_DIR)/resource/npk_parser.c \
    $(SRC_DIR)/resource/img_decoder.c \
    $(SRC_DIR)/resource/resource_manager.c \
    $(SRC_DIR)/resource/ani_loader.c \
    $(SRC_DIR)/resource/cache_manager.c

LIBS += -lz  # zlib for decompression
```

## References

- Taiwan DNF client: `C:\Users\waw\Desktop\地下城与勇士台服\imagepacks2\`
- SkillTreeViewer (C# reference): `NpkService.cs`, `ImgService.cs`
- pvfUtility: NPK decrypt key discovery
