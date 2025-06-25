#!/usr/bin/env bash

dst_root=../docs.zjusct.io

src=(
    docs/高性能/网络
)

dst=(
    docs/optimization/HPN
)

# check if root exist
if [[ ! -d "$dst_root" ]]; then
    echo "Destination root directory $dst_root does not exist."
    exit 1
fi

for i in "${!src[@]}"; do
    if [[ -d "${src[$i]}" ]]; then
        rsync -av --delete "${src[$i]}/" "${dst_root}/${dst[$i]}/"
    else
        echo "Source ${src[$i]} is not a directory."
    fi
done
