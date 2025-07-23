#!/bin/bash

# 定义参数数组
PARTICLES=("pi+" "pi-" "mu+" "mu-")
ENERGIES=("1 GeV" "1.5 GeV" "2 GeV" "3 GeV" "4 GeV" "5 GeV" "6 GeV" "7 GeV")
MATERIALS=("G4_Fe" "G4_Cu" "G4_Pb")
THICKNESSES=("50 cm" "60 cm" "70 cm" "80 cm" "90 cm" "100 cm")
Number=("100000")

# 遍历所有参数组合
for particle in "${PARTICLES[@]}"; do
  for material in "${MATERIALS[@]}"; do
    for thickness in "${THICKNESSES[@]}"; do

      echo "============================================================"
      echo "启动批次: $particle | $material $thickness —— 并行所有能量"
      echo "============================================================"
      echo

      # 对当前批次的每个能量启动后台任务
      for energy in "${ENERGIES[@]}"; do
        echo "  启动子任务: $particle @ $energy | $material $thickness"
        sh run_simulation.sh \
          -p "$particle" \
          -e "$energy" \
          -m "$material" \
          -t "$thickness" \
          -n "$Number" \
          -o "batch_run" &

      done

      # 等待本批次所有能量的模拟全部完成
      wait
      echo
      echo "批次完成: $particle | $material $thickness"
      echo

    done
  done
done

echo "所有批量运行完成！"
