#ifndef MV_LOAD_H
#define MV_LOAD_H

#include <stdio.h>
#include "std.h"

inline int MV_ID(int a, int b, int c, int d)
{
	return (a) | (b << 8) | (c << 16) | (d << 24);
}

struct MV_RGBA
{
	unsigned char r, g, b, a;
};

struct MV_Voxel
{
	unsigned char x, y, z, color_index;
};

struct chunk_t
{
	int id;
	int content_size;
	int children_size;
	long end;
};

struct MV_Model
{
	int size_x, size_y, size_z;

	// voxels
	int num_voxels;
	MV_Voxel *voxels;

	// palette
	bool is_custom_palette;
	MV_RGBA palette[256];

	int version;
};

inline void MV_FreeModel(MV_Model *model)
{
	
}

inline int ReadInt(FILE *fp)
{
	int v = 0; 
	fread(&v, 4, 1, fp);
	return v;
}

inline void ReadChunk(FILE *fp, chunk_t &chunk)
{
	chunk.id = ReadInt(fp);
	chunk.content_size = ReadInt(fp);
	chunk.children_size = ReadInt(fp);

	chunk.end = ftell(fp) + chunk.content_size + chunk.children_size;

	const char *c = (const char *)(&chunk.id);
	SDL_Log("MV_VoxelModel :: Chunk : %c%c%c%c : %d %d\n",
		c[0], c[1], c[2], c[3], chunk.content_size, chunk.children_size);
}

inline bool ReadModelFile(FILE *fp, MV_Model *model)
{
	const int MV_VERSION = 150;
	const int ID_VOX = MV_ID('V', 'O', 'X', ' ');
	const int ID_MAIN = MV_ID('M', 'A', 'I', 'N');
	const int ID_SIZE = MV_ID('S', 'I', 'Z', 'E');
	const int ID_XYZI = MV_ID('X', 'Y', 'Z', 'I');
	const int ID_RGBA = MV_ID('R', 'G', 'B', 'A');

	// magic number
	int magic = ReadInt(fp);
	if (magic != ID_VOX)
	{
		Error("Magic number does not match");
		return false;
	}

	// version number
	int version = ReadInt(fp);
	if (version != MV_VERSION)
	{
		Error("Version does not match");
		return false;
	}

	// main chunk
	chunk_t main_chunk;
	ReadChunk(fp, main_chunk);
	if (main_chunk.id != ID_MAIN)
	{
		Error("Main chunk is not found");
		return false;
	}

	fseek(fp, main_chunk.content_size, SEEK_CUR);

	// read children chunks
	while (ftell(fp) < main_chunk.end)
	{
		// read chunk header
		chunk_t sub;
		ReadChunk(fp, sub);

		if (sub.id == ID_SIZE)
		{
			model->size_x = ReadInt(fp);
			model->size_y = ReadInt(fp);
			model->size_z = ReadInt(fp);
		}
		else if (sub.id == ID_XYZI)
		{
			model->num_voxels = ReadInt(fp);
			if (model->num_voxels < 0)
			{
				Error("Negative number of voxels");
				return false;
			}

			if (model->num_voxels > 0)
			{
				model->voxels = new MV_Voxel[model->num_voxels];
				fread(model->voxels, sizeof(MV_Voxel), model->num_voxels, fp);
			}
		}
		else if (sub.id == ID_RGBA)
		{
			model->is_custom_palette = true;
			fread(model->palette + 1, sizeof(MV_RGBA), 255, fp);

			MV_RGBA reserved;
			fread(&reserved, sizeof(MV_RGBA), 1, fp);
			
		}
		fseek(fp, sub.end, SEEK_SET);
	}
	SDL_Log("MV_VoxelModel :: Model : %d %d %d : %d\n",
		model->size_x, model->size_y, model->size_z, model->num_voxels);
	return true;
}

inline MV_Model LoadModel(const char *path)
{
	MV_Model model = {};
	FILE *fp = fopen(path, "rb");
	if (!fp)
	{
		Error("Failed to open file");
	}

	bool success = ReadModelFile(fp, &model);

	fclose(fp);
	return model;
}

#endif // MV_LOAD_H