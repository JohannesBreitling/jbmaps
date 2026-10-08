
config mode: # Config the environment for debug
    cmake -S . -B ./build/{{mode}} -DCMAKE_BUILD_TYPE={{mode}}

[group('debug')]
build-debug target='': # Build the target in debug mode
    cmake --build ./build/debug --target {{target}}

[group('debug')]
run-debug target +params='':
    ./build/debug/{{target}} {{params}}

[group('release')]
build-release target='': # Build the target in release mode
    cmake --build ./build/release --target {{target}}


viz-sources:
    python3 ./scripts/visualize_vertices.py ./output/karlsruhe/edge_coords_start.txt ./output/karlsruhe/edge_coords_end.txt ./output/random_sources.txt