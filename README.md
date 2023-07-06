# Huffman Archiver

A command line tool for file compression using [canonical](https://en.wikipedia.org/wiki/Canonical_Huffman_code) [Huffman codes](https://en.wikipedia.org/wiki/Huffman_coding).

## Benchmarks

The archiver has been benchmarked on a dump of German wikipedia (24,26 GB of uncompressed XML files) on a 2022 MacBook Pro with M2 Pro. 

Compressing took about 124.4 seconds on average :
```bash
hyperfine --runs 5 'huffman -c -f samples/dewiki.xml -o dewiki.cmp'
Benchmark 1: huffman -c -f samples/dewiki.xml -o dewiki.cmp
  Time (mean ± σ):     124.394 s ±  0.140 s    [User: 102.721 s, System: 19.933 s]
  Range (min … max):   124.227 s … 124.545 s    5 runs
```

Decompressing the compressed file (16.7 GB) took 187.4 seconds on average:
```bash
 hyperfine --runs 5 'huffman -u -f dewiki.cmp -o dewiki.xml'
Benchmark 1: huffman -u -f dewiki.cmp -o dewiki.xml
  Time (mean ± σ):     187.407 s ± 11.534 s    [User: 160.005 s, System: 19.811 s]
  Range (min … max):   180.197 s … 206.779 s    5 runs
```

Which means that the compressing throughput is about 195 MB/s while for decompressing it is about 90 MB/s.

However, the code was optimized for x86 architecture, and as far as I remember, it achieved 300 MB/s for both compressing and decompressing on a 2019 Intel MacBook Pro.


## Building

The project has no extra dependencies and uses [CMake](https://cmake.org/) as a build system. Therefore, the build process is straightforward:
```bash
$ mkdir build && cd build
$ cmake .. && make
```

To install the tool, run from the same folder:
```bash
$ make install
```

To execute tests, run from the project root:
```bash
$ mkdir bin
$ build/tests
$ rm -rf bin
```

## Usage

You can always run with the `--help` flag to print the docs:
```bash
$ huffman --help
Usage: huffman_impl decoder [options]

Optional arguments:
-h --help       shows help message and exits
-v --version    prints version information and exits
-c              Compression mode [default: false]
-u              Decompression mode [default: false]
-f --file       Input file [required]
-o --output     Output file [required]
```

For example, let's compress and decompress the sample and check that the compression is non-lossy:
```bash
$ huffman -c -f samples/bible.txt -o bible.cmp
decompressed size: 4351186
compressed size: 2494322
additional data size: 269

$ huffman -u -f bible.cmp -o bible.txt       
compressed size: 2494322
decompressed size: 4351186
additional data size: 269

$ diff samples/bible.txt bible.txt
```
## Optimizations

### Avoiding virtual and system calls

Both `std::ifstream` and `std::ofstream` have built-in buffering, however, they are used behind `std::istream` and `std::ostream` interfaces which have virtual methods. Having additional buffering on top of those interfaces gave the most significant speedup.

### Optimizing for disk speed

Even though modern SSDs are fast for any kind of operations, sequential access is still much faster than random access. This is what Apache Kafka heavily exploits to achieve great performance despite the fact it is written in Java. Therefore, it makes sense for the algorithm to minimize the CPU workload and get close to the raw disk speed.

### Lowering the memory consumption

This archiver never fully stores input or output files in RAM. In fact, it has pretty constant memory footprint. In particular, this is why it compresses files in two passes. The first pass calculates the frequency of each byte and the second does the actual compression.

### Pleasing cache and branch prediction

We use integer numbers as buffers and bitwise operations everywhere to have a sliding window into the input and output files with bitwise precision. On top of these buffers we have a `bit_reader` and a `bit_writer` (see [fast_io.h](include/fast_io.h)).

This allows us to do fast compression: we just build the codes for each byte and store them as `uint64_t`. We then use the bit writer to dump the codes on disk really quickly.

For decoding, we also use a clever trick that is used in the gzip implementation and is explained well in [this article](https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art007). 

Briefly, we have a nested decoding table (see [decode_table.h](include/decode_table.h)) which tells for each combination of 16 bits whether it contains some code as a prefix or it is itself a prefix of some code. If it contains some code as a prefix, we know the length of the prefix and the value of the decoded byte, and we can just cut off this prefix and continue decoding. Otherwise, we skip all 16 bits and decode this byte using nested table.

I also tried to optimize branch prediction when checking for nested tables when the nesting depth is limited (which is often the case) by running different code with less branching for such cases but the approach didn't prove to be successful.


