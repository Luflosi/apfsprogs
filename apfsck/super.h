/*
 *  apfsprogs/apfsck/super.h
 *
 * Copyright (C) 2019 Ernesto A. Fernández <ernesto.mnd.fernandez@gmail.com>
 */

#ifndef _SUPER_H
#define _SUPER_H

#include "types.h"
#include "object.h"

union htable_entry;

/*
 * Structure used to store a range of physical blocks
 */
struct apfs_prange {
	__le64 pr_start_paddr;
	__le64 pr_block_count;
} __packed;

/* EFI constants */
#define APFS_NX_EFI_JUMPSTART_MAGIC	0x5244534A
#define APFS_NX_EFI_JUMPSTART_VERSION	1

/*
 * Information about the embedded EFI driver
 */
struct apfs_nx_efi_jumpstart {
	struct apfs_obj_phys	nej_o;
	__le32			nej_magic;
	__le32			nej_version;
	__le32			nej_efi_file_len;
	__le32			nej_num_extents;
	__le64			nej_reserved[16];
	struct apfs_prange	nej_rec_extents[];
};

/* Main container */

/* Container constants */
#define APFS_NX_MAGIC				0x4253584E
#define APFS_NX_BLOCK_NUM			0
#define APFS_NX_MAX_FILE_SYSTEMS		100

#define APFS_NX_EPH_INFO_COUNT			4
#define APFS_NX_EPH_MIN_BLOCK_COUNT		8
#define APFS_NX_MAX_FILE_SYSTEM_EPH_STRUCTS	4
#define APFS_NX_TX_MIN_CHECKPOINT_COUNT		4
#define APFS_NX_EPH_INFO_VERSION_1		1

/* Container flags */
#define APFS_NX_RESERVED_1			0x00000001LL
#define APFS_NX_RESERVED_2			0x00000002LL
#define APFS_NX_CRYPTO_SW			0x00000004LL
#define APFS_NX_FLAGS_VALID_MASK		(APFS_NX_RESERVED_1 \
						| APFS_NX_RESERVED_2 \
						| APFS_NX_CRYPTO_SW)

/* Optional container feature flags */
#define APFS_NX_FEATURE_DEFRAG			0x0000000000000001ULL
#define APFS_NX_FEATURE_LCFD			0x0000000000000002ULL
#define APFS_NX_SUPPORTED_FEATURES_MASK		(APFS_NX_FEATURE_DEFRAG | \
						APFS_NX_FEATURE_LCFD)

/* Read-only compatible container feature flags */
#define APFS_NX_SUPPORTED_ROCOMPAT_MASK		(0x0ULL)

/* Incompatible container feature flags */
#define APFS_NX_INCOMPAT_VERSION1		0x0000000000000001ULL
#define APFS_NX_INCOMPAT_VERSION2		0x0000000000000002ULL
#define APFS_NX_INCOMPAT_FUSION			0x0000000000000100ULL
#define APFS_NX_SUPPORTED_INCOMPAT_MASK		(APFS_NX_INCOMPAT_VERSION2 \
						| APFS_NX_INCOMPAT_FUSION)

/* Block and container sizes */
#define APFS_NX_MINIMUM_BLOCK_SIZE		4096
#define APFS_NX_DEFAULT_BLOCK_SIZE		4096
#define APFS_NX_MAXIMUM_BLOCK_SIZE		65536
#define APFS_NX_MINIMUM_CONTAINER_SIZE		1048576

/* Indexes into a container superblock's array of counters */
enum {
	APFS_NX_CNTR_OBJ_CKSUM_SET	= 0,
	APFS_NX_CNTR_OBJ_CKSUM_FAIL	= 1,

	APFS_NX_NUM_COUNTERS		= 32
};

/*
 * On-disk representation of the container superblock
 */
struct apfs_nx_superblock {
/*00*/	struct apfs_obj_phys nx_o;
/*20*/	__le32 nx_magic;
	__le32 nx_block_size;
	__le64 nx_block_count;

/*30*/	__le64 nx_features;
	__le64 nx_readonly_compatible_features;
	__le64 nx_incompatible_features;

/*48*/	char nx_uuid[16];

/*58*/	__le64 nx_next_oid;
	__le64 nx_next_xid;

/*68*/	__le32 nx_xp_desc_blocks;
	__le32 nx_xp_data_blocks;
/*70*/	__le64 nx_xp_desc_base;
	__le64 nx_xp_data_base;
	__le32 nx_xp_desc_next;
	__le32 nx_xp_data_next;
/*88*/	__le32 nx_xp_desc_index;
	__le32 nx_xp_desc_len;
	__le32 nx_xp_data_index;
	__le32 nx_xp_data_len;

/*98*/	__le64 nx_spaceman_oid;
	__le64 nx_omap_oid;
	__le64 nx_reaper_oid;

/*B0*/	__le32 nx_test_type;

	__le32 nx_max_file_systems;
/*B8*/	__le64 nx_fs_oid[APFS_NX_MAX_FILE_SYSTEMS];
/*3D8*/	__le64 nx_counters[APFS_NX_NUM_COUNTERS];
/*4D8*/	struct apfs_prange nx_blocked_out_prange;
	__le64 nx_evict_mapping_tree_oid;
/*4F0*/	__le64 nx_flags;
	__le64 nx_efi_jumpstart;
/*500*/	char nx_fusion_uuid[16];
	struct apfs_prange nx_keylocker;
/*520*/	__le64 nx_ephemeral_info[APFS_NX_EPH_INFO_COUNT];

/*540*/	__le64 nx_test_oid;

	__le64 nx_fusion_mt_oid;
/*550*/	__le64 nx_fusion_wbc_oid;
	struct apfs_prange nx_fusion_wbc;
} __packed;

/*
 * A mapping from an ephemeral object id to its physical address
 */
struct apfs_checkpoint_mapping {
	__le32 cpm_type;
	__le32 cpm_subtype;
	__le32 cpm_size;
	__le32 cpm_pad;
	__le64 cpm_fs_oid;
	__le64 cpm_oid;
	__le64 cpm_paddr;
} __packed;

/* Checkpoint flags */
#define	APFS_CHECKPOINT_MAP_LAST	0x00000001

/*
 * A checkpoint-mapping block
 */
struct apfs_checkpoint_map_phys {
	struct apfs_obj_phys		cpm_o;
	__le32				cpm_flags;
	__le32				cpm_count;
	struct apfs_checkpoint_mapping	cpm_map[];
} __packed;

/* Volume */

/* Volume constants */
#define APFS_MAGIC				0x42535041

#define APFS_MAX_HIST				8
#define APFS_VOLNAME_LEN			256

/* Volume flags */
#define APFS_FS_UNENCRYPTED			0x00000001LL
#define APFS_FS_EFFACEABLE			0x00000002LL
#define APFS_FS_RESERVED_4			0x00000004LL
#define APFS_FS_ONEKEY				0x00000008LL
#define APFS_FS_SPILLEDOVER			0x00000010LL
#define APFS_FS_RUN_SPILLOVER_CLEANER		0x00000020LL
#define APFS_FS_ALWAYS_CHECK_EXTENTREF		0x00000040LL
#define APFS_FS_FLAGS_VALID_MASK		(APFS_FS_UNENCRYPTED \
					       | APFS_FS_EFFACEABLE \
					       | APFS_FS_RESERVED_4 \
					       | APFS_FS_ONEKEY \
					       | APFS_FS_SPILLEDOVER \
					       | APFS_FS_RUN_SPILLOVER_CLEANER \
					       | APFS_FS_ALWAYS_CHECK_EXTENTREF)

#define APFS_FS_CRYPTOFLAGS			(APFS_FS_UNENCRYPTED \
						| APFS_FS_EFFACEABLE \
						| APFS_FS_ONEKEY)

/* Optional volume feature flags */
#define APFS_FEATURE_DEFRAG_PRERELEASE		0x00000001LL
#define APFS_FEATURE_HARDLINK_MAP_RECORDS	0x00000002LL
#define APFS_FEATURE_DEFRAG			0x00000004LL

#define APFS_SUPPORTED_FEATURES_MASK	(APFS_FEATURE_DEFRAG \
					| APFS_FEATURE_DEFRAG_PRERELEASE \
					| APFS_FEATURE_HARDLINK_MAP_RECORDS)

/* Read-only compatible volume feature flags */
#define APFS_SUPPORTED_ROCOMPAT_MASK		(0x0ULL)

/* Incompatible volume feature flags */
#define APFS_INCOMPAT_CASE_INSENSITIVE		0x00000001LL
#define APFS_INCOMPAT_DATALESS_SNAPS		0x00000002LL
#define APFS_INCOMPAT_ENC_ROLLED		0x00000004LL
#define APFS_INCOMPAT_NORMALIZATION_INSENSITIVE	0x00000008LL

#define APFS_SUPPORTED_INCOMPAT_MASK  (APFS_INCOMPAT_CASE_INSENSITIVE \
				      | APFS_INCOMPAT_DATALESS_SNAPS \
				      | APFS_INCOMPAT_ENC_ROLLED \
				      | APFS_INCOMPAT_NORMALIZATION_INSENSITIVE)

#define APFS_MODIFIED_NAMELEN	      32

/*
 * Structure containing information about a program that modified the volume
 */
struct apfs_modified_by {
	u8 id[APFS_MODIFIED_NAMELEN];
	__le64 timestamp;
	__le64 last_xid;
} __packed;

/*
 * Structure used to store the encryption state
 */
struct apfs_wrapped_meta_crypto_state {
	__le16 major_version;
	__le16 minor_version;
	__le32 cpflags;
	__le32 persistent_class;
	__le32 key_os_version;
	__le16 key_revision;
	__le16 unused;
} __packed;

/*
 * On-disk representation of a volume superblock
 */
struct apfs_superblock {
/*00*/	struct apfs_obj_phys apfs_o;

/*20*/	__le32 apfs_magic;
	__le32 apfs_fs_index;

/*28*/	__le64 apfs_features;
	__le64 apfs_readonly_compatible_features;
	__le64 apfs_incompatible_features;

/*40*/	__le64 apfs_unmount_time;

	__le64 apfs_fs_reserve_block_count;
	__le64 apfs_fs_quota_block_count;
	__le64 apfs_fs_alloc_count;

/*60*/	struct apfs_wrapped_meta_crypto_state apfs_meta_crypto;

/*74*/	__le32 apfs_root_tree_type;
	__le32 apfs_extentref_tree_type;
	__le32 apfs_snap_meta_tree_type;

/*80*/	__le64 apfs_omap_oid;
	__le64 apfs_root_tree_oid;
	__le64 apfs_extentref_tree_oid;
	__le64 apfs_snap_meta_tree_oid;

/*A0*/	__le64 apfs_revert_to_xid;
	__le64 apfs_revert_to_sblock_oid;

/*B0*/	__le64 apfs_next_obj_id;

/*B8*/	__le64 apfs_num_files;
	__le64 apfs_num_directories;
	__le64 apfs_num_symlinks;
	__le64 apfs_num_other_fsobjects;
	__le64 apfs_num_snapshots;

/*E0*/	__le64 apfs_total_blocks_alloced;
	__le64 apfs_total_blocks_freed;

/*F0*/	char apfs_vol_uuid[16];
/*100*/	__le64 apfs_last_mod_time;

	__le64 apfs_fs_flags;

/*110*/	struct apfs_modified_by apfs_formatted_by;
/*140*/	struct apfs_modified_by apfs_modified_by[APFS_MAX_HIST];

/*2C0*/	u8 apfs_volname[APFS_VOLNAME_LEN];
/*3C0*/	__le32 apfs_next_doc_id;

	__le16 apfs_role;
	__le16 reserved;

/*3C8*/	__le64 apfs_root_to_xid;
	__le64 apfs_er_state_oid;
} __packed;

struct volume_superblock {
	struct apfs_superblock *v_raw;
	struct btree *v_omap;
	struct btree *v_cat;
	struct btree *v_extent_ref;
	struct btree *v_snap_meta;
	union htable_entry **v_inode_table;	/* Hash table of all inodes */
	union htable_entry **v_dstream_table;	/* Hash table of all dstreams */
	union htable_entry **v_cnid_table;	/* Hash table of all cnids */
	union htable_entry **v_extent_table;	/* Hash table of all extents */

	/* Volume stats as measured by the fsck */
	u64 v_file_count;	/* Number of files */
	u64 v_dir_count;	/* Number of directories */
	u64 v_symlink_count;	/* Number of symlinks */
	u64 v_special_count;	/* Number of other filesystem objects */
	bool v_has_root;	/* Is there a root directory? */
	bool v_has_priv;	/* Is there a private directory? */

	/* Volume information read from the on-disk structure */
	u64 v_first_xid;	/* Transaction that created the volume */
	u64 v_next_obj_id;	/* Next cnid to be assigned */
	u32 v_next_doc_id;	/* Next document identifier to be assigned */

	struct object v_obj;		/* Object holding the volume sb */
};

/* Superblock data in memory */
struct super_block {
	struct apfs_nx_superblock *s_raw;
	struct btree *s_omap;
	unsigned long s_blocksize;
	unsigned char s_blocksize_bits;
	u64 s_block_count; /* Number of blocks in the container */
	u64 s_xid; /* Transaction id for the superblock */
	u64 s_next_oid;	/* Next virtual object id to be used */
	u32 s_max_vols; /* Maximum number of volumes allowed */
	u64 s_data_base; /* Base address of the checkpoint data area */
	u64 s_data_blocks; /* Number of blocks in the checkpoint data area */
	u32 s_data_index; /* Index of first valid block in checkpoint data */
	u32 s_data_len; /* Number of valid blocks in checkpoint data area */

	/* Hash table of ephemeral object mappings for the checkpoint */
	union htable_entry **s_cpoint_map_table;

	/* This is excessive in most cases.  TODO: switch to a linked list? */
	struct volume_superblock *s_volumes[APFS_NX_MAX_FILE_SYSTEMS];
};

/*
 * Checkpoint mapping data in memory
 */
struct cpoint_map {
	/* Hash table entry header (struct htable_entry_header from htable.h) */
	struct {
		union htable_entry	*m_next;
		u64			m_oid;	/* Ephemeral object id */
	};

	u32	m_type;		/* Type of the object */
	u32	m_subtype;	/* Subtype of the object */
	u64	m_paddr;	/* Physical address of the object */
	u32	m_size;		/* Size of the object in bytes */
};

static inline bool apfs_is_case_insensitive(void)
{
	extern struct volume_superblock *vsb;

	return (vsb->v_raw->apfs_incompatible_features &
		cpu_to_le64(APFS_INCOMPAT_CASE_INSENSITIVE)) != 0;
}

extern void parse_filesystem(void);

#endif	/* _SUPER_H */
