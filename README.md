# ORC (OpenWrt RESTCONF)

This is a prototype implementation of RESTCONF for the OpenWrt system that utilizes the UCI configuration files as a
datastore.

## Requirements

1. Python 3 for the YANG conversion script
2. Tool for converting YANG to YIN

## Adding YANG modules

To add YANG modules for OpenWrt they have to go through some pre-processing. This is
what the `./yin2json/yin2json.py` script does.

### Annotations

Before YANG modules can be used with this implementation they have to be
annotated with the extensions provided in `/yang/openwrt-uci-extension.yang`.
An example of an annotated module is `/yang/restconf-example.yang`

### Script

1. Convert the YANG modules to be included to YIN and put them in one folder, i.e. `/yin`.
   For example with [`pyang`](https://github.com/mbj4668/pyang)
   ```console
   pyang -f yin ./yang/restconf-example.yang -p ./yang -o ./yin/restconf-example.yin 
   ```
2. Run the `main.py` script
   ```console
   python3 ./yin2json/yin2json.py -y ./yin -o ./generated ./yin/restconf-example.yin ...
   ```
   This converts the YIN files and generates a `.h` file in `./generated` that has to be included in `/src/generated/yang.h`

## Building

1. Clone this repository
2. `Docker pull mgranderath/openwrt-build`
3. `docker run -v $(pwd):/restconf mgranderath/openwrt-build`
4. The generated `.ipk` will be in the `build` folder

## Architecture

![Architecture](docs/resources/Architecture.png)

## Testing

The tests are inside the `/test` directory and are based on the Python
[Tavern Testing Framework](https://github.com/taverntesting/tavern). After
installing the framework the tests can be run using either of the
following commands:

```console
tavern-ci ./test/test_restconf.tavern.yml
# or
py.test ./test/test_restconf.tavern.yml
```

This will run integration tests that check the actual implementation. The
url where the server is located can be changed in `/test/common.yaml`.