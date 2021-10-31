#include <stdio.h>
#include <assert.h>

#define MEMORY_SIZE 64000
#define MAX_REGIONS 128

/*
 * Memory
 */

int memory[MEMORY_SIZE];

/*
 * State of the memory (meta)
 */

typedef struct {
	size_t start;
	size_t size;
} Region;

size_t first_free_region = 0;
Region memory_regions[MAX_REGIONS];


/*
 * Calculate last writable address for region
 */
size_t region_last_address(Region r) {
	return (size_t) r.start + r.size;
}

/*
 * Allocates size number of bytes on the memory and returns a pointer to the
 * first byte of the allocated memory.
 */
void *gg_alloc(size_t size) {
	assert(first_free_region < MAX_REGIONS);

	size_t new_region = first_free_region;

	// Find a usable gap between regions
	for (size_t i = 1; i < first_free_region; i++) {
		size_t end = region_last_address(memory_regions[i - 1]);
		size_t start = memory_regions[i].start;

		if (start - end > size) {
			printf("Allocating region in the middle\n");
			new_region = i;
			break;
		}
	}

	if (new_region != first_free_region) {
		// Shift all regions to keep order
		for (size_t i = MAX_REGIONS - 1; i > new_region; i--) {
			memory_regions[i] = memory_regions[i - 1];
		}
	}

	// No usable inset region found, allocating at the end
	size_t first_free_byte = 0;

	if (new_region > 0) {
		first_free_byte = region_last_address(memory_regions[new_region - 1]);
	}

	assert(first_free_byte + size < MEMORY_SIZE);

	memory_regions[new_region] = (Region) {
		.start = first_free_byte,
		.size = size
	};

	first_free_region++;

	return memory + first_free_byte;
}

/*
 * Frees the memory region starting at ptr, returns 0 if successful, 1
 * otherwise.
 */
void gg_free(void *ptr) {
	// First memory region
	size_t shift;
	for (shift = 0; shift < MAX_REGIONS; shift++) {
		if (memory + memory_regions[shift].start == ptr) {
			break;
		}
	}

	assert(shift != MAX_REGIONS);

	// Shift all values
	for (size_t i = shift + 1; i < MAX_REGIONS; i++) {
		memory_regions[i - 1] = memory_regions[i];
	}

	first_free_region--;
}

/*
 * Debug
 */
void memory_info() {
	printf("Memory base address: %p\n", (void *) memory);
	printf("Allocated regions: %ld\n", first_free_region);

	for (size_t i = 0; i < first_free_region; i++) {
		printf("\t%ld) start: %d  size: %ld\n", i + 1, (int) memory_regions[i].start, memory_regions[i].size);
	}

	printf("\n");
}

int main() {

	const int A = 26;
	char *t[A];

	for (int i = 0; i < A; i++) {
		t[i] = (char *) gg_alloc(5);
	}

	for (int i = 0; i < A; i++) {
		*t[i] = 'A' + i;
		printf("%p => %c\n", t[i], t[i][0]);
	}

	gg_free(t[10]);
	gg_free(t[15]);

	t[10] = gg_alloc(4);
	t[15] = gg_alloc(4);

	memory_info();

	return 0;
}
