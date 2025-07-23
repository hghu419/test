#!/bin/bash
# 帮助信息函数
print_help() {
    cat <<EOF
使用说明:
用法: $0 [选项]

选项:
  -p, --particle <类型>      粒子类型 (默认: pi+)
                            可选值: e-, e+, mu-, mu+, pi+, pi-, proton, neutron
  -e, --energy <能量>        粒子能量 (默认: 2 GeV)
                            格式: <数值> <单位> (如: 100 MeV, 5 GeV)
  -m, --material <材料>      屏蔽材料 (默认: G4_Pb)
                            可选值: G4_Fe, G4_Pb, G4_W, G4_POLYETHYLENE, G4_CONCRETE
  -t, --thickness <厚度>     屏蔽厚度 (默认: 50 cm)
                            格式: <数值> <单位> (如: 10 cm, 0.5 m)
  -n, --particle_num <数量>  模拟粒子数量 (默认: 100000)
  -o, --output <前缀>        输出文件前缀 (默认: simulation)
  -h, --help                 显示此帮助信息

EOF
    exit 0
}

# 参数解析
while [[ $# -gt 0 ]]; do
    case "$1" in
        -p|--particle)
            PARTICLE_TYPE="$2"
            shift 2
            ;;
        -e|--energy)
            PARTICLE_ENERGY="$2"
            shift 2
            ;;
        -m|--material)
            SHIELD_MATERIAL="$2"
            shift 2
            ;;
        -t|--thickness)
            SHIELD_THICKNESS="$2"
            shift 2
            ;;
        -n| --particle_num)
            PARTICLE_NUM="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_PREFIX="$2"
            shift 2
            ;;
        -h|--help)
            print_help
            ;;
        *)
            echo "未知参数: $1"
            echo "请使用 -h 参数查看帮助信息。"
            exit 1
            ;;
    esac
done

# 设置默认值
: ${PARTICLE_TYPE:="pi+"}
: ${PARTICLE_ENERGY:="2 GeV"}
: ${SHIELD_MATERIAL:="G4_Pb"}
: ${SHIELD_THICKNESS:="50 cm"}
: ${PARTICLE_NUM:="100000"}
: ${OUTPUT_PREFIX:="simulation"}

# 清理文件名中的特殊字符
clean_name() {
    echo "$1" | tr -d ' ' | tr -s '_' '_'
}

# 生成唯一文件名
PARTICLE_CLEAN=$(clean_name "$PARTICLE_TYPE")
ENERGY_CLEAN=$(clean_name "$PARTICLE_ENERGY")
MATERIAL_CLEAN=$(clean_name "$SHIELD_MATERIAL")
THICKNESS_CLEAN=$(clean_name "$SHIELD_THICKNESS")
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
OUTPUT_FILE="${OUTPUT_PREFIX}_${PARTICLE_CLEAN}_${ENERGY_CLEAN}_${MATERIAL_CLEAN}_${THICKNESS_CLEAN}_${TIMESTAMP}.log"

# 生成临时MAC文件
TMP_MAC=$(mktemp)
cat > "$TMP_MAC" <<EOF
# 自动生成的MAC文件
/det/shieldThickness $SHIELD_THICKNESS
/det/shieldMaterial $SHIELD_MATERIAL
/run/initialize
/gun/particle $PARTICLE_TYPE
/gun/energy $PARTICLE_ENERGY
/run/beamOn $PARTICLE_NUM
EOF

echo "生成的MAC文件内容:"
echo "========================================"
cat "$TMP_MAC"
echo "========================================"
echo

# 运行模拟并提取结果
echo "开始运行模拟..."
echo "粒子类型: $PARTICLE_TYPE"
echo "粒子能量: $PARTICLE_ENERGY"
echo "屏蔽材料: $SHIELD_MATERIAL"
echo "屏蔽厚度: $SHIELD_THICKNESS"
echo "粒子数量: $PARTICLE_NUM"
echo "输出文件: $OUTPUT_FILE"
echo

# 运行程序并提取Run Summary块
#!/bin/bash

# [参数解析、默认值设置等保持不变...]

# 运行程序并精确提取 Merged Run Summary
./exampleB4a -m "$TMP_MAC" -t 5 2>&1 | awk '
    BEGIN {
        in_merged_block = 0
        merged_block = ""
        separator_count = 0
        found = 0
    }
    
    # 精确匹配 Merged Run Summary 开始行
    $0 ~ /^={5,} Merged Run Summary ={5,}$/ {
        in_merged_block = 1
        merged_block = $0
        separator_count = 0
        next
    }
    
    # 在块内时收集内容
    in_merged_block {
        # 保存当前行
        merged_block = merged_block "\n" $0
        
        # 检测分隔线（由等号组成的行）
        if ($0 ~ /^=+$/) {
            separator_count++
            
            # 当遇到第二个分隔线时结束块
            if (separator_count == 2) {
                in_merged_block = 0
                found = 1
                # 立即打印并退出
                print merged_block
                exit 0
            }
        }
    }
    
    # 处理所有行后检查是否找到
    END {
        if (!found) {
            print "错误: 未找到 Merged Run Summary 块" > "/dev/stderr"
            print "请检查程序输出" > "/dev/stderr"
            exit 1
        }
    }
' > "$OUTPUT_FILE"

# 检查 awk 退出状态
if [ $? -ne 0 ]; then
    echo "错误: 未能提取 Merged Run Summary"
    echo "临时输出已保存到 $OUTPUT_FILE"
    # 保留部分输出用于调试
else
    echo "模拟完成！结果保存在: $OUTPUT_FILE"
    echo "输出内容预览:"
    echo "========================================"
    cat "$OUTPUT_FILE"
    echo "========================================"
fi

# [清理临时文件保持不变...]

# 清理临时文件
rm -f "$TMP_MAC"

echo "模拟完成！结果保存在: $OUTPUT_FILE"
echo "输出内容预览:"
echo "========================================"
cat "$OUTPUT_FILE"
echo "========================================"