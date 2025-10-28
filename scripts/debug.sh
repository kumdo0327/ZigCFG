ROOT="$(dirname "$(readlink -f "$0")")/.."
make -C "$ROOT/build/debug" "$@"