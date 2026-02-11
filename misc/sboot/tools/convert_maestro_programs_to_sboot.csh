#! /bin/csh

set t_in=../maestro-programs/comp_out
set t_out=../maestro-programs/sboot
set create_sboot=./create_sboot
set verbose_f="--verbose"

foreach test_name (basic2 l1_dapaths l1_data l1_tag l2_dapaths l2_data l2_tag fpu)
    set full_test_name=itc_chip_bringup_${test_name}_test
    echo "Processing ${full_test_name}"
    ${create_sboot} ${verbose_f} \
      --test-name ${full_test_name} --test-type l1 \
      --l1 ${t_in}/${full_test_name}/${full_test_name}.l1boot \
      --sboot ${t_out}/${full_test_name}.sboot
end

# For an unknown reason, the "imesh" test doesn't follow the
# same naming convention as the tests above.

set test_name=imesh
set full_test_name1=itc_chip_bringup_${test_name}_test
set full_test_name2=itc_${test_name}_test
echo "Processing ${full_test_name1}"
${create_sboot} ${verbose_f} \
  --test-name ${full_test_name1} --test-type l1 \
  --l1 ${t_in}/${full_test_name1}/${full_test_name2}.l1boot \
  --sboot ${t_out}/${full_test_name2}.sboot
