#!/usr/bin/env bash

set -xe

if [ $# -ne 1 ]; then
    echo "Usage: $0 <path to directory>"
    exit 1
fi

cd "$1"

spack containerize > "$1".def

# >>> docker mirror
DOCKER_MIRROR=mirror.ccs.tencentyun.com
if [ -n "${DOCKER_MIRROR}" ]; then
  # library/ prefix is required for docker mirror
  sed -i '/^From: [^/]*:[0-9.]*$/ s/^From: /From: library\//'  "$1".def
  sed -i "s/From: /From: ${DOCKER_MIRROR}\//" "$1".def
fi 
# <<< docker mirror

# >>> local cache
# /opt/spack/var/spack/cache
BUILD_FLAGS=(--bind "${SPACK_ROOT}/var/spack/cache:/opt/spack/var/spack/cache")
if [ ! -d "${SPACK_ROOT}/var/spack/cache" ]; then
    mkdir -p "${SPACK_ROOT}/var/spack/cache"
fi
sed -i '/^Stage: final/ r /dev/stdin' "$1".def <<'EOF'

%setup
  mkdir -p $APPTAINER_ROOTFS/opt/spack/var/spack/cache
EOF
# <<< local cache

# >>> modified repository
# /spack-packages
SPACK_PACKAGES="/root/spack-packages"
if [ -n "${SPACK_PACKAGES}" ]; then
  BUILD_FLAGS+=(--bind "${SPACK_PACKAGES}:/spack-packages")
  sed -i '/^Stage: build/ r /dev/stdin' "$1".def <<'EOF'

%setup
  mkdir -p $APPTAINER_ROOTFS/spack-packages
  mkdir -p $APPTAINER_ROOTFS/opt/spack/etc/spack
  cat << EOF_INNER > $APPTAINER_ROOTFS/opt/spack/etc/spack/repos.yaml
repos:
  local_packages: /spack-packages/repos/spack_repo/builtin
EOF_INNER
EOF
  sed -i '/^Stage: final/ r /dev/stdin' "$1".def <<'EOF'

%setup
  mkdir -p $APPTAINER_ROOTFS/spack-packages
EOF
fi
# <<< modified repository

# >>> debug mode
if [ "$DEBUG" == "true" ]; then
    if [ -d "./sandbox" ]; then
        BUILD_FLAGS+=(--update)
    fi
    BUILD_FLAGS+=(--no-cleanup --sandbox ./sandbox)
else
    BUILD_FLAGS+=("$1.sif")
fi
# <<< debug mode

apptainer build "${BUILD_FLAGS[@]}" "$1".def
