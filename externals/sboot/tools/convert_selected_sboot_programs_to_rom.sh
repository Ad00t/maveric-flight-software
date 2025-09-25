#!/bin/sh

# The first two tests should succeed.
# The third r test, l2_data, should fail.

./convert_sboot_to_code \
  --output ../maestro-programs/rom/maestro_test_programs.c \
  ../maestro-programs/sboot/itc_chip_bringup_basic2_test.sboot \
  ../maestro-programs/sboot/itc_chip_bringup_fpu_test.sboot \
  ../maestro-programs/sboot/itc_chip_bringup_l2_dapaths_test.sboot \
  ../maestro-programs/sboot/itc_chip_bringup_l2_data_test.sboot \
  ../maestro-programs/sboot/crc.sboot

