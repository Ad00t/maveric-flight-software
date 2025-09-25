#! /bin/csh

set t_out=../maestro-programs/sboot
set create_sboot=./create_sboot
# set verbose_f="--verbose"
set verbose_f=""

foreach test_name (crc)
    echo "Processing ${test_name}"
    ${create_sboot} ${verbose_f} \
      --test-name ${test_name} --test-type ${test_name} \
      --no-l1-file \
      --no-enable-maestro \
      --no-reset-before-divisor-check \
      --no-check-uart-divisor-low-speed \
      --no-high-speed \
      --no-check-uart-divisor-high-speed \
      --program-timeout 600 \
      --sboot ${t_out}/${test_name}.sboot
end
