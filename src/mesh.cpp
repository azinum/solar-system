// mesh.cpp
// wavefront object loader

#include <string.h>	// strcmp

#include "common.hpp"
#include "memory.hpp"
#include "mesh.hpp"

#define MAX_LINE_SIZE 256

#define safe_scanf(ScanStatus, Iterator, Format, ...) { \
	u32 num_bytes_read = 0; \
	ScanStatus = sscanf(Iterator, Format "%n", __VA_ARGS__, &num_bytes_read); \
	Iterator += num_bytes_read; \
}

static void mesh_initialize(Mesh* mesh);

void mesh_initialize(Mesh* mesh) {
#if 1
	memset(mesh, 0, sizeof(Mesh));
#else
	mesh->vertices = NULL;
	mesh->vertex_count = 0;
	mesh->vertex_indices = NULL;
	mesh->vertex_index_count = 0;
	mesh->uv = NULL;
	mesh->uv_count = 0;
	mesh->uv_indices = NULL;
	mesh->uv_index_count = 0;
	mesh->normals = NULL;
	mesh->normal_count = 0;
	mesh->normal_indices = NULL;
	mesh->normal_index_count = 0;
#endif
}

i32 mesh_sort_indices(Mesh* mesh) {
	v2* uv = (v2*)m_malloc(sizeof(v2) * mesh->vertex_index_count);
	u32 uv_count = mesh->vertex_index_count;

	v3* normals = (v3*)m_malloc(sizeof(v3) * mesh->vertex_index_count);
	u32 normal_count = mesh->vertex_index_count;

	for (u32 i = 0; i < mesh->vertex_index_count; ++i) {
		u32 index = mesh->vertex_indices[i];
		u32 uv_index = mesh->uv_indices[i];
		u32 normal_index = mesh->normal_indices[i];

		v2 current_uv = mesh->uv[uv_index];
		uv[index] = current_uv;
		v3 current_normal = mesh->normals[normal_index];
		normals[index] = current_normal;
	}

	list_free(mesh->uv, mesh->uv_count);
	list_free(mesh->normals, mesh->normal_count);

	mesh->uv = uv;
	mesh->uv_count = uv_count;
	mesh->normals = normals;
	mesh->normal_count = normal_count;
}

i32 load_mesh(const char* path, Mesh* mesh, u8 sort_mesh) {
	i32 result = NoError;
	mesh_initialize(mesh);
	Buffer buffer = {0};	// Buffer to store the wavefront object contents in
	if (read_file(path, &buffer) != NoError) {
		return Error;
	}
	char line[MAX_LINE_SIZE] = {0};	// Current line we are reading
	char* iterator = &buffer.data[0];	// Iterator pointing at the beginning of the obj data file
	i32 scan_status = 0;	// Scan status to recognize when we can not read the file anymore (i.e. end of file)

	// printf("Parsing object file: %s, size: %i\n", path, buffer.size);
	while (1) {
		safe_scanf(scan_status, iterator, "%s\n", line);
		if (scan_status == EOF) {
			break;
		}
		if (!strncmp(line, "v", MAX_LINE_SIZE)) {
			v3 vertex;
			safe_scanf(scan_status, iterator, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			list_push(mesh->vertices, mesh->vertex_count, vertex);
		}
		else if (!strncmp(line, "vt", MAX_LINE_SIZE)) {
			v2 uv;
			safe_scanf(scan_status, iterator, "%f %f\n", &uv.x, &uv.y);
			list_push(mesh->uv, mesh->uv_count, uv);
		}
		else if (!strncmp(line, "vn", MAX_LINE_SIZE)) {
			v3 normal;
			safe_scanf(scan_status, iterator, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			list_push(mesh->normals, mesh->normal_count, normal);
		}
		else if (!strncmp(line, "f", MAX_LINE_SIZE)) {
			u32 x[3] = {0};	// vertex indices
			u32 y[3] = {0};	// uv indices
			u32 z[3] = {0}; // normal indices
			safe_scanf(scan_status, iterator,
				"%i/%i/%i %i/%i/%i %i/%i/%i",
				&x[0], &y[0], &z[0],
				&x[1], &y[1], &z[1],
				&x[2], &y[2], &z[2]
			);
			if (scan_status != 9) {
				fprintf(stderr, "Failed to parse wavefront object file '%s'\n", path);
				unload_mesh(mesh);
				result = Error;
				goto done;
			}
			list_push(mesh->vertex_indices, mesh->vertex_index_count, x[0] - 1);
			list_push(mesh->vertex_indices, mesh->vertex_index_count, x[1] - 1);
			list_push(mesh->vertex_indices, mesh->vertex_index_count, x[2] - 1);

			list_push(mesh->uv_indices, mesh->uv_index_count, y[0] - 1);
			list_push(mesh->uv_indices, mesh->uv_index_count, y[1] - 1);
			list_push(mesh->uv_indices, mesh->uv_index_count, y[2] - 1);

			list_push(mesh->normal_indices, mesh->normal_index_count, y[0] - 1);
			list_push(mesh->normal_indices, mesh->normal_index_count, y[1] - 1);
			list_push(mesh->normal_indices, mesh->normal_index_count, y[2] - 1);
		}
	}
	if (sort_mesh) {
		mesh_sort_indices(mesh);
	}
done:
	buffer_free(&buffer);	// The buffer data is parsed and loaded into the mesh data structure, therefore it is not needed anymore
	return result;
}

void unload_mesh(Mesh* mesh) {
	list_free(mesh->vertices, mesh->vertex_count);
	list_free(mesh->vertex_indices, mesh->vertex_index_count);
	list_free(mesh->uv, mesh->uv_count);
	list_free(mesh->uv_indices, mesh->uv_index_count);
	list_free(mesh->normals, mesh->normal_count);
	list_free(mesh->normal_indices, mesh->normal_index_count);
}