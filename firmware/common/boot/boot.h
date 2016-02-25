#ifndef __FIRMWARE__BOOT_H__
#define __FIRMWARE__BOOT_H__

int boot_clusters(int argc, char * const argv[]);
int boot_cluster(int clus_id, const char bin_file[], const char * argv[] );
void boot_set_nb_clusters(int nb_clusters);
int join_clusters(void);
int join_cluster(int clus_id, int *status);

#endif /* __FIRMWARE__BOOT_H__ */
