// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 2018-2020 by Jaime "Lactozilla" Passos.
// Copyright (C) 2019-2020 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  r_patch.h
/// \brief Patch generation.

#ifndef __R_PATCH__
#define __R_PATCH__

#include "r_defs.h"
#include "w_wad.h"
#include "m_aatree.h"
#include "i_video.h"
#include "doomdef.h"

void *Patch_CacheSoftware(UINT16 wad, UINT16 lump, INT32 tag, boolean store);
void *Patch_CacheGL(UINT16 wad, UINT16 lump, INT32 tag, boolean store);

patchtreetype_t Patch_GetTreeType(rendermode_t mode);
patchtree_t *Patch_GetRendererTree(UINT16 wadnum, rendermode_t mode);
aatree_t *Patch_GetRendererBaseSubTree(UINT16 wadnum, rendermode_t mode);
#ifdef ROTSPRITE
aatree_t *Patch_GetRendererRotatedSubTree(UINT16 wadnum, rendermode_t mode, boolean flip);
#endif

void Patch_UpdateReferences(void);
void Patch_FreeReferences(void);

void Patch_InitInfo(wadfile_t *wadfile);

// Structs
struct patchreference_s
{
	struct patchreference_s *prev, *next;
	UINT16 wad, lump;
	INT32 tag;
	boolean flip;
	INT32 rollangle;
	void *ptr;
};
typedef struct patchreference_s patchreference_t;

typedef enum
{
	ROTAXIS_X, // Roll (the default)
	ROTAXIS_Y, // Pitch
	ROTAXIS_Z  // Yaw
} rotaxis_t;

typedef struct
{
	INT32 x, y;
	rotaxis_t rotaxis;
} spriteframepivot_t;

typedef struct
{
	spriteframepivot_t pivot[64]; // Max number of frames in a sprite.
	boolean available;
} spriteinfo_t;

boolean PixelMap_ApplyToColumn(pixelmap_t *pmap, INT32 *map, patch_t *patch, UINT8 *post, size_t *colsize, boolean flipped);
UINT8 *Patch_GetPixel(patch_t *patch, INT32 x, INT32 y, boolean flip);

// Conversions between patches / flats / textures...
boolean R_CheckIfPatch(lumpnum_t lump);
void R_TextureToFlat(size_t tex, UINT8 *flat);
void R_PatchToFlat(patch_t *patch, UINT8 *flat);
patch_t *R_FlatToPatch(UINT8 *raw, UINT16 width, UINT16 height, UINT16 leftoffset, UINT16 topoffset, size_t *destsize, boolean transparency);
patch_t *R_TransparentFlatToPatch(UINT16 *raw, UINT16 width, UINT16 height, UINT16 leftoffset, UINT16 topoffset, size_t *destsize);

// PNGs
boolean R_IsLumpPNG(const UINT8 *d, size_t s);
#define W_ThrowPNGError(lumpname, wadfilename) I_Error("W_Wad: Lump \"%s\" in file \"%s\" is a .png - please convert to either Doom or Flat (raw) image format.", lumpname, wadfilename); // Fears Of LJ Sonic

#ifndef NO_PNG_LUMPS
UINT8 *R_PNGToFlat(UINT16 *width, UINT16 *height, UINT8 *png, size_t size);
patch_t *R_PNGToPatch(const UINT8 *png, size_t size, size_t *destsize);
boolean R_PNGDimensions(UINT8 *png, INT16 *width, INT16 *height, size_t size);
#endif

// SpriteInfo
extern spriteinfo_t spriteinfo[NUMSPRITES];
void R_LoadSpriteInfoLumps(UINT16 wadnum, UINT16 numlumps);
void R_ParseSPRTINFOLump(UINT16 wadNum, UINT16 lumpNum);

// Sprite rotation
#ifdef ROTSPRITE
INT32 R_GetRollAngle(angle_t rollangle);

// Arguments for RotSprite_ functions.
typedef struct
{
	INT32 rollangle;
	boolean flip;
	boolean sprite;
	spriteframepivot_t *pivot;
} rotsprite_vars_t;

typedef struct
{
	pixelmap_t pixelmap[ROTANGLES];
	patch_t *patches[ROTANGLES];
	boolean cached[ROTANGLES];

	UINT32 lumpnum;
	rotsprite_vars_t vars;
	INT32 tag;
} rotsprite_t;

rotsprite_t *RotSprite_GetFromPatchNumPwad(UINT16 wad, UINT16 lump, INT32 tag, rotsprite_vars_t rsvars, boolean store);
rotsprite_t *RotSprite_GetFromPatchNum(lumpnum_t lumpnum, INT32 tag, rotsprite_vars_t rsvars, boolean store);
rotsprite_t *RotSprite_GetFromPatchName(const char *name, INT32 tag, rotsprite_vars_t rsvars, boolean store);
rotsprite_t *RotSprite_GetFromPatchLongName(const char *name, INT32 tag, rotsprite_vars_t rsvars, boolean store);

void RotSprite_Create(rotsprite_t *rotsprite, rotsprite_vars_t rsvars);
void RotSprite_CreateColumns(pixelmap_t *pixelmap, pmcache_t *cache, patch_t *patch, rotsprite_vars_t rsvars);
void RotSprite_CreatePixelMap(patch_t *patch, pixelmap_t *pixelmap, rotsprite_vars_t rsvars);
patch_t *RotSprite_CreatePatch(rotsprite_t *rotsprite, rotsprite_vars_t rsvars);

patch_t *Patch_CacheRotated(UINT32 lumpnum, INT32 tag, rotsprite_vars_t rsvars, boolean store);
patch_t *Patch_CacheRotatedForSprite(UINT32 lumpnum, INT32 tag, rotsprite_vars_t rsvars, boolean store);
patch_t *Patch_CacheRotatedPwad(UINT16 wad, UINT16 lump, INT32 tag, rotsprite_vars_t rsvars, boolean store);
patch_t *Patch_CacheRotatedForSpritePwad(UINT16 wad, UINT16 lump, INT32 tag, rotsprite_vars_t rsvars, boolean store);
patch_t *Patch_CacheRotatedName(const char *name, INT32 tag, rotsprite_vars_t rsvars, boolean store);
patch_t *Patch_CacheRotatedLongName(const char *name, INT32 tag, rotsprite_vars_t rsvars, boolean store);

void RotSprite_InitPatchTree(patchtree_t *rcache);
void RotSprite_Recreate(rotsprite_t *rotsprite, rendermode_t rmode);
void RotSprite_RecreateAll(void);

int RotSprite_GetCurrentPatchInfoIdx(INT32 rollangle, boolean flip);

extern fixed_t rollcosang[ROTANGLES];
extern fixed_t rollsinang[ROTANGLES];
#endif

#endif // __R_PATCH__
