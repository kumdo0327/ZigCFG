ROOT="$(dirname "$(readlink -f "$0")")/.."
CMAKE_FORWARDED_OPTIONS=()
for arg_name in "$@"; do
    CMAKE_FORWARDED_OPTIONS+=("-D${arg_name}=ON")
done

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DCMAKE_BUILD_TYPE=Debug \
      "${CMAKE_FORWARDED_OPTIONS[@]}" \
      -S "$ROOT" \
      -B "$ROOT/build/debug"

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DCMAKE_BUILD_TYPE=Release \
      "${CMAKE_FORWARDED_OPTIONS[@]}" \
      -S "$ROOT" \
      -B "$ROOT/build/release"