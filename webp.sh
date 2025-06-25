#!/usr/bin/env bash

# Function to check if cwebp is installed and install it if not
check_and_install_cwebp() {
    if ! command -v cwebp &>/dev/null; then
        echo "cwebp is not installed. Installing..."

        # Determine Linux distribution
        if [ -f /etc/os-release ]; then
            source /etc/os-release
            case $ID in
            ubuntu | debian)
                sudo apt-get update && sudo apt-get install -y webp
                ;;
            tencentos | centos | fedora | rhel)
                sudo yum install -y libwebp-tools
                ;;
            arch)
                sudo pacman -S --noconfirm libwebp
                ;;
            *)
                echo "Unsupported Linux distribution. Please install cwebp manually."
                exit 1
                ;;
            esac
        else
            echo "Cannot determine Linux distribution. Please install cwebp manually."
            exit 1
        fi
    fi
}

# Call the function to check and install cwebp
check_and_install_cwebp

# 指定要处理的图像类型
IMAGE_EXTENSIONS=("jpg" "jpeg" "png" "gif" "bmp" "tiff")

# 遍历当前目录及所有子目录
find docs -type f | while read -r file; do
    # 获取文件的扩展名
    extension="${file##*.}"

    # 判断文件是否是图像文件
    if [[ " ${IMAGE_EXTENSIONS[@]} " =~ " ${extension} " ]]; then
        # 获取文件的基础名称（去掉路径和扩展名）
        filename=$(basename -- "$file")
        filename_without_extension="${filename%.*}"

        # 生成新文件名（.webp 格式）
        new_file="${file%.*}.webp"

        # 提示用户确认转换
        read -p "是否将 $file 转换为 $new_file 并删除原图？[y/N] " confirm

        cwebp "$file" -o "$new_file"

        # 如果转换成功，删除原图
        if [[ $? -eq 0 ]]; then
            rm "$file"
            echo "$file 已成功转换为 $new_file 并删除原图。"
        else
            echo "$file 转换失败。"
        fi

    fi
done
