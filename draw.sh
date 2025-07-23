#!/bin/bash

# 批量绘制ROOT文件中多个变量的分布图
# 用法: ./drawAll.sh [输入目录] [输出目录]

# 可执行程序路径 (根据实际情况修改)
DRAW_PROGRAM="./draw.out"

# 默认目录设置
INPUT_DIR="./build"     # 默认输入目录
# INPUT_DIR="./output/root/add_angle_info"
OUTPUT_DIR="./output/plots"   # 默认输出目录
# OUTPUT_DIR="./"

# 处理命令行参数
if [ $# -ge 1 ]; then
    INPUT_DIR="$1"
fi
if [ $# -ge 2 ]; then
    OUTPUT_DIR="$2"
fi

# 确保目录存在
mkdir -p "$INPUT_DIR"
mkdir -p "$OUTPUT_DIR"

# 要绘制的变量列表
VARIABLES=("E_out" "px_out" "py_out" "pz_out" "theta_out" "phi_out")

# 查找所有ROOT文件
find "$INPUT_DIR" -maxdepth 1 -type f -name "*.root" | while read rootfile; do
    # 提取文件名（不含路径和扩展名）
    filename=$(basename "$rootfile" .root)
    
    # 解析文件名各部分 (格式: particle_energy_engine_material_thickness_timestamp)
    IFS='_' read -ra parts <<< "$filename"
    
    if [ ${#parts[@]} -lt 6 ]; then
        echo "警告: 文件名格式不符合预期 - $filename"
        continue
    fi
    
    # 提取文件信息
    particle="${parts[0]}"
    energy="${parts[1]}"
    engine="${parts[2]}"
    material="${parts[3]}"
    thickness="${parts[4]}"
    timestamp="${parts[5]}"
    
    # 创建输出子目录 (按粒子类型分类)
    particle_dir="$OUTPUT_DIR/$particle"
    mkdir -p "$particle_dir"
    
    echo "处理: $filename"
    echo "----------------------------------------"
    
    # 绘制每个变量
    for var in "${VARIABLES[@]}"; do
        # 构建输出文件名
        outfile="${particle_dir}/${particle}_${energy}_${material}_${thickness}_${var}.png"
        
        # 执行绘图命令
        echo "正在绘制: $var"
        $DRAW_PROGRAM "$rootfile" "tree" "$var" -o "$outfile"

        # root -l -q ''${DRAW_PROGRAM}'("'${rootfile}'", "tree", "'${var}'", "'${outfile}'")'

        # 检查执行结果
        if [ $? -eq 0 ] && [ -f "$outfile" ]; then
            echo "保存到: $outfile"
        else
            echo "错误: 无法生成 $var 的分布图"
        fi
        echo
    done
    
    echo "完成处理: $filename"
    echo "========================================"
    echo
done

echo "所有文件处理完成!"
echo "输出目录: $OUTPUT_DIR"