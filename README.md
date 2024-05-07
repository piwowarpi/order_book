# order_book

order_book is C++ application creating Order Book for given simple instrument

## Project requirements

C++ 17
CMake 3.14 - 3.28
package arpa/inet.h

## Building project

Choose directory where project will be build. Ex. BUILD

```bash
cmake -B BUILD
(cd BUILD && make)
```

## Input files

Inside input_files directory are instruction given by Sky Quant to build this Order Book. There is also ticks.raw - input binary file.
If you want to change binary file, you need to change settings inside ./src/CMakeLists.txt

## Output

Output ticks.csv will be stored inside result_files. To console (as required) will be throw duration in microseconds per tick and total duration in mircoseconds.
If you want to change name and location of output file, you need to change settings inside ./src/CMakeLists.txt

## Usage

After successful build process, you can run from root repository:

```bash
./BUILD/app/app
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

[MIT](https://choosealicense.com/licenses/mit/)