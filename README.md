## Island Bridge sources

### Building

This project uses conan package manager. To compile the project using the default conan profile run

```bash
conan install . --output-folder=build --build=missing
cmake -B build -DCMAKE_TOOLCHAIN_FILE='build/conan_toolchain.cmake'
cmake --build build --config Release
```

On linux you may omin `--config`

### Testing

To test the algorithms run
```bash
build/Release/test
```

### Benchmarks

To reproduce results from the article first run the benchmark
```bash
build/Release/benchmarks --from 1 --to 1000 --csv --out bench.csv
```

And then produce plots using the following command. It will open a GUI window where you can select how to export the plot. `titel` is not a mistake.

```sh
gnuplot -p -e "titel='My title'; filename='bench.csv';" plot/benchmark.gnuplot
```