
#include "u_hash_map_structs_ct.h"

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = srunner_create(u_hash_map_structs_suite());
  srunner_set_log(sr, "u_hash_map_structs_ct.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
