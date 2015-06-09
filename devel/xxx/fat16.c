//#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "fat16.h"



// ============== INTERNAL PROTOTYPES ==================

/** Read boot sector from given address */
void read_bs(const BLOCKDEV* dev, Fat16BootSector* info, const uint32_t addr);

/** Find absolute address of first BootSector. Returns 0 on failure. */
uint32_t find_bs(const BLOCKDEV* dev);

/** Get cluster's starting address */
uint32_t clu_addr(const FAT16* fat, const uint16_t cluster);

/** Find following cluster using FAT for jumps */
uint16_t next_clu(const FAT16* fat, uint16_t cluster);

/** Find relative address in a file, using FAT for cluster lookup */
uint32_t clu_offs(const FAT16* fat, uint16_t cluster, uint32_t addr);

/** Read a file entry from directory (dir starting cluster, entry number) */
void open_file(const FAT16* fat, FAT16_FILE* file, const uint16_t dir_cluster, const uint16_t num);

/** Allocate and chain new cluster to a chain starting at given cluster */
bool append_cluster(const FAT16* fat, const uint16_t clu);

/** Allocate a new cluster, clean it, and mark with 0xFFFF in FAT */
uint16_t alloc_cluster(const FAT16* fat);

/** Zero out entire cluster. */
void wipe_cluster(const FAT16* fat, const uint16_t clu);

/** Free cluster chain, starting at given number */
void free_cluster_chain(const FAT16* fat, uint16_t clu);

/**
 * Check if there is already a file of given RAW name
 * Raw name - name as found on disk, not "display name".
 */
bool dir_contains_file_raw(FAT16_FILE* dir, char* fname);

/** Write a value into FAT */
void write_fat(const FAT16* fat, const uint16_t cluster, const uint16_t value);

/** Read a value from FAT */
uint16_t read_fat(const FAT16* fat, const uint16_t cluster);

/** Go through a directory, and open first NONE or DELETED file. */
bool find_empty_file_slot(FAT16_FILE* file);


// =========== INTERNAL FUNCTION IMPLEMENTATIONS =========


uint16_t read16(const BLOCKDEV* dev)
{
	uint16_t a;
	dev->load(&a, 2);
	return a;
}


void write16(const BLOCKDEV* dev, const uint16_t val)
{
	dev->store(&val, 2);
}

/** Find absolute address of first boot sector. Returns 0 on failure. */
uint32_t find_bs(const BLOCKDEV* dev)
{
	//  Reference structure:
	//
	//  typedef struct __attribute__((packed)) {
	//      uint8_t first_byte;
	//      uint8_t start_chs[3];
	//      uint8_t partition_type;
	//      uint8_t end_chs[3];
	//      uint32_t start_sector;
	//      uint32_t length_sectors;
	//  } PartitionTable;

	uint16_t addr = 0x1BE + 4; // fourth byte of structure is the type.
	uint32_t tmp = 0;
	uint16_t tmp2;

	for (uint8_t i = 0; i < 4; i++, addr += 16)
	{
		// Read partition type
		dev->seek(addr);
		tmp = dev->read();

		// Check if type is valid
		if (tmp == 4 || tmp == 6 || tmp == 14)
		{
			// read MBR address
			dev->rseek(3);// skip 3 bytes
			dev->load(&tmp, 4);

			tmp = tmp << 9; // multiply address by 512 (sector size)

			// Verify that the boot sector has a valid signature mark
			dev->seek(tmp + 510);
			dev->load(&tmp2, 2);
			if (tmp2 != 0xAA55) continue; // continue to next entry

			// return absolute MBR address
			return tmp;
		}
	}

	return 0;
}


/** Read the boot sector */
void read_bs(const BLOCKDEV* dev, Fat16BootSector* info, const uint32_t addr)
{
	dev->seek(addr + 13); // skip 13

	dev->load(&(info->sectors_per_cluster), 6); // spc, rs, nf, re

	info->total_sectors = 0;
	dev->load(&(info->total_sectors), 2); // short sectors

	dev->rseek(1); // md

	dev->load(&(info->fat_size_sectors), 2);

	dev->rseek(8); // spt, noh, hs

	// read or skip long sectors field
	if (info->total_sectors == 0)
	{
		dev->load(&(info->total_sectors), 4);
	}
	else
	{
		dev->rseek(4); // tsl
	}

	dev->rseek(7); // dn, ch, bs, vi

	dev->load(&(info->volume_label), 11);
}


void write_fat(const FAT16* fat, const uint16_t cluster, const uint16_t value)
{
	fat->dev->seek(fat->fat_addr + (cluster * 2));
	write16(fat->dev, value);
}


uint16_t read_fat(const FAT16* fat, const uint16_t cluster)
{
	fat->dev->seek(fat->fat_addr + (cluster * 2));
	return read16(fat->dev);
}


/** Get cluster starting address */
uint32_t clu_addr(const FAT16* fat, const uint16_t cluster)
{
	if (cluster < 2) return fat->rd_addr;
	return fat->data_addr + (cluster - 2) * fat->bs.bytes_per_cluster;
}


uint16_t next_clu(const FAT16* fat, uint16_t cluster)
{
	return read_fat(fat, cluster);
}


/** Find file-relative address in fat table */
uint32_t clu_offs(const FAT16* fat, uint16_t cluster, uint32_t addr)
{
	while (addr >= fat->bs.bytes_per_cluster)
	{
		cluster = next_clu(fat, cluster);
		if (cluster == 0xFFFF) return 0xFFFF; // fail
		addr -= fat->bs.bytes_per_cluster;
	}

	return clu_addr(fat, cluster) + addr;
}


/**
 * Zero out entire cluster
 * This is important only for directory clusters, so we can
 * zero only every first byte of each file entry, to indicate
 * that it is unused (FT_NONE).
 */
void wipe_cluster(const FAT16* fat, const uint16_t clu)
{
	uint32_t addr = clu_addr(fat, clu);

	const BLOCKDEV* dev = fat->dev;

	dev->seek(addr);

	for (uint32_t b = 0; b < fat->bs.bytes_per_cluster; b += 32)
	{
		dev->write(0);
		dev->rseek(32);
	}
}


/** Allocate a new cluster, clean it, and mark with 0xFFFF in FAT */
uint16_t alloc_cluster(const FAT16* fat)
{
	// find new unclaimed cluster that can be added to the chain.
	uint16_t i, b;
	for (i = 2; i < fat->bs.fat_size_sectors * 256; i++)
	{
		// read value from FAT
		b = read_fat(fat, i);
		if (b == 0) // unused cluster
		{
			// Write FFFF to "i", to mark end of file
			write_fat(fat, i, 0xFFFF);

			// Wipe the cluster
			wipe_cluster(fat, i);

			return i;
		}
	}

	return 0xFFFF;//error code
}


/** Allocate and chain new cluster to a chain starting at given cluster */
bool append_cluster(const FAT16* fat, const uint16_t clu)
{
	uint16_t clu2 = alloc_cluster(fat);
	if (clu2 == 0xFFFF) return false;

	// Write "i" to "clu"
	write_fat(fat, clu, clu2);

	return true;
}


void free_cluster_chain(const FAT16* fat, uint16_t clu)
{
	do
	{
		// get address of the next cluster
		const uint16_t clu2 = read_fat(fat, clu);

		// mark cluster as unused
		write_fat(fat, clu, 0x0000);

		// advance
		clu = clu2;
	}
	while (clu != 0xFFFF);
}


/**
 * Check if there is already a file of given RAW name
 * Raw name - name as found on disk, not "display name".
 */
bool dir_contains_file_raw(FAT16_FILE* dir, char* fname)
{
	do
	{
		bool diff = false;
		for (uint8_t i = 0; i < 11; i++)
		{
			if (dir->name[i] != fname[i])
			{
				diff = true;
				break;
			}
		}

		if (!diff) return true;

	}
	while (fat16_next(dir));

	return false;
}


/**
 * Read a file entry
 *
 * dir_cluster ... directory start cluster
 * num ... entry number in the directory
 */
void open_file(const FAT16* fat, FAT16_FILE* file, const uint16_t dir_cluster, const uint16_t num)
{
	// Resolve starting address
	uint32_t addr;
	if (dir_cluster == 0)
	{
		addr = clu_addr(fat, dir_cluster) + num * 32; // root directory, max 512 entries.
	}
	else
	{
		addr = clu_offs(fat, dir_cluster, num * 32); // cluster + N (wrapping to next cluster if needed)
	}

	fat->dev->seek(addr);
	fat->dev->load(file, 12); // name, ext, attribs
	fat->dev->rseek(14); // skip 14 bytes
	fat->dev->load(((void*)file) + 12, 6); // read remaining bytes

	file->clu = dir_cluster;
	file->num = num;

	// Resolve filename & type

	file->type = FT_FILE;

	const uint8_t c = file->name[0];
	switch (c)
	{
		case 0x00:
			file->type = FT_NONE;
			return;

		case 0xE5:
			file->type = FT_DELETED;
			return;

		case 0x05: // Starting with 0xE5
			file->type = FT_FILE;
			file->name[0] = 0xE5; // convert to the real character
			break;

		case 0x2E:
			if (file->name[1] == 0x2E)
			{
				// ".." directory
				file->type = FT_PARENT;
			}
			else
			{
				// "." directory
				file->type = FT_SELF;
			}
			break;

		default:
			if (c < 32)
			{
				file->type = FT_INVALID; // File is corrupt, treat it as invalid
			}
			else
			{
				file->type = FT_FILE;
			}
	}

	// handle subdir, label
	if (file->attribs & FA_DIR && file->type == FT_FILE)
	{
		file->type = FT_SUBDIR;
	}
	else if (file->attribs == FA_LABEL)
	{
		file->type = FT_LABEL; // volume label special file
	}
	else if (file->attribs == 0x0F)
	{
		file->type = FT_LFN; // long name special file, can be ignored
	}

	// add a FAT pointer
	file->fat = fat;

	// Init cursors
	fat16_fseek(file, 0);
}


void delete_file_do(FAT16_FILE* file)
{
	const FAT16* fat = file->fat;

	// seek to file record
	fat->dev->seek(clu_offs(fat, file->clu, file->num * 32));

	// mark as deleted
	fat->dev->write(0xE5); // "deleted" mark

	// free allocated clusters
	free_cluster_chain(fat, file->clu_start);

	file->type = FT_DELETED;
}



/**
 * Write information into a file header.
 * "file" is an open handle.
 */
void write_file_header(FAT16_FILE* file, const char* fname_raw, const uint8_t attribs, const uint16_t clu_start)
{
	const BLOCKDEV* dev = file->fat->dev;

	const uint32_t entrystart = clu_offs(file->fat, file->clu, file->num * 32);

	// store the file name
	dev->seek(entrystart);
	dev->store(fname_raw, 11);

	// attributes
	dev->write(attribs);

	// 10 reserved, 2+2 date & time
	// (could just skip, but better to fill with zeros)
	for (uint8_t i = 0; i < 14; i++)
	{
		dev->write(0);
	}

	// addr of the first file cluster
	write16(dev, clu_start);

	// file size (uint32_t)
	write16(dev, 0);
	write16(dev, 0);

	// reopen file - load & parse the information just written
	open_file(file->fat, file, file->clu, file->num);
}


/** Go through a directory, and "open" first FT_NONE or FT_DELETED file entry. */
bool find_empty_file_slot(FAT16_FILE* file)
{
	const uint16_t clu = file->clu;
	const FAT16* fat = file->fat;

	// Find free directory entry that can be used
	for (uint16_t num = 0; num < 0xFFFF; num++)
	{
		// root directory has fewer entries, error if trying
		// to add one more.
		if (file->clu == 0 && num >= fat->bs.root_entries)
			return false;

		// Resolve addres of next file entry
		uint32_t addr;
		do
		{
			addr = clu_offs(fat, file->clu, num * 32);

			if (addr == 0xFFFF)
			{
				// end of chain of allocated clusters for the directory
				// append new cluster, return false on failure
				if (!append_cluster(fat, file->clu)) return false;
			}

			// if new cluster was just added, repeat.
		}
		while (addr == 0xFFFF);

		// Open the file entry
		open_file(fat, file, clu, num);

		// Check if can be overwritten
		if (file->type == FT_DELETED || file->type == FT_NONE)
		{
			return true;
		}
	}

	return false; // not found.
}


// =============== PUBLIC FUNCTION IMPLEMENTATIONS =================

/** Initialize a FAT16 handle */
void fat16_init(const BLOCKDEV* dev, FAT16* fat)
{
	const uint32_t bs_a = find_bs(dev);
	fat->dev = dev;
	read_bs(dev, &(fat->bs), bs_a);
	fat->fat_addr = bs_a + (fat->bs.reserved_sectors * 512);
	fat->rd_addr = bs_a + (fat->bs.reserved_sectors + fat->bs.fat_size_sectors * fat->bs.num_fats) * 512;
	fat->data_addr = fat->rd_addr + (fat->bs.root_entries * 32); // entry is 32B long

	fat->bs.bytes_per_cluster = (fat->bs.sectors_per_cluster * 512);
}


/**
 * Move file cursor to a position relative to file start
 * Allows seek past end of file, will allocate new cluster if needed.
 */
bool fat16_fseek(FAT16_FILE* file, uint32_t addr)
{
	const FAT16* fat = file->fat;

	// Store as rel
	file->cur_rel = addr;

	// Rewind and resolve abs, clu, ofs
	file->cur_clu = file->clu_start;

	while (addr >= fat->bs.bytes_per_cluster)
	{
		uint32_t next;

		// Go to next cluster, allocate if needed
		do
		{
			next = next_clu(fat, file->cur_clu);
			if (next == 0xFFFF)
			{
				// reached end of allocated space
				// add one more cluster
				if (!append_cluster(fat, file->cur_clu))
				{
					return false;
				}
			}
		}
		while (next == 0xFFFF);

		file->cur_clu = next;
		addr -= fat->bs.bytes_per_cluster;
	}

	file->cur_abs = clu_addr(fat, file->cur_clu) + addr;
	file->cur_ofs = addr;

	// Physically seek to that location
	fat->dev->seek(file->cur_abs);

	return true;
}


/**
 * Check if file is a valid entry (to be shown)
 */
bool fat16_is_file_valid(const FAT16_FILE* file)
{
	switch (file->type)
	{
		case FT_FILE:
		case FT_SUBDIR:
		case FT_SELF:
		case FT_PARENT:
			return true;

		default:
			return false;
	}
}


#define MIN(a, b) (((a) < (b)) ? (a) : (b))

bool fat16_fread(FAT16_FILE* file, void* target, uint32_t len)
{
	if (file->cur_abs == 0xFFFF)
		return false; // file at the end already

	if (file->cur_rel + len > file->size)
		return false; // Attempt to read more than what is available

	const FAT16* fat = file->fat;
	const BLOCKDEV* dev = fat->dev;

	while (len > 0 && file->cur_rel < file->size)
	{
		// How much can be read from the cluster
		uint16_t chunk = MIN(file->size - file->cur_rel, MIN(fat->bs.bytes_per_cluster - file->cur_ofs, len));

		// read the chunk
		dev->seek(file->cur_abs);
		dev->load(target, chunk);

		// move the cursors
		file->cur_abs += chunk;
		file->cur_rel += chunk;
		file->cur_ofs += chunk;

		// move target pointer
		target += chunk;

		// reached end of cluster?
		if (file->cur_ofs >= fat->bs.bytes_per_cluster)
		{
			file->cur_clu = next_clu(fat, file->cur_clu);
			file->cur_abs = clu_addr(fat, file->cur_clu);
			file->cur_ofs = 0;
		}

		// subtract read length
		len -= chunk;
	}

	return true;
}


bool fat16_fwrite(FAT16_FILE* file, void* source, uint32_t len)
{
	const FAT16* fat = file->fat;
	const BLOCKDEV* dev = fat->dev;


	if (file->cur_abs == 0xFFFF)
		return false; // file at the end already

	// Attempt to write past end of file
	if (file->cur_rel + len >= file->size)
	{
		const uint32_t pos_start = file->cur_rel;

		// Seek to the last position
		// -> fseek will allocate clusters
		if (!fat16_fseek(file, pos_start + len))
			return false; // error in seek

		// Write starts beyond EOF - creating a zero-filled "hole"
		if (file->cur_rel > file->size)
		{
			// Seek to the end of valid data
			fat16_fseek(file, file->size);

			// fill space between EOF and start-of-write with zeros
			uint32_t fill = pos_start - file->size;

			// repeat until all "fill" zeros are stored
			while (fill > 0)
			{
				// How much will fit into this cluster
				const uint16_t chunk = MIN(fat->bs.bytes_per_cluster - file->cur_ofs, fill);

				// write the zeros
				dev->seek(file->cur_abs);
				for (uint16_t i = 0; i < chunk; i++)
				{
					dev->write(0);
				}

				// subtract from "needed" what was just placed
				fill -= chunk;

				// advance cursors to the next cluster
				file->cur_clu = next_clu(fat, file->cur_clu);
				file->cur_abs = clu_addr(fat, file->cur_clu);
				file->cur_ofs = 0;
			}
		}

		// Save new size
		fat16_resize(file, pos_start + len);

		// Seek back to where it was before
		fat16_fseek(file, pos_start);
	} // (end zerofill)


	// write the data
	while (len > 0)
	{
		// How much can be stored in this cluster
		const uint16_t chunk = MIN(fat->bs.bytes_per_cluster - file->cur_ofs, len);

		// store the chunk
		dev->seek(file->cur_abs);
		dev->store(source, chunk);

		// advance cursors
		file->cur_abs += chunk;
		file->cur_rel += chunk;
		file->cur_ofs += chunk;

		// Pointer arith!
		source += chunk; // advance the source pointer

		// detect cluster overflow
		if (file->cur_ofs >= fat->bs.bytes_per_cluster)
		{
			// advance to following cluster
			file->cur_clu = next_clu(fat, file->cur_clu);
			file->cur_abs = clu_addr(fat, file->cur_clu);
			file->cur_ofs = 0;
		}

		// subtract written length
		len -= chunk;
	}

	return true;
}



/** Open next file in the directory */
bool fat16_next(FAT16_FILE* file)
{
	const FAT16* fat = file->fat;
	const BLOCKDEV* dev = fat->dev;

	if (file->clu == 0 && file->num >= fat->bs.root_entries)
		return false; // attempt to read outside root directory.

	const uint32_t addr = clu_offs(fat, file->clu, (file->num + 1) * 32);
	if (addr == 0xFFFF)
		return false; // next file is out of the directory cluster

	// read first byte of the file entry
	dev->seek(addr);
	if (dev->read() == 0)
		return false; // can't read (file is NONE)

	open_file(fat, file, file->clu, file->num + 1);

	return true;
}


/** Open previous file in the directory */
bool fat16_prev(FAT16_FILE* file)
{
	if (file->num == 0)
		return false; // first file already

	open_file(file->fat, file, file->clu, file->num - 1);

	return true;
}


/** Rewind to first file in directory */
void fat16_first(FAT16_FILE* file)
{
	open_file(file->fat, file, file->clu, 0);
}


/** Open a directory denoted by the file. */
bool fat16_opendir(FAT16_FILE* file)
{
	// Don't open non-dirs and "." directory.
	if (!(file->attribs & FA_DIR) || file->type == FT_SELF)
		return false;

	open_file(file->fat, file, file->clu_start, 0);
	return true;
}


void fat16_root(const FAT16* fat, FAT16_FILE* file)
{
	open_file(fat, file, 0, 0);
}


/**
 * Find a file with given "display name" in this directory.
 * If file is found, "dir" will contain it's handle.
 * Either way, "dir" gets modified and you may need to rewind it afterwards.
 */
bool fat16_find(FAT16_FILE* dir, const char* name)
{
	char fname[11];
	fat16_rawname(name, fname);
	return dir_contains_file_raw(dir, fname);
}

bool fat16_mkfile(FAT16_FILE* file, const char* name)
{
	// Convert filename to zero padded raw string
	char fname[11];
	fat16_rawname(name, fname);

	// Abort if file already exists
	bool exists = dir_contains_file_raw(file, fname);
	fat16_first(file); // rewind dir
	if (exists)
		return false; // file already exists in the dir.


	if (!find_empty_file_slot(file))
		return false; // error finding a slot

	// Write into the new slot
	const uint16_t newclu = alloc_cluster(file->fat);
	write_file_header(file, fname, 0, newclu);

	return true;
}


/**
 * Create a sub-directory of given name.
 * Directory is allocated and populated with entries "." and ".."
 */
bool fat16_mkdir(FAT16_FILE* file, const char* name)
{
	// Convert filename to zero padded raw string
	char fname[11];
	fat16_rawname(name, fname);

	// Abort if file already exists
	bool exists = dir_contains_file_raw(file, fname);
	fat16_first(file); // rewind dir
	if (exists)
		return false; // file already exusts in the dir.

	if (!find_empty_file_slot(file))
		return false; // error finding a slot


	// Write into the new slot
	const uint16_t newclu = alloc_cluster(file->fat);
	write_file_header(file, fname, FA_DIR, newclu);

	const uint32_t parent_clu = file->clu;
	open_file(file->fat, file, file->clu_start, 0);

	write_file_header(file, ".          ", FA_DIR, newclu);

	// Advance to next file slot
	find_empty_file_slot(file);

	write_file_header(file, "..         ", FA_DIR, parent_clu);

	// rewind.
	fat16_first(file);

	return true;
}


char* fat16_disk_label(const FAT16* fat, char* label_out)
{
	FAT16_FILE first;
	fat16_root(fat, &first);

	if (first.type == FT_LABEL)
	{
		return fat16_dispname(&first, label_out);
	}

	// find where spaces end
	int8_t j = 10;
	for (; j >= 0; j--)
	{
		if (fat->bs.volume_label[j] != ' ') break;
	}

	// copy all until spaces
	uint8_t i;
	for (i = 0; i <= j; i++)
	{
		label_out[i] = fat->bs.volume_label[i];
	}

	label_out[i] = 0; // ender

	return label_out;
}


char* fat16_dispname(const FAT16_FILE* file, char* disp_out)
{
	// Cannot get name for special files
	if (file->type == FT_NONE ||        // not-yet-used directory location
			file->type == FT_DELETED ||     // deleted file entry
			file->attribs == 0x0F)          // long name special entry (system, hidden)
		return NULL;

	// find first non-space
	int8_t j = 7;
	for (; j >= 0; j--)
	{
		if (file->name[j] != ' ') break;
	}

	// j ... last no-space char

	uint8_t i;
	for (i = 0; i <= j; i++)
	{
		disp_out[i] = file->name[i];
	}


	// directory entry, no extension
	if (file->type == FT_SUBDIR || file->type == FT_SELF || file->type == FT_PARENT)
	{
		disp_out[i] = 0; // end of string
		return disp_out;
	}


	// add a dot
	if (file->type != FT_LABEL) // volume label has no dot!
		disp_out[i++] = '.';

	// Add extension chars
	for (j = 8; j < 11; j++, i++)
	{
		const char c = file->name[j];
		if (c == ' ') break;
		disp_out[i] = c;
	}

	disp_out[i] = 0; // end of string

	return disp_out;
}


char* fat16_rawname(const char* disp_in, char* raw_out)
{
	uint8_t name_c = 0, wr_c = 0;
	bool filling = false;
	bool at_ext = false;
	for (; wr_c < 11; wr_c++)
	{
		// start filling with spaces if end of filename reached
		uint8_t c = disp_in[name_c];
		// handle special rule for 0xE5
		if (name_c == 0 && c == 0xE5)
		{
			c = 0x05;
		}

		if (c == '.' || c == 0)
		{
			if (!filling)
			{
				filling = true;

				if (c == '.')
				{
					name_c++; // skip the dot
					c = disp_in[name_c];
					at_ext = true;
				}
			}
		}

		// if at the start of ext
		if (wr_c == 8)
		{
			if (!at_ext)
			{
				// try to advance past dot (if any)
				while (true)
				{
					c = disp_in[name_c++];
					if (c == 0) break;
					if (c == '.')
					{
						// read char PAST the dot
						c = disp_in[name_c];
						at_ext = true;
						break;
					}
				}
			}

			// if c has valid char for extension
			if (c != 0 && c != '.')
			{
				// start copying again.
				filling = false;
			}
		}

		if (!filling)
		{
			// copy char of filename
			raw_out[wr_c] = disp_in[name_c++];
		}
		else
		{
			// add a filler space
			raw_out[wr_c] = ' ';
		}
	}

	return raw_out;
}


/** Write new file size (also to the disk). Does not allocate clusters. */
void fat16_resize(FAT16_FILE* file, uint32_t size)
{
	const FAT16* fat = file->fat;
	const BLOCKDEV* dev = file->fat->dev;

	// Find address for storing the size
	const uint32_t addr = clu_offs(fat, file->clu, file->num * 32 + 28);
	file->size = size;

	dev->seek(addr);
	dev->store(&size, 4);

	// Seek to the end of the file, to make sure clusters are allocated
	fat16_fseek(file, size - 1);

	const uint16_t next = next_clu(fat, file->cur_clu);
	if (next != 0xFFFF)
	{
		free_cluster_chain(fat, next);

		// Mark that there's no further clusters
		write_fat(fat, file->cur_clu, 0xFFFF);
	}
}

/** Delete a simple file */
bool fat16_rmfile(FAT16_FILE* file)
{
	if (file->type != FT_FILE)
		return false; // not a simple file

	delete_file_do(file);
	return true;
}


/** Delete an empty directory */
bool fat16_rmdir(FAT16_FILE* file)
{
	if (file->type != FT_SUBDIR)
		return false; // not a subdirectory entry

	const FAT16* fat = file->fat;

	const uint16_t dir_clu = file->clu;
	const uint16_t dir_num = file->num;

	// Look for valid files and subdirs in the directory
	if (!fat16_opendir(file))
		return false; // could not open

	uint8_t cnt = 0;
	do
	{
		// Stop on apparent corrupt structure (missing "." or "..")
		// Can safely delete the folder.
		if (cnt == 0 && file->type != FT_SELF) break;
		if (cnt == 1 && file->type != FT_PARENT) break;

		// Found valid file
		if (file->type == FT_SUBDIR || file->type == FT_FILE)
		{
			// Valid child file was found, aborting.
			// reopen original file
			open_file(fat, file, dir_clu, dir_num);
			return false;
		}

		if (cnt < 2) cnt++;
	}
	while (fat16_next(file));

	// reopen original file
	open_file(fat, file, dir_clu, dir_num);

	// and delete as ordinary file
	delete_file_do(file);

	return true;
}


bool fat16_parent(FAT16_FILE* file)
{
	const uint16_t clu = file->clu;
	const uint16_t num = file->num;

	// open second entry of the directory
	open_file(file->fat, file, file->clu, 1);

	// if it's a valid PARENT link, follow it.
	if (file->type == FT_PARENT)
	{
		open_file(file->fat, file, file->clu_start, 0);
		return true;
	}
	else
	{
		// maybe in root already?

		// reopen original file
		open_file(file->fat, file, clu, num);
		return false;
	}
}
