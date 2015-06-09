#pragma once

/** Abstract block device interface */
typedef struct
{
	// Sequential read
	void (*load)(void* dest, const uint16_t len);
	// Sequential write
	void (*store)(const void* src, const uint16_t len);
	// Sequential byte write
	void (*write)(const uint8_t b);
	// Sequential byte read
	uint8_t (*read)(void);
	// Absolute seek
	void (*seek)(const uint32_t);
	// Relative seek
	void (*rseek)(const int16_t);
} BLOCKDEV;


// -------------------------------

/** file types (values don't matter) */
typedef enum
{
	FT_NONE = '-',
	FT_DELETED = 'x',
	FT_SUBDIR = 'D',
	FT_PARENT = 'P',
	FT_LABEL = 'L',
	FT_LFN = '~',
	FT_INVALID = 'i', // not recognized weird file
	FT_SELF = '.',
	FT_FILE = 'F'
} FAT16_FT;


// File Attributes (bit flags)
#define FA_READONLY 0x01 // read only file
#define FA_HIDDEN   0x02 // hidden file
#define FA_SYSTEM   0x04 // system file
#define FA_LABEL    0x08 // volume label entry, found only in root directory.
#define FA_DIR      0x10 // subdirectory
#define FA_ARCHIVE  0x20 // archive flag


/** Boot Sector structure - INTERNAL! */
typedef struct __attribute__((packed))
{

	// Fields loaded directly from disk:

	// 13 bytes skipped
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t num_fats;
	uint16_t root_entries;
	// 3 bytes skipped
	uint16_t fat_size_sectors;
	// 8 bytes skipped
	uint32_t total_sectors; // if "short size sectors" is used, it's copied here too
	// 7 bytes skipped
	char volume_label[11]; // space padded, no terminator

	// Added fields:

	uint32_t bytes_per_cluster;

}
Fat16BootSector;


/** FAT filesystem handle - private fields! */
typedef struct __attribute__((packed))
{
	// Backing block device
	const BLOCKDEV* dev;

	// Root directory sector start
	uint32_t rd_addr;

	// Start of first cluster (number "2")
	uint32_t data_addr;

	// Start of fat table
	uint32_t fat_addr;

	// Boot sector data struct
	Fat16BootSector bs;
}
FAT16;


/** File handle struct */
typedef struct __attribute__((packed))
{
	// Fields loaded directly from disk:

	uint8_t name[11]; // Starting 0x05 converted to 0xE5, other "magic chars" left intact
	uint8_t attribs; // composed of FA_* constants
	// 14 bytes skipped
	uint16_t clu_start;
	uint32_t size;

	// Added fields:

	FAT16_FT type;

	// --- Private fields ---

	// Cursor
	uint32_t cur_abs; // absolute position in device
	uint32_t cur_rel; // relative position in file
	uint16_t cur_clu; // cluster where the cursor is
	uint16_t cur_ofs; // offset within the active cluster

	// File position in the directory
	uint16_t clu; // first cluster of directory
	uint16_t num; // fiel entry number

	// pointer to FAT
	const FAT16* fat;
}
FAT16_FILE;


/** Initialize a filesystem */
void fat16_init(const BLOCKDEV* dev, FAT16* fat);


/**
 * Open the first file of the root directory.
 * The file may be invalid (eg. a volume label, deleted etc),
 * or blank (type FT_NONE) if the filesystem is empty.
 *
 * Either way, the prev and next functions will work as expected.
 */
void fat16_root(const FAT16* fat, FAT16_FILE* file);


/**
 * Resolve the disk label.
 */
char* fat16_disk_label(const FAT16* fat, char* label_out);


// ----------- FILE I/O -------------


/**
 * Move file cursor to a position relative to file start
 * Returns false on I/O error (bad file, out of range...)
 */
bool fat16_fseek(FAT16_FILE* file, uint32_t addr);


/**
 * Read bytes from file into memory
 * Returns false on I/O error (bad file, out of range...)
 */
bool fat16_fread(FAT16_FILE* file, void* target, uint32_t len);


/**
 * Write into file at a "seek" position.
 * "seek" cursor must be within (0..filesize)
 */
bool fat16_fwrite(FAT16_FILE* file, void* source, uint32_t len);


/**
 * Create a new file in given folder
 *
 * file ... open directory; new file is opened into this handle.
 * name ... name of the new file, including extension
 */
bool fat16_mkfile(FAT16_FILE* file, const char* name);


/**
 * Create a sub-directory of given name.
 * Directory is allocated and populated with entries "." and ".."
 */
bool fat16_mkdir(FAT16_FILE* file, const char* name);


/**
 * Write new file size (also to the disk).
 * Allocates / frees needed clusters, does not erase them.
 */
void fat16_resize(FAT16_FILE* file, uint32_t size);


/**
 * Delete a file entry and free clusters.
 * Does NOT work on folders.
 */
bool fat16_rmfile(FAT16_FILE* file);


/**
 * Delete an empty directory.
 * Calling with non-subfolder entry or non-empty directory will cause error.
 */
bool fat16_rmdir(FAT16_FILE* file);

// --------- NAVIGATION ------------


/** Go to previous file in the directory (false = no prev file) */
bool fat16_prev(FAT16_FILE* file);


/** Go to next file in directory (false = no next file) */
bool fat16_next(FAT16_FILE* file);


/**
 * Open a subdirectory denoted by the file.
 * Provided handle changes to first entry of the directory.
 */
bool fat16_opendir(FAT16_FILE* file);

/**
 * Open a parent directory. Fails in root.
 */
bool fat16_parent(FAT16_FILE* file);


/** Rewind to first file in directory */
void fat16_first(FAT16_FILE* file);


/**
 * Find a file with given "display name" in this directory.
 * If file is found, "dir" will contain it's handle.
 * Either way, "dir" gets modified and you may need to rewind it afterwards.
 */
bool fat16_find(FAT16_FILE* dir, const char* name);


// -------- FILE INSPECTION -----------

/** Check if file is a valid entry, or long-name/label/deleted */
bool fat16_is_file_valid(const FAT16_FILE* file);


/**
 * Resolve a file name, trim spaces and add null terminator.
 * Returns the passed char*, or NULL on error.
 */
char* fat16_dispname(const FAT16_FILE* file, char* disp_out);


/**
 * Convert filename to zero-padded fixed length one
 * Returns the passed char*.
 */
char* fat16_rawname(const char* disp_in, char* raw_out);

